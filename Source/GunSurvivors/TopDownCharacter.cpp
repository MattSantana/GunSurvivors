#include "TopDownCharacter.h"


ATopDownCharacter::ATopDownCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	SetRootComponent(CapsuleComp);

	CharacterFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("CharacterFlipbook"));
	CharacterFlipbook->SetupAttachment(RootComponent);

}

void ATopDownCharacter::BeginPlay()
{
	Super::BeginPlay();
	

	// Using the Built in variable called Controller to get the player controller 
	APlayerController* PlayerController = Cast<APlayerController>(Controller);

	//checking if the cast was successful or not
	if (PlayerController)
	{
		//I need the Subsystem to use the AddMappingContext method, and add it tho the player. That's what i'm doing here

		//get the subsystem from the PlayerController
		UEnhancedInputLocalPlayerSubsystem* Subsystem = 
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

		//check if Subsystem is valid. If it is, we asign the Input mapping context
		if (Subsystem)
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}
}

void ATopDownCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CanMove)
	{
		//checking if the buttons are being pressed
		if (MovementDirection.Length() > 0.0f)
		{
			//If player press 2 movement keys at same time
			if (MovementDirection.Length() > 1.0f)
			{
				//Normalize diagonal movement 
				MovementDirection.Normalize();
			}

			FVector2D DistanceToMove = MovementDirection * MovementSpeed * DeltaTime;

			//Unreal uses 3Dimantion location. Get that and use only X and Z axis (We don't use Y for 2D Games)
			FVector CurrentLocation = GetActorLocation();

			//calculate new Location
			FVector NewLocation = CurrentLocation + FVector(DistanceToMove.X, 0.0f, DistanceToMove.Y);
		
			//move the player to the new location
			SetActorLocation(NewLocation);
		}
	}
}

void ATopDownCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (EnhancedInputComponent)
	{
		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, 
			&ATopDownCharacter::MoveTriggred);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this,
			&ATopDownCharacter::MoveCompleted);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Canceled, this,
			&ATopDownCharacter::MoveCompleted);

		//Shoot
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this,
			&ATopDownCharacter::Shoot);
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this,
			&ATopDownCharacter::Shoot);
	}
}

void  ATopDownCharacter::MoveTriggred(const FInputActionValue& Value)
{
	FVector2D MoveActionValue = Value.Get<FVector2D>();

	if (CanMove)
	{
		MovementDirection = MoveActionValue;
	}
}

	
void  ATopDownCharacter::MoveCompleted(const FInputActionValue& Value)
{
	MovementDirection = FVector2D::ZeroVector;
}

void  ATopDownCharacter::Shoot(const FInputActionValue& Value)
{

}

