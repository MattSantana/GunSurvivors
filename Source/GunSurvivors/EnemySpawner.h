#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Enemy.h"
#include "Engine/TimerHandle.h"

#include "EnemySpawner.generated.h"

UCLASS()
class GUNSURVIVORS_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AEnemy> EnemyActorToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnTime = 1.0f;

	FTimerHandle SpawnTimer;

	AEnemySpawner();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void OnSpawnTimerTimeout();
	void StartSpawning();
	void StopSpawning();
	void SpawnEnemy();

};
