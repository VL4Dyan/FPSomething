// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Player/PlayerStateBase.h"
#include "GameMode/FPSmthGameStateBase.h"
#include "FPSmthGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class FPSOMETHING_API AFPSmthGameModeBase : public AGameMode
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
		void UpdatePlayerGameplayState(AController* Player);
	UFUNCTION(BlueprintNativeEvent)
		bool TrySwitchPlayerTeam(AController* Player, ETeamAffiliation NewPlayerTeam);
	UFUNCTION(BlueprintNativeEvent)
		bool TrySwitchPlayerClass(AController* Player, EPlayerClass NewPlayerClass);
};
