// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Abilities/GA_SlowFall.h"
#include "Characters/CharacterBase.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Characters/CustomMovementComponent.h"

UGA_SlowFall::UGA_SlowFall()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.SlowFall")));
}

void UGA_SlowFall::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}

		ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
		UCustomMovementComponent* MovementComponent = Cast<UCustomMovementComponent>(Character->GetMovementComponent());

		if (MovementComponent)
		{
			MovementComponent->TurnOnSlowFall();
		}
	}
}

bool UGA_SlowFall::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	const ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed);
	const UCustomMovementComponent* MovementComponent = Cast<UCustomMovementComponent>(Character->GetMovementComponent());

	return MovementComponent && MovementComponent->CanTurnOnSlowFall();
}

void UGA_SlowFall::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo != NULL && ActorInfo->AvatarActor != NULL)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
}

void UGA_SlowFall::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
	UCustomMovementComponent* MovementComponent = Cast<UCustomMovementComponent>(Character->GetMovementComponent());

	if (MovementComponent)
	{
		MovementComponent->TurnOffSlowFall();
	}
}