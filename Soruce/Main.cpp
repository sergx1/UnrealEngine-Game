// Fill out your copyright notice in the Description page of Project Settings.


#include "Main.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "MainPlayerController.h"
#include "Enemy.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AMain::AMain()
{	
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Pravi CameraBoom(primice se igracu ako dodje do sudara).
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f; // udaljenost kamere od igraca.
	CameraBoom->bUsePawnControlRotation = true; // rotacija bazirana na igracu.

	GetCapsuleComponent()->SetCapsuleSize(48.f, 105.f);

	// Pravi FollowCamera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	LvlUpParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("LvlUp Particle"));
	LvlUpParticles->SetupAttachment(GetRootComponent());

	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	// Kamera se nece pomerati zajedno sa karakterom.
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in direction of input.
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 420.f, 0.0f); // at this rotation rate, only in yaw direction.
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	MaxSpeed = 300.f;
	GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;
	CharacterSpeedMultiplier = 1.8f;

	bShiftPressed = false;
	StaminaDecreaseMultiplier = 10.f;


	bIsDead = false;
	bLMBDown = false;
	
	MaxHealth = 100.f;
	Health = 100.f;
	MaxStamina = 100.f;
	Stamina = 80.f;
	Exp = 0;
	TotalExp = 0;
	Lvl = 1;
	Damage = 0.f;

	SwitchTime = 10.f;

	bHasCombatTarget = false;


}

// Called when the game starts or when spawned
void AMain::BeginPlay()
{
	Super::BeginPlay();

	MainPlayerController = Cast<AMainPlayerController>(GetController());
	LvlUpParticles->DeactivateSystem();

}

// Called every frame
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Stamina <= 0 && bShiftPressed) {
		// Health -= 10 * DeltaTime;
		StopRunning();
	}

	if (bShiftPressed && Stamina > 0.f && (GetCharacterMovement()->Velocity.X != 0 || GetCharacterMovement()->Velocity.Y != 0)) {
		Stamina -= StaminaDecreaseMultiplier * DeltaTime;
	}
	else if (bShiftPressed && Stamina < MaxStamina && (GetCharacterMovement()->Velocity.Z != 0 || GetCharacterMovement()->Velocity == FVector(0.f, 0.f, 0.f))) {
		Stamina += 2* StaminaDecreaseMultiplier * DeltaTime;
	}
	else if (!bShiftPressed && Stamina < MaxStamina) {
		Stamina += 2 * StaminaDecreaseMultiplier * DeltaTime;
	}

	/*if (CombatTarget) {
		CombatTargetLocation = CombatTarget->GetActorLocation();
		if (MainPlayerController) {
			MainPlayerController->EnemyLocation = CombatTargetLocation;
		}
	}*/

}

// Called to bind functionality to input
void AMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);


	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMain::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMain::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAxis("TurnRate", this, &AMain::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMain::LookUpRate);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AMain::Run);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &AMain::StopRunning);

	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMain::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMain::LMBUp);

	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &AMain::EquipItem);

	PlayerInputComponent->BindAction("ChangeCamera", IE_Pressed, this, &AMain::ChangeCamera);

	PlayerInputComponent->BindAction("NPressed", IE_Pressed, this, &AMain::NPressed);


}

void AMain::MoveForward(float Value) {
	if (Controller != nullptr && Value != 0.0f && !bAttacking) {
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMain::MoveRight(float Value) {
	if (Controller != nullptr && Value != 0.0f && !bAttacking) {
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AMain::TurnAtRate(float Rate) {
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMain::LookUpRate(float Rate) {
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMain::Run() {
	if (Stamina > 0.f) {
		GetCharacterMovement()->MaxWalkSpeed *= CharacterSpeedMultiplier;
		bShiftPressed = true;
	}
}

void AMain::StopRunning() {
	if (GetCharacterMovement()->MaxWalkSpeed > MaxSpeed) {
		GetCharacterMovement()->MaxWalkSpeed /= CharacterSpeedMultiplier;
		bShiftPressed = false;
	}
}

void AMain::ChangeCamera() {
	if (CameraBoom->TargetArmLength == 600.f) {
		CameraBoom->TargetArmLength /= 2;
	}
	else {
		CameraBoom->TargetArmLength *= 2;
	}
}

void AMain::EquipItem() {
	if (ActiveOverlappingItem) {
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
		if (Weapon) {
			Weapon->Equip(this);
			SetActiveOverlappingItem(nullptr);
		}
	}
}

void AMain::SetEquippedWeapon(AWeapon* WeaponToSet) {
	if (EquippedWeapon) {
		EquippedWeapon->Destroy();
	}
	EquippedWeapon = WeaponToSet;
}


void AMain::LMBUp() {
	bLMBDown = false;
}

void AMain::LMBDown() {
	bLMBDown = true;
	if (EquippedWeapon) {
		Attack();
	}
}

void AMain::NPressed() {
	IncreaseExp(1000); // For debugging
}

void AMain::IncreaseExp(int32 Amount) {
	if (Lvl >= 100) return;
	if (Amount + Exp >= 1000) {
		Lvl = Lvl + ((Amount + Exp) / 1000);
		Exp = (Amount + Exp) % 1000;
		MaxHealth += Lvl * 5;
		Health = MaxHealth;
		if (EquippedWeapon) {
			Damage = Lvl * 10 + EquippedWeapon->Damage;
		}
		UGameplayStatics::PlaySound2D(this, LvlUpSound);
		LvlUpParticles->ActivateSystem(true);
	}
	else {
		Exp += Amount;
	}
	TotalExp += Amount;

}

void AMain::LowerHealth(float Amount) {
	if (Health <= 0) return;
	if (Health - Amount <= 0) {
		Health -= Amount;
		UGameplayStatics::PlaySound2D(this, DeathSound);
		GetMesh()->GetAnimInstance()->StopAllMontages(0);
		bIsDead = true;
		GetWorldTimerManager().SetTimer(SwitchHandle, this, &AMain::Die, SwitchTime);
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		this->DisableInput(PlayerController);
	}
	else {
		Health -= Amount;
	}
}

void AMain::IncreaseHealth(float Amount) {
	if (Health + Amount > MaxHealth) {
		Health = MaxHealth;
	}
	else {
		Health += Amount;
	}
}

void AMain::Attack() {
	if (!bAttacking) {
		bAttacking = true;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMontage) {
			int32 Section = FMath::RandRange(0, 1);
			if (Section == 0) {
				AnimInstance->Montage_Play(CombatMontage, 2.3f);
				AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
				if (EquippedWeapon->OnAttack1Sound) {
					UGameplayStatics::PlaySound2D(this, EquippedWeapon->OnAttack1Sound);
				}
			}
			else if (Section == 1) {
				AnimInstance->Montage_Play(CombatMontage, 2.f);
				AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
				if (EquippedWeapon->OnAttack2Sound) {
					UGameplayStatics::PlaySound2D(this, EquippedWeapon->OnAttack2Sound);
				}
			}
		}
	}
}

void AMain::AttackEnd() {
	bAttacking = false;
	if (bLMBDown) {
		Attack();
	}
}
void AMain::Die() {
	GetEquippedWeapon()->Destroy();
	Destroy();
}

void AMain::SwitchLevel(FName LevelName) {
	UWorld* World = GetWorld();
	if (World) {
		FString CurrentLevel = World->GetMapName();
		FName CurrentLevelName = FName(CurrentLevel);
		if (CurrentLevelName != LevelName) {
			UGameplayStatics::OpenLevel(World, LevelName);
		}
	}
}