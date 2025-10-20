// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleBlasterGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Tower.h"
#include "BattleBlasterGameInstance.h"

void ABattleBlasterGameMode::BeginPlay()
{
	Super::BeginPlay();


	TArray<AActor*> Towers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATower::StaticClass(), Towers);
	TowerCount = Towers.Num();
	UE_LOG(LogTemp, Warning, TEXT("Tower count: %d"), TowerCount);


	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn)
	{
		Tank = Cast<ATank>(PlayerPawn);
		if (!Tank)	// Tank == nullptr
		{
			UE_LOG(LogTemp, Error, TEXT("Player pawn is not of type ATank"));
		}
	}

	int32 LoopIndex = 0;
	while (LoopIndex < TowerCount)
	{

		AActor* TowerActor = Towers[LoopIndex];
		if (TowerActor)
		{
			ATower* Tower = Cast<ATower>(TowerActor);
			if (Tower && Tank)
			{
				Tower->Tank = Tank;

				UE_LOG(LogTemp, Warning, TEXT("The name of the tower is %s"), *Tower->GetActorNameOrLabel());
			}
		}
		
		LoopIndex++;

	}
}

void ABattleBlasterGameMode::ActorDied(AActor* DeadActor)
{
	bool bIsGameOver = false;

	if (DeadActor == Tank)
	{
		Tank->HandleDestruction();
		bIsGameOver = true;
	}
	else
	{
		ATower* DeadTower = Cast<ATower>(DeadActor);
		if (DeadTower)
		{
			DeadTower->HandleDestruction();

			TowerCount--;
			if (TowerCount == 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("Victory"));
				bIsGameOver = true;
				bIsVictory = true;
			}
		}
	}

	if (bIsGameOver)
	{
		FString GameOverString = bIsVictory ? "Victory!" : "Defeat!";
		
		UE_LOG(LogTemp, Warning, TEXT("The Result is %s"), *GameOverString);
		
		FTimerHandle GameOverHandle;
		GetWorldTimerManager().SetTimer(GameOverHandle, this, &ABattleBlasterGameMode::OnGameOverTimerTimeout, GameOverDelay, false);
	}
}

void ABattleBlasterGameMode::OnGameOverTimerTimeout()
{
	UGameInstance* GameInstance = GetGameInstance();

	if (GameInstance)
	{
		UBattleBlasterGameInstance* BattleBlasterGameInstance = Cast<UBattleBlasterGameInstance>(GameInstance);
		if (BattleBlasterGameInstance)
		{
			if (bIsVictory)
			{
				BattleBlasterGameInstance->LoadNextLevel();
			}
			else
			{
				BattleBlasterGameInstance->RestartCurrentLevel();
			}
		}
	}
}
