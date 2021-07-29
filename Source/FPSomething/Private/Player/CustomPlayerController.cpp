// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CustomPlayerController.h"
#include "Player/CustomPlayerState.h"

void ACustomPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ACustomPlayerState* PS = GetPlayerState<ACustomPlayerState>();
}

void ACustomPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
}