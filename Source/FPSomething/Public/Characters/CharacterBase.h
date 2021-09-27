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
class UAttributeSetBase;

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

	virtual void RespondToHealthModification(const FOnAttributeChangeData& Data);

	UFUNCTION(BlueprintCallable)
		void AddWeaponToCharacter(AWeapon* NewWeapon, bool bEquipWeapon = false);
	UFUNCTION(BlueprintCallable)
		void EquipFirstWeapon();
	UFUNCTION(BlueprintCallable)
		void EquipNextWeapon();
	UFUNCTION(BlueprintCallable)
		void EquipPreviousWeapon();
	UFUNCTION(BlueprintCallable)
		AWeapon* GetCurrentWeapon() const;

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
	bool bASCInputBound = false;
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
		USkeletalMeshComponent* FPMesh;
	UPROPERTY()
		UFPSmthAbilitySystemComponent* AbilitySystemComponent;
	UPROPERTY()
		UAttributeSetBase* AttributeSet;

	//Weaponry character spawns with
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FPSomething|Gear")
		TArray<TSubclassOf<AWeapon>> CharacterLoadoutWeaponry;
	//Any abilities related to class
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FPSomething|Class")
		TArray<TSubclassOf<UFPSmthGameplayAbility>> CharacterAbilities;
	//Attributes that define class
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FPSomething|Class")
		TSubclassOf<class UGameplayEffect> DefaultAttributes;

	//Weaponry character currently holds
	UPROPERTY()
		TArray<AWeapon*> CharacterCurrentWeaponry;

	virtual void BeginPlay() override;

	virtual void InitializeAttributes();

	virtual void PostInitializeComponents() override;

	void SetDefaultPlayerPerspective();
	void SetLoadoutWeaponry();

	void MoveForward(float Value);
	void MoveRight(float Value);
	void LookUp(float Value);
	void Turn(float Value);
	void BindASCInput();

	UFUNCTION(Server, Reliable)
		void Server_EquipWeapon(AWeapon* WeaponToEquip);
	UFUNCTION(Server, Reliable)
		void Server_SyncCurrentWeapon();
	UFUNCTION(Client, Reliable)
		void Client_SyncCurrentWeapon(AWeapon* Weapon);

	void SetCurrentWeapon(AWeapon* NewWeapon, AWeapon* LastWeapon);
	void EquipWeapon(AWeapon* WeaponToEquip);
	void UnequipWeapon(AWeapon* WeaponToUnequip);

	UFUNCTION()
		void OnRep_CurrentWeapon(AWeapon* LastWeapon);

	virtual void OnRep_Controller() override;
	virtual void OnRep_PlayerState() override;
};
