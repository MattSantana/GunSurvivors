#include "Enemy.h"
#include "Kismet/GameplayStatics.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	SetRootComponent(CapsuleComp);

	EnemyFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("EnemyFlipbook"));
	EnemyFlipbook->SetupAttachment(RootComponent);

}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();


	//getting a player reference to make the enemy AI
	if (!Player)
	{
		AActor* PlayerActor = UGameplayStatics::GetActorOfClass(GetWorld(), ATopDownCharacter::StaticClass());

		if (PlayerActor)
		{
			Player = Cast<ATopDownCharacter>(PlayerActor);

			CanFollow = true;
		}
	}
	
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (IsAlive && CanFollow && Player)
	{
		//Move towards the player

		FVector CurrentLocation = GetActorLocation();
		FVector PlayerLocation = Player->GetActorLocation();

		FVector DirectionToPlayer = PlayerLocation - CurrentLocation;

		float DistanceToPlayer = DirectionToPlayer.Length();

		if (DistanceToPlayer >= StopDistance)
		{
			DirectionToPlayer.Normalize();

			FVector NewLocation = CurrentLocation + (DirectionToPlayer * MovementSpeed * DeltaTime);

			SetActorLocation(NewLocation);
		}

		//Face the player

		CurrentLocation = GetActorLocation();
		float FlipbookXScale = EnemyFlipbook->GetComponentScale().X;

		if ( (PlayerLocation.X - CurrentLocation.X) >= 0.0f )// If is greater than 0, playr is on the right side relative to the enemy
		{
			if (FlipbookXScale < 0.0f){
				EnemyFlipbook->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
			}
		}
		else //Player is on the left side of the enemy
		{
			if (FlipbookXScale > 0.0f) {
				EnemyFlipbook->SetWorldScale3D(FVector(-1.0f, 1.0f, 1.0f));
			}
		}
	}

}

void AEnemy::Die()
{
	if (!IsAlive) { return; }

	IsAlive = false;
	CanFollow = false;

	EnemyFlipbook->SetFlipbook(DeadFlipbookAsset);
	EnemyFlipbook->SetTranslucentSortPriority(-5);

	GetWorldTimerManager().SetTimer(DestroyTimer, this, &AEnemy::OnDestroyTimerTimeout, 1.0f, false, 10.0f);
}

void AEnemy::OnDestroyTimerTimeout()
{
	Destroy();
}

