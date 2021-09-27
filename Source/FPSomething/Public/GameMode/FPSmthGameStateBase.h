// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "FPSmthGameStateBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGameStarted);

UENUM()
enum class EGamePhase : uint8
{
	GP_Undefined,
	GP_Preparation,
	GP_Fight,
	GP_Humiliation
};

UCLASS()
class FPSOMETHING_API AFPSmthGameStateBase : public AGameState
{
	GENERATED_BODY()

public:
	AFPSmthGameStateBase();

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
		FGameStarted OnGameStarted;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
		void SetGameStarted(bool NewGameStarted);
	UFUNCTION(BlueprintCallable)
		void SetGamePhase(EGamePhase NewGamePhase);

	UFUNCTION(BlueprintCallable)
		bool IsGameStarted() const;
	UFUNCTION(BlueprintCallable)
		EGamePhase GetGamePhase() const;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_GameStarted)
		bool bGameStarted;
	UPROPERTY(Replicated)
		EGamePhase GamePhase;

	UFUNCTION()
		void OnRep_GameStarted(bool PreviousGameState);
};
