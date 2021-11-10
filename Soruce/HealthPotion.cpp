// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPotion.h"
#include "Main.h"

AHealthPotion::AHealthPotion() {
	HealthIncrease = 50.f;
}

void AHealthPotion::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (OtherActor) {
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main) {
			HealthIncrease = (Main->MaxHealth) / 2;
			Main->IncreaseHealth(HealthIncrease);
			Destroy();
		}
	}
}

void AHealthPotion::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {

}