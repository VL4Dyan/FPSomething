// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "PlayerStateBase.generated.h"

/**
 *
 */
class UFPSmthAbilitySystemComponent;
class UAbilitySystemComponent;
class UAttributeSetBase;

UENUM(BlueprintType)
enum class ETeamAffiliation : uint8
{
	Undefined,
	Spectators,
	TeamBLU,
	TeamRED
};

UENUM(BlueprintType)
enum class EPlayerClass : uint8
{
	Undefined,
	Scout
};

UCLASS()
class FPSOMETHING_API APlayerStateBase : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	APlayerStateBase();

	UPROPERTY(Replicated, BlueprintReadWrite)
		ETeamAffiliation PlayerTeam = ETeamAffiliation::Undefined;
	UPROPERTY(Replicated, BlueprintReadWrite)
		EPlayerClass PlayerClass = EPlayerClass::Undefined;
	UPROPERTY(Replicated, BlueprintReadWrite)
		bool bIsDead = true;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	UPROPERTY()
		UFPSmthAbilitySystemComponent* AbilitySystemComponent;
	UPROPERTY()
		UAttributeSetBase* AttributeSetBase;

	FDelegateHandle HealthModifiedDelegateHandle;

	virtual void BeginPlay() override;

	virtual void OnHealthModified(const FOnAttributeChangeData& Data);
};
