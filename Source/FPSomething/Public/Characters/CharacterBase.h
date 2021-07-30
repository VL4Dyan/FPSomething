// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Characters/Abilities/AttributeSets/AttributeSetBase.h"
#include "CharacterBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDiedDelegate, ACharacterBase*, Character);

class AWeapon;
class UAbilitySystemComponent;
class UFPSmthGameplayAbility;
class UFPSmthAbilitySystemComponent;

UCLASS()
class FPSOMETHING_API ACharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACharacterBase(const FObjectInitializer& ObjectInitializer);

	FGameplayTag CurrentWeaponTag;

	UPROPERTY(BlueprintAssignable)
		FCharacterDiedDelegate OnCharacterDied;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void AddCharacterAbilities();
	virtual void RemoveCharacterAbilities();

	UFUNCTION(BlueprintCallable)
		AWeapon* GetCurrentWeapon() const;

	FName GetWeaponAttachPoint();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSomething|Character")
		USkeletalMeshComponent* GetFirstPersonMesh() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSomething|Character")
		USkeletalMeshComponent* GetThirdPersonMesh() const;

	virtual void Die();
	UFUNCTION(BlueprintCallable)
		virtual void FinishDying();

protected:
	bool ASCInputBound = false;
	bool bChangedWeaponLocally;

	FGameplayTag WeaponAbilityTag;
	FGameplayTag DeadTag;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentWeapon)
		AWeapon* CurrentWeapon;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FName WeaponAttachPoint;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		USkeletalMeshComponent* FirstPersonMesh;
	UPROPERTY()
		UFPSmthAbilitySystemComponent* AbilitySystemComponent;
	UPROPERTY()
		UAttributeSetBase* AttributeSet;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FPSomething|Weapons")
		TArray<TSubclassOf<AWeapon>> CharacterWeapons;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FPSomething|Abilities")
		TArray<TSubclassOf<UFPSmthGameplayAbility>> CharacterAbilities;

	UFUNCTION()
		void OnRep_CurrentWeapon(AWeapon* LastWeapon);

	void SetCurrentWeapon(AWeapon* NewWeapon, AWeapon* LastWeapon);

	void UnequipWeapon(AWeapon* WeaponToUnequip);
};
