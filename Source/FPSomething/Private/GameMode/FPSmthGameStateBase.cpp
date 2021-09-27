// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/FPSmthGameStateBase.h"
#include "Net/UnrealNetwork.h"

AFPSmthGameStateBase::AFPSmthGameStateBase()
{
	bReplicates = true;

	bGameStarted = false;
	GamePhase = EGamePhase::GP_Undefined;
}

void AFPSmthGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSmthGameStateBase, bGameStarted);
	DOREPLIFETIME(AFPSmthGameStateBase, GamePhase);
}

void AFPSmthGameStateBase::SetGameStarted(bool NewGameStarted)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		bGameStarted = NewGameStarted;
	}
}

void AFPSmthGameStateBase::SetGamePhase(EGamePhase NewGamePhase)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		GamePhase = NewGamePhase;
	}
}

bool AFPSmthGameStateBase::IsGameStarted() const
{
	return bGameStarted;
}

EGamePhase AFPSmthGameStateBase::GetGamePhase() const
{
	return GamePhase;
}

void AFPSmthGameStateBase::OnRep_GameStarted(bool PreviousGameState)
{
	if (PreviousGameState != bGameStarted && bGameStarted)
	{
		OnGameStarted.Broadcast();
	}
}