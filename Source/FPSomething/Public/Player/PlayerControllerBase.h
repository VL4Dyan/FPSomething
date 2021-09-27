// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Player/PlayerStateBase.h"
#include "PlayerControllerBase.generated.h"

/**
 *
 */
UCLASS()
class FPSOMETHING_API APlayerControllerBase : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Server, Reliable)
		void Server_SetPlayerTeam(ETeamAffiliation NewPlayerTeam);
	UFUNCTION(BlueprintCallable, Server, Reliable)
		void Server_SetPlayerClass(EPlayerClass NewPlayerClass);

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnRep_PlayerState() override;

	UFUNCTION(BlueprintNativeEvent, Category = "FPSomething|UI")
		void OpenTeamSelectionWindow();
};
