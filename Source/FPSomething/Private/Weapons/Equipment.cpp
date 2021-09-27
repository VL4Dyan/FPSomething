// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Equipment.h"
#include "Net/UnrealNetwork.h"
#include "Characters/Abilities/FPSmthAbilitySystemComponent.h"
#include "Characters/Abilities/FPSmthGameplayAbility.h"
#include "Weapons/Weapon.h" //delete and rework set owner
#include "Characters/CharacterBase.h"

AEquipment::AEquipment()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	bNetUseOwnerRelevancy = true;
	NetUpdateFrequency = 100.0f;

	FPWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("FirstPersonWeaponMesh"));
	FPWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FPWeaponMesh->CastShadow = false;
	FPWeaponMesh->SetVisibility(false, true);
	FPWeaponMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;

	TPWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("ThirdPersonWeaponMesh"));
	TPWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TPWeaponMesh->CastShadow = true;
	TPWeaponMesh->SetVisibility(true, true);
	TPWeaponMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
}

UAbilitySystemComponent* AEquipment::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

USkeletalMeshComponent* AEquipment::GetFPWeaponMesh() const
{
	return FPWeaponMesh;
}

USkeletalMeshComponent* AEquipment::GetTPWeaponMesh() const
{
	return TPWeaponMesh;
}

UAnimMontage* AEquipment::GetFPEquipMontage() const
{
	return FPEquipMontage;
}

UAnimMontage* AEquipment::GetTPEquipMontage() const
{
	return TPEquipMontage;
}

void AEquipment::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AEquipment, OwningCharacter, COND_OwnerOnly);
}

void AEquipment::SetOwningCharacter(ACharacterBase* InOwningCharacter)
{
	OwningCharacter = InOwningCharacter;
	if (OwningCharacter)
	{
		// Called when added to inventory
		AbilitySystemComponent = Cast<UFPSmthAbilitySystemComponent>(OwningCharacter->GetAbilitySystemComponent());
		SetOwner(InOwningCharacter);
		AttachToComponent(OwningCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

		if (OwningCharacter->GetCurrentWeapon() != this)
		{
			TPWeaponMesh->CastShadow = false;
			TPWeaponMesh->SetVisibility(true, true);
			TPWeaponMesh->SetVisibility(false, true);
		}
	}
	else
	{
		AbilitySystemComponent = nullptr;
		SetOwner(nullptr);
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
}

void AEquipment::AddAbilities()
{
	if (!IsValid(OwningCharacter) || !OwningCharacter->GetAbilitySystemComponent())
	{
		return;
	}

	UFPSmthAbilitySystemComponent* ASC = Cast<UFPSmthAbilitySystemComponent>(OwningCharacter->GetAbilitySystemComponent());

	if (!ASC)
	{
		return;
	}

	// Grant abilities, but only on the server	
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	for (TSubclassOf<UFPSmthGameplayAbility>& Ability : Abilities)
	{
		AbilitySpecHandles.Add(ASC->GiveAbility(
			FGameplayAbilitySpec(Ability, 1, static_cast<int32>(Ability.GetDefaultObject()->AbilityInputID), this)));
	}
}

void AEquipment::RemoveAbilities()
{
	if (!IsValid(OwningCharacter) || !OwningCharacter->GetAbilitySystemComponent())
	{
		return;
	}

	UFPSmthAbilitySystemComponent* ASC = Cast<UFPSmthAbilitySystemComponent>(OwningCharacter->GetAbilitySystemComponent());

	if (!ASC)
	{
		return;
	}

	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	for (FGameplayAbilitySpecHandle& SpecHandle : AbilitySpecHandles)
	{
		ASC->ClearAbility(SpecHandle);
	}
}

