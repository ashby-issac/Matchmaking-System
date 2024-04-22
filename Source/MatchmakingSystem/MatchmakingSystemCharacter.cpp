// Copyright Epic Games, Inc. All Rights Reserved.

#include "MatchmakingSystemCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// AMatchmakingSystemCharacter

AMatchmakingSystemCharacter::AMatchmakingSystemCharacter() : 
	CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
	JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Blue,
				FString::Printf(TEXT("Found subsystem %s"), *OnlineSubsystem->GetSubsystemName().ToString())
			);
		}
	}
}

void AMatchmakingSystemCharacter::CreateGameSession()
{
	if (!OnlineSessionInterface.IsValid()) { return; }
		
	auto ExistingSession = OnlineSessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		OnlineSessionInterface->DestroySession(NAME_GameSession);
	}

	OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);
	ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	
	SessionSettings = MakeShareable(new FOnlineSessionSettings());
	SessionSettings->bIsLANMatch = false;
	SessionSettings->NumPublicConnections = 4;
	SessionSettings->bAllowJoinInProgress = true;
	SessionSettings->bAllowJoinViaPresence = true;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bUsesPresence = true;
	SessionSettings->bUseLobbiesIfAvailable = true;
	SessionSettings->Set(FName("MatchType"), FString("FreeForAll"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	OnlineSessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *SessionSettings);
}

void AMatchmakingSystemCharacter::JoinGameSession()
{
	if (!OnlineSessionInterface.IsValid())
	{
		return;
	}

	OnlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);
	SearchSettings = MakeShareable(new FOnlineSessionSearch());

	SearchSettings->MaxSearchResults = 10000;
	SearchSettings->bIsLanQuery = false;
	SearchSettings->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	OnlineSessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SearchSettings.ToSharedRef());
}

void AMatchmakingSystemCharacter::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (!GEngine) { return; }
	
	if (bWasSuccessful)
	{
		GetWorld()->ServerTravel("/Game/ThirdPerson/Maps/Lobby?listen");
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Green,
			FString::Printf(TEXT("Successfully created game session: %s"), *SessionName.ToString())
		);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Red,
			FString::Printf(TEXT("Failed to create game session: %s"), *SessionName.ToString())
		);
	}
}

void AMatchmakingSystemCharacter::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (!GEngine || !OnlineSessionInterface.IsValid()) 
	{
		return;
	}

	if (bWasSuccessful)
	{
		for (auto SearchResult : SearchSettings->SearchResults)
		{
			auto Id = SearchResult.GetSessionIdStr();
			auto User = SearchResult.Session.OwningUserName;

			FString MatchType;
			SearchResult.Session.SessionSettings.Get(FName("MatchType"), MatchType);
			
			if (MatchType.Equals("FreeForAll"))
			{
				OnlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

				ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
				OnlineSessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SearchResult);
				GEngine->AddOnScreenDebugMessage(
					-1,
					15.f,
					FColor::Cyan,
					FString::Printf(TEXT("Found FreeForAll Session with Id: %s and UserName: %s"), *Id, *User)
				);
				break;
			}
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Red,
			FString::Printf(TEXT("No sessions found"))
		);
	}
}

void AMatchmakingSystemCharacter::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!OnlineSessionInterface.IsValid() || !GEngine)
	{
		return;
	}

	FString ConnectInfo;
	if (OnlineSessionInterface->GetResolvedConnectString(SessionName, ConnectInfo))
	{
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Green,
				FString::Printf(TEXT("Joined Session Successfully with connection info: %s"), *ConnectInfo)
			);
			PlayerController->ClientTravel(ConnectInfo, ETravelType::TRAVEL_Absolute);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMatchmakingSystemCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AMatchmakingSystemCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AMatchmakingSystemCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AMatchmakingSystemCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AMatchmakingSystemCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AMatchmakingSystemCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AMatchmakingSystemCharacter::TouchStopped);
}

void AMatchmakingSystemCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AMatchmakingSystemCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void AMatchmakingSystemCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AMatchmakingSystemCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AMatchmakingSystemCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMatchmakingSystemCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

