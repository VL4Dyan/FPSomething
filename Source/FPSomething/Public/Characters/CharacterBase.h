// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Characters/Abilities/FPSmthAbilitySystemComponent.h"
#include "Characters/Abilities/AttributeSets/AttributeSetBase.h"
#include "CharacterBase.generated.h"

class AWeapon;

UCLASS()
class FPSOMETHING_API ACharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACharacterBase(const FObjectInitializer& ObjectInitializer);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void RemoveCharacterAbilities();

	UFUNCTION(BlueprintCallable, Category = "FPSomething|Inventory")
		AWeapon* GetCurrentWeapon() const;

	FName GetWeaponAttachPoint();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSomething|Character")
		USkeletalMeshComponent* GetFirstPersonMesh() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSomething|Character")
		USkeletalMeshComponent* GetThirdPersonMesh() const;

	FGameplayTag CurrentWeaponTag;

protected:
	void SetCurrentWeapon(AWeapon* NewWeapon, AWeapon* LastWeapon);

	void UnequipWeapon(AWeapon* WeaponToUnequip);

	bool ASCInputBound = false;

	FGameplayTag WeaponAbilityTag;

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
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		TArray<TSubclassOf<AWeapon>> CharacterWeapons;

	bool bChangedWeaponLocally;

	UFUNCTION()
		void OnRep_CurrentWeapon(AWeapon* LastWeapon);
};
