// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "Main.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "MainPlayerController.h"
#include "Components/WidgetComponent.h"
#include "Components/ProgressBar.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHealthBar.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());
	AgroSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(100.f);

	LeftCombatCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollisionBox"));
	LeftCombatCollisionBox->SetupAttachment(GetMesh(), FName("LeftClawSocket"));
	// LeftCombatCollisionBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("LeftClawSocket"));

	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar Widget"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->MaxWalkSpeed = 200.f;

	bOverlappingCombatSphere = false;

	MaxHealth = 100.f;
	Health = 100.f;
	Damage = 10.f;
	bIsDead = false;
	SwitchTime = 25.f;
	bAttacking = false;

	MinAttackTime = 0.5f;
	MaxAttackTime = 1.5;
	AgroLocationMove = FVector(0.f, 0.f, 0.f);

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	AIController = Cast<AAIController>(GetController());

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapBegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapEnd);
	AgroSphere->SetSphereRadius(600.f);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapEnd);

	LeftCombatCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::LeftCombatBoxOnOverlapBegin);
	LeftCombatCollisionBox->OnComponentEndOverlap.AddDynamic(this, &AEnemy::LeftCombatBoxOnOverlapEnd);

	LeftCombatCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftCombatCollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftCombatCollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftCombatCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	HealthBarWidget->InitWidget();
	HealthBarWidget->SetVisibility(false);
	
	EnemyHealthBar = Cast<UEnemyHealthBar>(HealthBarWidget->GetUserWidgetObject());
	if (!EnemyHealthBar)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: Failed to Cast to UHealthBarWidget"), *GetOwner()->GetName());
		return;
	}

	ExpToGive = (int32)(MaxHealth * 1.5);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (EnemyHealthBar) {
		EnemyHealthBar->GetHealthBar()->SetPercent(Health/MaxHealth);
	}
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
void AEnemy::AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor && !bIsDead) {
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main) {
			AgroLocationMove = GetActorLocation();
			AgroSphere->SetSphereRadius(1200.f);
			MoveToTarget(Main);
			Main->SetCombatTarget(this);
			Main->bHasCombatTarget = true;
			HealthBarWidget->SetVisibility(true);
			/*if (Main->MainPlayerController) {
				Main->MainPlayerController->DisplayEnemyHealthBar();
			}*/
		}
	}
}


void AEnemy::AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (OtherActor) {
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main && !bIsDead) {
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
			if (AIController) {
				AIController->MoveToLocation(AgroLocationMove);
				if (GetActorLocation() == AgroLocationMove) {
					AIController->StopMovement();
				}
				AgroSphere->SetSphereRadius(600.f);
			}
			Main->SetCombatTarget(nullptr);
			Main->bHasCombatTarget = false;
			HealthBarWidget->SetVisibility(false);
			/*if (Main->MainPlayerController) {
				Main->MainPlayerController->RemoveEnemyHealthBar();
			}*/
		}
	}
}


void AEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor && !bIsDead) {
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main) {
			CombatTarget = Main;
			bOverlappingCombatSphere = true;
			Attack();
		}
	}
}


void AEnemy::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (OtherActor) {
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main) {
			bOverlappingCombatSphere = false;
			if (EnemyMovementStatus != EEnemyMovementStatus::EMS_Attacking) {
				MoveToTarget(Main);
				CombatTarget = nullptr;
			}
			GetWorldTimerManager().ClearTimer(AttackTimer);
		}
	}
}

void AEnemy::MoveToTarget(AMain* Target) {

	if (!bIsDead) {
		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
		if (AIController) {
			// AIController->MoveToActor(Target);
			FAIMoveRequest MoveRequest;
			MoveRequest.SetGoalActor(Target);
			MoveRequest.SetAcceptanceRadius(20.0f);
			FNavPathSharedPtr NavPath;

			AIController->MoveTo(MoveRequest, &NavPath);
		}
	}
}

void AEnemy::LowerHealth(float Amount) {
	if (Health - Amount <= 0) {
		Health -= Amount;
		GetMesh()->GetAnimInstance()->StopAllMontages(0);
		bIsDead = true;
		HealthBarWidget->SetVisibility(false);
		SetActorEnableCollision(false);
		GetWorldTimerManager().SetTimer(SwitchHandle, this, &AEnemy::Die, SwitchTime);
	}
	else {
		Health -= Amount;
	}
}

void AEnemy::Die() {
	Destroy();
}


void AEnemy::LeftCombatBoxOnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor) {
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main) {
			Main->LowerHealth(Damage);
			DeactivateCollision();
			if (Main->HitParticles) {
				const USkeletalMeshSocket* ClawSocket = GetMesh()->GetSocketByName(FName("LeftClawSocket"));
				if (ClawSocket) {
					FVector SocketLocation = ClawSocket->GetSocketLocation(GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Main->HitParticles, SocketLocation, FRotator(0.f), true);
				}
			}
			if (Main->HitSound) {
				UGameplayStatics::PlaySound2D(this, Main->HitSound);
			}
		}
	}
}

void AEnemy::LeftCombatBoxOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {

}

void AEnemy::ActivateCollision() {
	LeftCombatCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	if (SwingSound) {
		UGameplayStatics::PlaySound2D(this, SwingSound);
	}
}

void AEnemy::DeactivateCollision() {
	LeftCombatCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::Attack() {
	if (AIController) {
		AIController->StopMovement();
		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
	}
	if (!bAttacking) {
		bAttacking = true;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (CombatTarget) {
			AMain* Main = Cast<AMain>(CombatTarget);
			if (Main->bIsDead) {
				AnimInstance->StopAllMontages(0);
			}
		}
		if (AnimInstance) {
			AnimInstance->Montage_Play(CombatMontage, 1.35f);
			AnimInstance->Montage_JumpToSection(FName("Attack1"), CombatMontage);
		}
	}
}

void AEnemy::AttackEnd() {
	AMain* Main = Cast<AMain>(CombatTarget);
	bAttacking = false;
	if (bOverlappingCombatSphere && !(Main->bIsDead)) {
		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_NotAttacking);
		float AttackTime = FMath::FRandRange(MinAttackTime, MaxAttackTime);
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
	}
}


