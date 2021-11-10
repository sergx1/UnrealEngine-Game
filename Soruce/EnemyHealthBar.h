// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHealthBar.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_API UEnemyHealthBar : public UUserWidget
{
	GENERATED_BODY()
public:
    FORCEINLINE class UProgressBar* GetHealthBar() { return HealthBar; }

    UPROPERTY(meta = (BindWidget))
    class UProgressBar* HealthBar;
};
