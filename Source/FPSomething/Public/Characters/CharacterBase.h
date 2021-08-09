// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "AbilitySystemInterface.h"
#include "Characters/Abilities/AttributeSets/AttributeSetBase.h"
#include "CharacterBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDiedDelegate, ACharacterBase*, Character);

class AWeapon;
class UCameraComponent;
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

	virtual void PossessedBy(AController* NewController) override;

	virtual void AddCharacterAbilities();
	virtual void RemoveCharacterAbilities();

	UFUNCTION(BlueprintCallable)
		void AddWeaponToCharacter(AWeapon* NewWeapon, bool bEquipWeapon = false);

	UFUNCTION(BlueprintCallable)
		void EquipWeapon(AWeapon* WeaponToEquip);

	UFUNCTION(Server, Reliable)
		void ServerEquipWeapon(AWeapon* WeaponToEquip);
	void ServerEquipWeapon_Implementation(AWeapon* WeaponToEquip);
	bool ServerEquipWeapon_Validate(AWeapon* WeaponToEquip);

	UFUNCTION(BlueprintCallable)
		AWeapon* GetCurrentWeapon() const;

	UFUNCTION(Server, Reliable)
		void ServerSyncCurrentWeapon();
	void ServerSyncCurrentWeapon_Implementation();
	bool ServerSyncCurrentWeapon_Validate();

	UFUNCTION(Client, Reliable)
		void ClientSyncCurrentWeapon(AWeapon* Weapon);
	void ClientSyncCurrentWeapon_Implementation(AWeapon* Weapon);
	bool ClientSyncCurrentWeapon_Validate(AWeapon* Weapon);

	FName GetWeaponAttachPoint();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSomething|Character")
		USkeletalMeshComponent* GetFirstPersonMesh() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPSomething|Character")
		USkeletalMeshComponent* GetThirdPersonMesh() const;

	// Gets character's default move speed
	UFUNCTION(BlueprintCallable)
		float GetMoveSpeed() const;

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
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "FPSomething|Camera")
		UCameraComponent* FPCamera;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		USkeletalMeshComponent* FirstPersonMesh;
	UPROPERTY()
		UFPSmthAbilitySystemComponent* AbilitySystemComponent;
	UPROPERTY()
		UAttributeSetBase* AttributeSet;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FPSomething|Weapons")
		TArray<TSubclassOf<AWeapon>> CharacterLoadoutWeaponry;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FPSomething|Abilities")
		TArray<TSubclassOf<UFPSmthGameplayAbility>> CharacterAbilities;

	UPROPERTY()
		TArray<AWeapon*> CharacterCurrentWeaponry;

	virtual void PostInitializeComponents() override;

	void SetDefaultPlayerPerspective();
	void SetLoadoutWeaponry();

	void MoveForward(float Value);
	void MoveRight(float Value);
	void LookUp(float Value);
	void Turn(float Value);

	UFUNCTION()
		void OnRep_CurrentWeapon(AWeapon* LastWeapon);

	virtual void OnRep_Controller() override;

	void SetCurrentWeapon(AWeapon* NewWeapon, AWeapon* LastWeapon);

	void UnequipWeapon(AWeapon* WeaponToUnequip);
};
