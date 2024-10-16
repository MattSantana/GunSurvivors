#include "TopDownCharacter.h"
#include "Kismet/KismetMathLibrary.h"



ATopDownCharacter::ATopDownCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	SetRootComponent(CapsuleComp);

	CharacterFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("CharacterFlipbook"));
	CharacterFlipbook->SetupAttachment(RootComponent);

	GunParent = CreateDefaultSubobject<USceneComponent>(TEXT("GunParent"));
	GunParent->SetupAttachment(RootComponent);

	GunSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("GunSprite"));
	GunSprite->SetupAttachment(GunParent);

	BulletSpawnPosition = CreateDefaultSubobject<USceneComponent>(TEXT("BulletSpawnPosition"));
	BulletSpawnPosition->SetupAttachment(GunSprite);

}

void ATopDownCharacter::BeginPlay()
{
	Super::BeginPlay();
	

	// Using the Built in variable called Controller to get the player controller 
	APlayerController* PlayerController = Cast<APlayerController>(Controller);

	//checking if the cast was successful or not
	if (PlayerController)
	{
		PlayerController->SetShowMouseCursor(true);

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

bool ATopDownCharacter::IsInMapBoundsHorizontal(float XPos)
{
	bool Result = true;

	Result = (XPos > HorizontalLimits.X) && (XPos < HorizontalLimits.Y);

	return Result;
}

bool ATopDownCharacter::IsInMapBoundsVertical(float ZPos)
{
	bool Result = true;

	Result = (ZPos > VerticalLimits.X) && (ZPos < VerticalLimits.Y);

	return Result;
}

void ATopDownCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//move the player
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
			FVector NewLocation = CurrentLocation + FVector(DistanceToMove.X, 0.0f, 0.0f);
			
			if (!IsInMapBoundsHorizontal(NewLocation.X))
			{
				NewLocation-= FVector(DistanceToMove.X, 0.0f, 0.0f);
			}
			
			NewLocation += FVector(0.0f, 0.0f, DistanceToMove.Y);

			if (!IsInMapBoundsVertical(NewLocation.Z))
			{
				NewLocation -= FVector(0.0f, 0.0f, DistanceToMove.Y);
			}

			//move the player to the new location
			SetActorLocation(NewLocation);
		}
	}

	//Rotate the gun
	
	//We need the player controller to find the mouse position at the world
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		FVector MouseWorldLocation, MouseWorldDirection;
		PlayerController->DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection);
		
		FVector CurrentLocation = GetActorLocation();
		FVector Start = FVector(CurrentLocation.X, 0.0f, CurrentLocation.Z);
		FVector Target = FVector(MouseWorldLocation.X, 0.0f, MouseWorldLocation.Z);
		
		FRotator GunParentRotator = UKismetMathLibrary::FindLookAtRotation(Start, Target);
		
		GunParent->SetRelativeRotation(GunParentRotator);
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

		CharacterFlipbook->SetFlipbook(RunFlipBook);

		FVector FlipbookScale = CharacterFlipbook->GetComponentScale();


		//checking which direction we're going
		//inside of it, checking if the flipbook comp already has the scale we want. 
		//if not, we change it once.
		if (MovementDirection.X < 0.0f)
		{
			if (FlipbookScale.X > 0.0f)
			{
				CharacterFlipbook->SetWorldScale3D(FVector(-1.0f, 1.0f, 1.0f));
			}
		}
		else if (MovementDirection.X > 0.0f)
		{
			if (FlipbookScale.X < 0.0f)
			{
				CharacterFlipbook->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
			}
		}
	}
}

	
void  ATopDownCharacter::MoveCompleted(const FInputActionValue& Value)
{
	MovementDirection = FVector2D::ZeroVector;

	CharacterFlipbook->SetFlipbook(IdleFlipBook);
}

void  ATopDownCharacter::Shoot(const FInputActionValue& Value)
{
	//We're basically saying that we have to wait the timer finish, so can shoot become true
	//and we're able to shoot again.
	if(CanShoot) 
	{ 
		CanShoot = false; 

		//bulletSpawn actor code
		ABullet* Bullet = GetWorld()->SpawnActor<ABullet>( BulletActorToSpawn, BulletSpawnPosition->GetComponentLocation(), FRotator::ZeroRotator);

		check(Bullet);

		//get mouse world location
		APlayerController* PlayerController = Cast<APlayerController>(Controller);

		check(PlayerController);

		FVector MouseWorldLocation, MouseWorldDirection;
		PlayerController->DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection);

		//calculate bullet direction
		FVector CurrentLocation = GetActorLocation();
		FVector2D BulletDirection = FVector2D(MouseWorldLocation.X - CurrentLocation.X, MouseWorldLocation.Z - CurrentLocation.Z);

		BulletDirection.Normalize();

		//Launch the bullet
		Bullet->Launch(BulletDirection, BulletSpeed);

		GetWorldTimerManager().SetTimer(ShootCooldownTimer, this, &ATopDownCharacter::OnShootCoooldownTimerTimeout, 1.0f, false, ShootCooldownDurationInSeconds);

	}
	
	
}

void ATopDownCharacter::OnShootCoooldownTimerTimeout()
{
	CanShoot = true;
}

