// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/PlayerControllerBase.h"
#include "GameMode/FPSmthGameModeBase.h"

void APlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Controller's begin play triggered!"));
}

void APlayerControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	APlayerStateBase* PS = GetPlayerState<APlayerStateBase>();
}

void APlayerControllerBase::Server_SetPlayerTeam_Implementation(ETeamAffiliation NewPlayerTeam)
{
	AFPSmthGameModeBase* GM = Cast<AFPSmthGameModeBase>(GetWorld()->GetAuthGameMode());
	GM->TrySwitchPlayerTeam(this, NewPlayerTeam);
}

void APlayerControllerBase::Server_SetPlayerClass_Implementation(EPlayerClass NewPlayerClass)
{
	AFPSmthGameModeBase* GM = Cast<AFPSmthGameModeBase>(GetWorld()->GetAuthGameMode());
	GM->TrySwitchPlayerClass(this, NewPlayerClass);
}

void APlayerControllerBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	
}

void APlayerControllerBase::OpenTeamSelectionWindow_Implementation()
{

}