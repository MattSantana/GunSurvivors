#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "PaperSpriteComponent.h"
#include "PaperFlipbookComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/Controller.h"

#include "Engine/TimerHandle.h"

#include "Bullet.h"

#include "TopDownCharacter.generated.h"

UCLASS()
class GUNSURVIVORS_API ATopDownCharacter : public APawn
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UCapsuleComponent* CapsuleComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UPaperFlipbookComponent* CharacterFlipbook;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USceneComponent* GunParent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UPaperSpriteComponent* GunSprite;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USceneComponent* BulletSpawnPosition;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* ShootAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UPaperFlipbook* IdleFlipBook;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UPaperFlipbook* RunFlipBook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D HorizontalLimits;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D VerticalLimits;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ABullet>BulletActorToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovementSpeed = 100.0f;

	UPROPERTY(BlueprintReadWrite)
	FVector2D MovementDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool CanMove = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool CanShoot = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ShootCooldownDurationInSeconds = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BulletSpeed = 100.0f;

	FTimerHandle ShootCooldownTimer;

	ATopDownCharacter();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveTriggred(const FInputActionValue& Value);
	void MoveCompleted(const FInputActionValue& Value);

	void Shoot(const FInputActionValue& Value);

	bool IsInMapBoundsHorizontal(float XPos);
	bool IsInMapBoundsVertical(float ZPos);

	void OnShootCoooldownTimerTimeout();
};
