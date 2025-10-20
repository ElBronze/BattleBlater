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

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		ScreenMessageWidget = CreateWidget<UScreenMessage>(PlayerController, ScreenMessageClass);

		if (ScreenMessageWidget)
		{
			ScreenMessageWidget->AddToPlayerScreen();
			ScreenMessageWidget->SetMessageText("Get Ready!");
		}
	}
	CountdownSeconds = CountdownDelay;
	GetWorldTimerManager().SetTimer(CountdownTimerHandler, this, &ABattleBlasterGameMode::OnCountdownTimerTimeout, 1.0f, true);
}

void ABattleBlasterGameMode::OnCountdownTimerTimeout()
{
	CountdownSeconds -= 1;

	if (CountdownSeconds > 0)
	{
		ScreenMessageWidget->SetMessageText(FString::FromInt(CountdownSeconds));

	}
	else if (CountdownSeconds == 0)
	{
		ScreenMessageWidget->SetMessageText("Go!");
		Tank->SetPlayerEnable(true);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(CountdownTimerHandler);
		ScreenMessageWidget->SetVisibility(ESlateVisibility::Hidden);
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
		
		ScreenMessageWidget->SetMessageText(GameOverString);
		ScreenMessageWidget->SetVisibility(ESlateVisibility::Visible);

		
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
