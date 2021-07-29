// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "Characters/Abilities/FPSmthAbilitySystemComponent.h"
#include "CustomPlayerState.generated.h"

/**
 *
 */
UCLASS()
class FPSOMETHING_API ACustomPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ACustomPlayerState();

protected:
	virtual void BeginPlay() override;
};
