#include "EnemySpawner.h"
#include "Kismet/GameplayStatics.h"

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	StartSpawning();
	
}

void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemySpawner::StartSpawning()
{
	GetWorldTimerManager().SetTimer(SpawnTimer, this, &AEnemySpawner::OnSpawnTimerTimeout, SpawnTime, true, SpawnTime);
}
void AEnemySpawner::StopSpawning()
{
	GetWorldTimerManager().ClearTimer(SpawnTimer);
}

void AEnemySpawner::OnSpawnTimerTimeout()
{
	SpawnEnemy();
}

void AEnemySpawner::SpawnEnemy()
{
	FVector2D RandomPostion = FVector2D(FMath::VRand());
	RandomPostion.Normalize();
	RandomPostion *= SpawnDistance;


	FVector EnemyLocation = GetActorLocation() + FVector(RandomPostion.X, 0, RandomPostion.Y);
	AEnemy* Enemy = GetWorld()->SpawnActor <AEnemy> (EnemyActorToSpawn, EnemyLocation, FRotator::ZeroRotator);
}