// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpec.h"
#include "../FPSomething.h"
#include "Weapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWeaponAmmoChangedDelegate, int32, OldValue, int32, NewValue);

class ACharacterBase;
class AGATA_Trace;
class UFPSmthAbilitySystemComponent;
class UFPSmthGameplayAbility;
class ACharacterBase;

UCLASS()
class FPSOMETHING_API AWeapon : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AWeapon();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FPSomething|Weapon")
		FGameplayTag WeaponTag;

	UPROPERTY(BlueprintAssignable, Category = "FPSomething|Weapon")
		FWeaponAmmoChangedDelegate OnClipAmmoChanged;

	UPROPERTY(BlueprintAssignable, Category = "FPSomething|Weapon")
		FWeaponAmmoChangedDelegate OnReserveAmmoChanged;

	UPROPERTY(BlueprintAssignable, Category = "FPSomething|Weapon")
		FWeaponAmmoChangedDelegate OnMaxClipAmmoChanged;

	UPROPERTY(BlueprintAssignable, Category = "FPSomething|Weapon")
		FWeaponAmmoChangedDelegate OnMaxReserveAmmoChanged;

	//IAbilitySystemInterface
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//End of IAbilitySystemInterface

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSomething|Weapon")
		virtual USkeletalMeshComponent* GetWeaponMesh1P() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSomething|Weapon")
		virtual USkeletalMeshComponent* GetWeaponMesh3P() const;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;

	void SetOwningCharacter(ACharacterBase* InOwningCharacter);

	virtual void Equip();

	virtual void Unequip();

	virtual void AddAbilities();

	virtual void RemoveAbilities();

	UFUNCTION(BlueprintCallable, Category = "FPSomething|Weapon")
		virtual int32 GetClipAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "FPSomething|Weapon")
		virtual int32 GetMaxClipAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "FPSomething|Weapon")
		virtual int32 GetReserveAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "FPSomething|Weapon")
		virtual int32 GetMaxReserveAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "FPSomething|Weapon")
		virtual void SetClipAmmo(int32 NewPrimaryClipAmmo);

	UFUNCTION(BlueprintCallable, Category = "FPSomething|Weapon")
		virtual void SetMaxClipAmmo(int32 NewMaxPrimaryClipAmmo);

	UFUNCTION(BlueprintCallable, Category = "FPSomething|Weapon")
		virtual void SetReserveAmmo(int32 NewMaxPrimaryClipAmmo);

	UFUNCTION(BlueprintCallable, Category = "FPSomething|Weapon")
		virtual void SetMaxReserveAmmo(int32 NewMaxPrimaryClipAmmo);

	UFUNCTION(BlueprintCallable, Category = "FPSomething|Animation")
		UAnimMontage* GetEquip1PMontage() const;

	UFUNCTION(BlueprintCallable, Category = "FPSomething|Animation")
		UAnimMontage* GetEquip3PMontage() const;

	UFUNCTION(BlueprintCallable, Category = "FPSomething|Targeting")
		AGATA_Trace* GetLineTraceTargetActor();

protected:
	UPROPERTY()
		UFPSmthAbilitySystemComponent* AbilitySystemComponent;

	// How much ammo in the clip the gun starts with
	UPROPERTY(BlueprintReadOnly, EditAnywhere, ReplicatedUsing = OnRep_ClipAmmo, Category = "FPSomething|Weapon|Ammo")
		int32 ClipAmmo;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, ReplicatedUsing = OnRep_MaxClipAmmo, Category = "FPSomething|Weapon|Ammo")
		int32 MaxClipAmmo;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, ReplicatedUsing = OnRep_ReserveAmmo, Category = "FPSomething|Weapon|Ammo")
		int32 ReserveAmmo;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, ReplicatedUsing = OnRep_MaxReserveAmmo, Category = "FPSomething|Weapon|Ammo")
		int32 MaxReserveAmmo;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASShooter|GSWeapon|Ammo")
		bool bInfiniteAmmo;

	UPROPERTY()
		AGATA_Trace* LineTraceTargetActor;

	// Collision capsule for when weapon is in pickup mode
	UPROPERTY(VisibleAnywhere)
		class UCapsuleComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, Category = "GASShooter|GSWeapon")
		USkeletalMeshComponent* WeaponMesh1P;

	UPROPERTY(VisibleAnywhere, Category = "GASShooter|GSWeapon")
		USkeletalMeshComponent* WeaponMesh3P;

	// Relative Location of weapon 3P Mesh when in pickup mode
	// 1P weapon mesh is invisible so it doesn't need one
	UPROPERTY(EditDefaultsOnly, Category = "GASShooter|GSWeapon")
		FVector WeaponMesh3PickupRelativeLocation;

	// Relative Location of weapon 1P Mesh when equipped
	UPROPERTY(EditDefaultsOnly, Category = "GASShooter|GSWeapon")
		FVector WeaponMesh1PEquippedRelativeLocation;

	// Relative Location of weapon 3P Mesh when equipped
	UPROPERTY(EditDefaultsOnly, Category = "GASShooter|GSWeapon")
		FVector WeaponMesh3PEquippedRelativeLocation;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "GASShooter|GSWeapon")
		ACharacterBase* OwningCharacter;

	UPROPERTY(EditAnywhere, Category = "GASShooter|GSWeapon")
		TArray<TSubclassOf<UFPSmthGameplayAbility>> Abilities;

	UPROPERTY(BlueprintReadOnly, Category = "GASShooter|GSWeapon")
		TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASShooter|GSWeapon")
		FGameplayTag DefaultFireMode;

	// Things like fire mode for rifle
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASShooter|GSWeapon")
		FText DefaultStatusText;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASShooter|Animation")
		UAnimMontage* Equip1PMontage;

	UPROPERTY(BlueprintReadonly, EditAnywhere, Category = "GASShooter|Animation")
		UAnimMontage* Equip3PMontage;

	// Cache tags
	FGameplayTag WeaponPrimaryInstantAbilityTag;
	FGameplayTag WeaponSecondaryInstantAbilityTag;
	FGameplayTag WeaponAlternateInstantAbilityTag;
	FGameplayTag WeaponIsFiringTag;

	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
		virtual void OnRep_ClipAmmo(int32 OldClipAmmo);

	UFUNCTION()
		virtual void OnRep_MaxClipAmmo(int32 OldMaxClipAmmo);

	UFUNCTION()
		virtual void OnRep_ReserveAmmo(int32 OldReserveAmmo);

	UFUNCTION()
		virtual void OnRep_MaxReserveAmmo(int32 OldMaxReserveAmmo);
};
