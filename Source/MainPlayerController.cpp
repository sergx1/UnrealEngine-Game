// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

void AMainPlayerController::BeginPlay() {
	Super::BeginPlay();
	if (HUDOverlayAsset) {
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset);
	}
	HUDOverlay->AddToViewport();
	HUDOverlay->SetVisibility(ESlateVisibility::Visible);

	if (WPauseMenu) {
		PauseMenu = CreateWidget<UUserWidget>(this, WPauseMenu);
		if (PauseMenu) {
			PauseMenu->AddToViewport();
			PauseMenu->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (WControls) {
		Controls = CreateWidget<UUserWidget>(this, WControls);
		if (Controls) {
			Controls->AddToViewport();
			Controls->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	bPauseMenuVisible = false;

	if (WDeadMenu) {
		DeadMenu = CreateWidget<UUserWidget>(this, WDeadMenu);
		if (DeadMenu) {
			DeadMenu->AddToViewport();
			DeadMenu->SetVisibility(ESlateVisibility::Hidden);
		}
	}


}

void AMainPlayerController::Tick(float DeltaTime) {

	Super::Tick(DeltaTime);
}

void AMainPlayerController::DisplayPauseMenu() {
	if (PauseMenu) {
		bPauseMenuVisible = true;
		PauseMenu->SetVisibility(ESlateVisibility::Visible);
		if (PauseMenuSound) {
			UGameplayStatics::PlaySound2D(this, PauseMenuSound);
		}

		UGameplayStatics::SetGamePaused(GetWorld(), true);
		FInputModeGameAndUI InputModeGameAndUI;
		SetInputMode(InputModeGameAndUI);
		bShowMouseCursor = true;
	}
}

void AMainPlayerController::RemovePauseMenu() {
	if (PauseMenu) {
		bPauseMenuVisible = false;
		PauseMenu->SetVisibility(ESlateVisibility::Hidden);
		if (Controls) {
			Controls->SetVisibility(ESlateVisibility::Hidden);
		}
		if (PauseMenuSound) {
			UGameplayStatics::PlaySound2D(this, PauseMenuSound);
		}

		UGameplayStatics::SetGamePaused(GetWorld(), false);
		FInputModeGameOnly InputModeGameOnly;
		SetInputMode(InputModeGameOnly);
		bShowMouseCursor = false;
	}
}

void AMainPlayerController::TogglePauseMenu() {
	if(bPauseMenuVisible) {
		RemovePauseMenu();
	}
	else {
		DisplayPauseMenu();
	}
}

void AMainPlayerController::DisplayControls() {
	if (Controls) {
		if (PauseMenu) {
			PauseMenu->SetVisibility(ESlateVisibility::Hidden);
		}
		Controls->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainPlayerController::DisplayDeadMenu() {
	if (DeadMenu) {
		DeadMenu->SetVisibility(ESlateVisibility::Visible);
		// UGameplayStatics::SetGamePaused(GetWorld(), true);
		FInputModeGameAndUI InputModeGameAndUI;
		SetInputMode(InputModeGameAndUI);
		bShowMouseCursor = true;
	}
}

void AMainPlayerController::RestartGame() {
	RemovePauseMenu();
	RestartLevel();
}
