// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Weapon.h"
#include "Characters/Abilities/FPSmthAbilitySystemComponent.h"
#include "Characters/Abilities/FPSmthGameplayAbility.h"
#include "Characters/CharacterBase.h"
#include "Characters/Abilities/GATA_Trace.h"
#include "Net/UnrealNetwork.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	bNetUseOwnerRelevancy = true;
	NetUpdateFrequency = 100.0f;
	LoadedAmmo = 0;
	MaxLoadedAmmo = 0;
	CarriedAmmo = 0;
	MaxCarriedAmmo = 0;
	bInfiniteAmmo = false;

	WeaponIsFiringTag = FGameplayTag::RequestGameplayTag("Weapon.IsFiring");
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AWeapon, LoadedAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AWeapon, MaxLoadedAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AWeapon, CarriedAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AWeapon, MaxCarriedAmmo, COND_OwnerOnly);
}

void AWeapon::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);

	DOREPLIFETIME_ACTIVE_OVERRIDE(AWeapon, LoadedAmmo, (IsValid(AbilitySystemComponent) && !AbilitySystemComponent->HasMatchingGameplayTag(WeaponIsFiringTag)));
	DOREPLIFETIME_ACTIVE_OVERRIDE(AWeapon, CarriedAmmo, (IsValid(AbilitySystemComponent) && !AbilitySystemComponent->HasMatchingGameplayTag(WeaponIsFiringTag)));
}

void AWeapon::Equip()
{
	if (!OwningCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("%s %s OwningCharacter is nullptr"), *FString(__FUNCTION__), *GetName());
		return;
	}

	FName AttachPoint = OwningCharacter->GetWeaponAttachPoint();

	if (FPWeaponMesh)
	{
		FPWeaponMesh->AttachToComponent(OwningCharacter->GetFirstPersonMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, AttachPoint);
		FPWeaponMesh->SetRelativeLocation(WeaponMesh1PEquippedRelativeLocation);
		FPWeaponMesh->SetRelativeRotation(FRotator(0, 0, -90.0f));
		FPWeaponMesh->SetVisibility(true, true);

	}
}

void AWeapon::Unequip()
{
	if (OwningCharacter == nullptr)
	{
		return;
	}

	// Necessary to detach so that when toggling perspective all meshes attached won't become visible.

	FPWeaponMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	FPWeaponMesh->SetVisibility(false, true);

	TPWeaponMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	TPWeaponMesh->CastShadow = false;
	TPWeaponMesh->bCastHiddenShadow = false;
	TPWeaponMesh->SetVisibility(true, true); // Without this, the unequipped weapon's third person shadow hangs around
	TPWeaponMesh->SetVisibility(false, true);
}

int32 AWeapon::GetLoadedAmmo() const
{
	return LoadedAmmo;
}

int32 AWeapon::GetMaxLoadedAmmo() const
{
	return MaxLoadedAmmo;
}

int32 AWeapon::GetCarriedAmmo() const
{
	return CarriedAmmo;
}

int32 AWeapon::GetMaxCarriedAmmo() const
{
	return MaxCarriedAmmo;
}

void AWeapon::SetLoadedAmmo(int32 NewClipAmmo)
{
	int32 OldClipAmmo = LoadedAmmo;
	LoadedAmmo = NewClipAmmo;
	OnLoadedAmmoChanged.Broadcast(OldClipAmmo, LoadedAmmo);
}

void AWeapon::SetMaxLoadedAmmo(int32 NewMaxClipAmmo)
{
	int32 OldMaxClipAmmo = MaxLoadedAmmo;
	MaxLoadedAmmo = NewMaxClipAmmo;
	OnMaxLoadedAmmoChanged.Broadcast(OldMaxClipAmmo, MaxLoadedAmmo);
}

void AWeapon::SetCarriedAmmo(int32 NewReserveAmmo)
{
	int32 OldReserveAmmo = CarriedAmmo;
	CarriedAmmo = NewReserveAmmo;
	OnCarriedAmmoChanged.Broadcast(OldReserveAmmo, CarriedAmmo);
}

void AWeapon::SetMaxCarriedAmmo(int32 NewMaxReserveAmmo)
{
	int32 OldMaxReserveAmmo = MaxCarriedAmmo;
	MaxCarriedAmmo = NewMaxReserveAmmo;
	OnMaxCarriedAmmoChanged.Broadcast(OldMaxReserveAmmo, MaxCarriedAmmo);
}

AGATA_Trace* AWeapon::GetLineTraceTargetActor()
{
	if (LineTraceTargetActor)
	{
		return LineTraceTargetActor;
	}

	LineTraceTargetActor = GetWorld()->SpawnActor<AGATA_Trace>();
	LineTraceTargetActor->SetOwner(this);
	return LineTraceTargetActor;
}

void AWeapon::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	if (LineTraceTargetActor)
	{
		LineTraceTargetActor->Destroy();
	}

	Super::EndPlay(EndPlayReason);
}

void AWeapon::OnRep_LoadedAmmo(int32 OldClipAmmo)
{
	OnLoadedAmmoChanged.Broadcast(OldClipAmmo, LoadedAmmo);
}

void AWeapon::OnRep_MaxLoadedAmmo(int32 OldMaxClipAmmo)
{
	OnMaxLoadedAmmoChanged.Broadcast(OldMaxClipAmmo, MaxLoadedAmmo);
}

void AWeapon::OnRep_CarriedAmmo(int32 OldReserveAmmo)
{
	OnCarriedAmmoChanged.Broadcast(OldReserveAmmo, CarriedAmmo);
}

void AWeapon::OnRep_MaxCarriedAmmo(int32 OldMaxReserveAmmo)
{
	OnMaxCarriedAmmoChanged.Broadcast(OldMaxReserveAmmo, MaxCarriedAmmo);
}


