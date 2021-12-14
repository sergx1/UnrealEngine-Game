// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelChange.h"
#include "Components/BoxComponent.h"
#include "Main.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ALevelChange::ALevelChange()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LevelChangeBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LevelChangeBox"));
	LevelChangeBox->SetupAttachment(GetRootComponent());

	LvlSwitchParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("LvlSwitch Particle"));
	LvlSwitchParticles->SetupAttachment(GetRootComponent());

	TransitionLevelName = "FrozenCove";
	LvlRequired = 3;

}

// Called when the game starts or when spawned
void ALevelChange::BeginPlay()
{
	Super::BeginPlay();

	LevelChangeBox->OnComponentBeginOverlap.AddDynamic(this, &ALevelChange::OnOverlapBegin);
	
}

// Called every frame
void ALevelChange::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALevelChange::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor) {
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main) {
			if (Main->Lvl >= LvlRequired) {
				Main->SwitchLevel(TransitionLevelName);
			}
			else {
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, 
					FString::Printf(TEXT("Lvl %d required to enter this level."), LvlRequired), true, FVector2D(1.8f));
			}
		}
	}
}

