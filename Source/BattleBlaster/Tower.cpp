// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"

ATower::ATower()
{
}

void ATower::BeginPlay()
{
	Super::BeginPlay();


	FTimerHandle FireTimeHandle;
	GetWorldTimerManager().SetTimer(FireTimeHandle, this, &ATower::CheckFireCondition, FireRate, true);
}

void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsInFireRange())
	{
		RotateTurret(Tank->GetActorLocation());
	}
	
}

void ATower::CheckFireCondition()
{
	if (Tank && Tank->bIsAlive &&IsInFireRange())
	{
		Fire();
	}
}

bool ATower::IsInFireRange()
{
	bool Result = false;
	
	if (Tank)
	{
		float Distance = FVector::Dist(GetActorLocation(), Tank->GetActorLocation());

		Result = (Distance <= FireRange);

	}	
	return Result;
}

void ATower::HandleDestruction()
{
	Super::HandleDestruction();

	Destroy();
	UE_LOG(LogTemp, Warning, TEXT("Tower HandleDestruction"));
}

