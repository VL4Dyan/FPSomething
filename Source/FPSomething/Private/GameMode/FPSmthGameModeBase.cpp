// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/FPSmthGameModeBase.h"

void AFPSmthGameModeBase::UpdatePlayerGameplayState_Implementation(AController* Player)
{

}

bool AFPSmthGameModeBase::TrySwitchPlayerTeam_Implementation(AController* Player, ETeamAffiliation NewPlayerTeam)
{
	return false;
}

bool AFPSmthGameModeBase::TrySwitchPlayerClass_Implementation(AController* Player, EPlayerClass NewPlayerClass)
{
	return false;
}