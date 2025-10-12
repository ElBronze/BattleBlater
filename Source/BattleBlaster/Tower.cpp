// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"

ATower::ATower()
{
}

void ATower::BeginPlay()
{
	Super::BeginPlay();

}

void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Tank)
	{
		float Distance = FVector::Dist(GetActorLocation(), Tank->GetActorLocation());
		
		if (Distance <= FireRange)
		{
			DrawDebugSphere(GetWorld(), GetActorLocation(), FireRange, 12, FColor::Red, false, 0.0f, 0, 2.0f);
			
			RotateTurret(Tank->GetActorLocation());

		}

	}
	
}
