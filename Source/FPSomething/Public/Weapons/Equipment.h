// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpec.h"
#include "../FPSomething.h"
#include "Equipment.generated.h"

class ACharacterBase;
class UFPSmthAbilitySystemComponent;
class UFPSmthGameplayAbility;
class ACharacterBase;

UCLASS()
class FPSOMETHING_API AEquipment : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AEquipment();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "FPSomething|Weapon")
		FGameplayTag WeaponTag;

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSomething|Weapon")
		virtual USkeletalMeshComponent* GetFPWeaponMesh() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSomething|Weapon")
		virtual USkeletalMeshComponent* GetTPWeaponMesh() const;

	UFUNCTION(BlueprintCallable, Category = "FPSomething|Animation")
		UAnimMontage* GetFPEquipMontage() const;

	UFUNCTION(BlueprintCallable, Category = "FPSomething|Animation")
		UAnimMontage* GetTPEquipMontage() const;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetOwningCharacter(ACharacterBase* InOwningCharacter);

	virtual void AddAbilities();

	virtual void RemoveAbilities();

protected:
	UPROPERTY()
		UFPSmthAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
		bool bCanBeEquipped;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FPSomething|Weapon")
		UAnimMontage* FPEquipMontage;

	UPROPERTY(BlueprintReadonly, EditAnywhere, Category = "FPSomething|Weapon")
		UAnimMontage* TPEquipMontage;

	UPROPERTY(VisibleAnywhere, Category = "FPSomething|Weapon")
		USkeletalMeshComponent* FPWeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "FPSomething|Weapon")
		USkeletalMeshComponent* TPWeaponMesh;

	// Relative Location of first person perspective weapon mesh when equipped
	UPROPERTY(EditDefaultsOnly, Category = "FPSomething|Weapon")
		FVector WeaponMesh1PEquippedRelativeLocation;

	// Relative Location of third person perspective weapon mesh when equipped
	UPROPERTY(EditDefaultsOnly, Category = "FPSomething|Weapon")
		FVector WeaponMesh3PEquippedRelativeLocation;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "FPSomething|Weapon")
		ACharacterBase* OwningCharacter;

	UPROPERTY(EditAnywhere, Category = "FPSomething|Weapon")
		TArray<TSubclassOf<UFPSmthGameplayAbility>> Abilities;

	UPROPERTY(BlueprintReadOnly, Category = "FPSomething|Weapon")
		TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;
};
