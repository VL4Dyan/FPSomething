// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpec.h"
#include "Weapons/Equipment.h"
#include "../FPSomething.h"
#include "Weapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWeaponAmmoChangedDelegate, int32, OldValue, int32, NewValue);

class ACharacterBase;
class AGATA_Trace;
class UFPSmthAbilitySystemComponent;
class UFPSmthGameplayAbility;
class ACharacterBase;

UCLASS()
class FPSOMETHING_API AWeapon : public AEquipment
{
	GENERATED_BODY()

public:
	AWeapon();

	UPROPERTY(BlueprintAssignable, Category = "FPSomething|Weapon")
		FWeaponAmmoChangedDelegate OnLoadedAmmoChanged;
	UPROPERTY(BlueprintAssignable, Category = "FPSomething|Weapon")
		FWeaponAmmoChangedDelegate OnCarriedAmmoChanged;
	UPROPERTY(BlueprintAssignable, Category = "FPSomething|Weapon")
		FWeaponAmmoChangedDelegate OnMaxLoadedAmmoChanged;
	UPROPERTY(BlueprintAssignable, Category = "FPSomething|Weapon")
		FWeaponAmmoChangedDelegate OnMaxCarriedAmmoChanged;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;

	virtual void Equip();

	virtual void Unequip();

	UFUNCTION(BlueprintCallable, Category = "FPSomething|Weapon")
		virtual int32 GetLoadedAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "FPSomething|Weapon")
		virtual int32 GetMaxLoadedAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "FPSomething|Weapon")
		virtual int32 GetCarriedAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "FPSomething|Weapon")
		virtual int32 GetMaxCarriedAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "FPSomething|Weapon")
		virtual void SetLoadedAmmo(int32 NewClipAmmo);

	UFUNCTION(BlueprintCallable, Category = "FPSomething|Weapon")
		virtual void SetMaxLoadedAmmo(int32 NewMaxClipAmmo);

	UFUNCTION(BlueprintCallable, Category = "FPSomething|Weapon")
		virtual void SetCarriedAmmo(int32 NewReserveAmmo);

	UFUNCTION(BlueprintCallable, Category = "FPSomething|Weapon")
		virtual void SetMaxCarriedAmmo(int32 NewMaxReserveAmmo);

	UFUNCTION(BlueprintCallable, Category = "FPSomething|Targeting")
		AGATA_Trace* GetLineTraceTargetActor();

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, ReplicatedUsing = OnRep_LoadedAmmo, Category = "FPSomething|Weapon|Ammo")
		int32 LoadedAmmo;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, ReplicatedUsing = OnRep_MaxLoadedAmmo, Category = "FPSomething|Weapon|Ammo")
		int32 MaxLoadedAmmo;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, ReplicatedUsing = OnRep_CarriedAmmo, Category = "FPSomething|Weapon|Ammo")
		int32 CarriedAmmo;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, ReplicatedUsing = OnRep_MaxCarriedAmmo, Category = "FPSomething|Weapon|Ammo")
		int32 MaxCarriedAmmo;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FPSomething|Weapon|Ammo")
		bool bInfiniteAmmo;

	UPROPERTY()
		AGATA_Trace* LineTraceTargetActor;

	// Cache tags
	FGameplayTag WeaponPrimaryInstantAbilityTag;
	FGameplayTag WeaponSecondaryInstantAbilityTag;
	FGameplayTag WeaponAlternateInstantAbilityTag;
	FGameplayTag WeaponIsFiringTag;

	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
		virtual void OnRep_LoadedAmmo(int32 OldClipAmmo);

	UFUNCTION()
		virtual void OnRep_MaxLoadedAmmo(int32 OldMaxClipAmmo);

	UFUNCTION()
		virtual void OnRep_CarriedAmmo(int32 OldReserveAmmo);

	UFUNCTION()
		virtual void OnRep_MaxCarriedAmmo(int32 OldMaxReserveAmmo);
};
