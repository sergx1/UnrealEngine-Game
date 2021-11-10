// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Main.generated.h"

UCLASS()
class FIRSTPROJECT_API AMain : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMain();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	float CharacterSpeedMultiplier;

	bool bShiftPressed;

	float MaxSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Death")
	bool bIsDead;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats | Stamina")
	float StaminaDecreaseMultiplier;

	/* Player Stats */

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats | Health")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats | Health")
	float Health;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats | Stamina")
	float MaxStamina;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats | Stamina")
	float Stamina;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats | Experience")
	int32 Exp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats | Experience")
	int32 TotalExp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats | Lvl")
	int32 Lvl;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats | Damage");
	float Damage;
	
	FTimerHandle SwitchHandle;
	float SwitchTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Items");
	class AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items");
	class AItem* ActiveOverlappingItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundCue* HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundCue* DeathSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundCue* LvlUpSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UParticleSystemComponent* LvlUpParticles;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void TurnAtRate(float Rate);

	void LookUpRate(float Rate);

	void Run();

	void StopRunning();

	void ChangeCamera();

	void NPressed();

	void IncreaseExp(int32 Amount);
	
	void IncreaseHealth(float Amount);

	void LowerHealth(float Amount);

	void Die();

	void EquipItem();

	void LMBUp();

	void LMBDown();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Death")
	bool bLMBDown;

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	void SetEquippedWeapon(AWeapon* WeaponToSet);
	FORCEINLINE AWeapon* GetEquippedWeapon() { return EquippedWeapon; }

	FORCEINLINE void SetActiveOverlappingItem(AItem* Item) { ActiveOverlappingItem = Item; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bAttacking;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* CombatMontage;

	void Attack();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class AEnemy* CombatTarget;

	// FORCEINLINE AEnemy* GetCombatTarget() { return Enemy; }
	FORCEINLINE void SetCombatTarget(AEnemy* Enemy) { CombatTarget = Enemy; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bHasCombatTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
	class AMainPlayerController* MainPlayerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	FVector CombatTargetLocation;

	void SwitchLevel(FName LevelName);
};
