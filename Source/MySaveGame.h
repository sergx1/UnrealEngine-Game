// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MySaveGame.generated.h"

USTRUCT(BlueprintType)
struct FCharacterStats{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float MaxHealth;
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float Health;
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float MaxStamina;
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float Stamina;
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	int32 Exp;
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	int32 TotalExp;
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	int32 Lvl;
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float Damage;
};

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_API UMySaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UMySaveGame();

	UPROPERTY(VisibleAnywhere, Category = "Basics")
	FString PlayerName;

	UPROPERTY(VisibleAnywhere, Category = "Basics")
	int32 UserIndex;

	UPROPERTY(VisibleAnywhere, Category = "Basics")
	FCharacterStats CharacterStats;
};
