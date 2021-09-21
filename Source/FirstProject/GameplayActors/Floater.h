// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Floater.generated.h"

UCLASS()
class FIRSTPROJECT_API AFloater : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloater();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ActorMeshComponents")
	UStaticMeshComponent* StaticMesh;

	// Lokacija koju koristi SetActorLocation kada se pokrene BeginPlay().
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "FloaterVariables")
	FVector InitialLocation;

	// Lokacija kada prevucemo objekat iz editora.
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "FloaterVariables")
	FVector PlacedLocation;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "FloaterVariables")
	FVector WorldOrigin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FloaterVariables")
	FVector InitialDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FloaterVariables")
	FRotator InitialRotation;

	UPROPERTY(EditInstanceonly, BlueprintReadWrite, Category = "FloaterVariables")
	FVector InitialForce;

	UPROPERTY(EditInstanceonly, BlueprintReadWrite, Category = "FloaterVariables")
	FVector InitialTorque;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FloaterVariables")
	bool bShouldFloat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FloaterVariables")
	bool bShouldRotate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FloaterVariables")
	bool bInitializeFloaterLocation;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
