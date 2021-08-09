// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/CharacterBase.h"
#include "Characters/CustomMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "Weapons/Weapon.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Characters/Abilities/FPSmthAbilitySystemComponent.h"
#include "Characters/Abilities/FPSmthGameplayAbility.h"

ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UCustomMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = false;
	bChangedWeaponLocally = false;

	AbilitySystemComponent = CreateDefaultSubobject<UFPSmthAbilitySystemComponent>(TEXT("Ability System Component"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	FPCamera = CreateDefaultSubobject<UCameraComponent>(FName("FirstPersonCamera"));
	FPCamera->SetupAttachment(RootComponent);
	FPCamera->bUsePawnControlRotation = true;

	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("FirstPersonMesh"));
	FirstPersonMesh->SetupAttachment(FPCamera);
	FirstPersonMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));
	FirstPersonMesh->bReceivesDecals = false;
	FirstPersonMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
	FirstPersonMesh->CastShadow = false;
	FirstPersonMesh->SetVisibility(false, true);

	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionProfileName(FName("NoCollision"));
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	GetMesh()->bCastHiddenShadow = true;
	GetMesh()->bReceivesDecals = false;

	AttributeSet = CreateDefaultSubobject<UAttributeSetBase>(TEXT("BaseAttributeSet"));
}

void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACharacterBase::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &ACharacterBase::LookUp);
	PlayerInputComponent->BindAxis("Turn", this, &ACharacterBase::Turn);

	AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, FGameplayAbilityInputBinds(FString("ConfirmTarget"),
		FString("CancelTarget"), FString("EAbilityInputID"), static_cast<int32>(EAbilityInputID::MainAttack), static_cast<int32>(EAbilityInputID::AltAttack)));
}

void ACharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	SetDefaultPlayerPerspective();
}

void ACharacterBase::SetDefaultPlayerPerspective()
{
	APlayerController* PC = Cast<APlayerController>(GetController());

	if (PC && PC->IsLocalController())
	{
		FirstPersonMesh->SetVisibility(true, true);
		GetMesh()->SetVisibility(false, true);
	}
}

void ACharacterBase::SetLoadoutWeaponry()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		return;
	}

	int32 NumWeaponClasses = CharacterLoadoutWeaponry.Num();
	for (int32 i = 0; i < NumWeaponClasses; i++)
	{
		AWeapon* NewWeapon = GetWorld()->SpawnActorDeferred<AWeapon>(CharacterLoadoutWeaponry[i],
			FTransform::Identity, this, this, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		NewWeapon->FinishSpawning(FTransform::Identity);

		bool bEquipFirstWeapon = i == 0;
		AddWeaponToCharacter(NewWeapon, bEquipFirstWeapon);
	}
}

void ACharacterBase::MoveForward(float Value)
{
	AddMovementInput(UKismetMathLibrary::GetForwardVector(FRotator(0, GetControlRotation().Yaw, 0)), Value);
}

void ACharacterBase::MoveRight(float Value)
{
	AddMovementInput(UKismetMathLibrary::GetRightVector(FRotator(0, GetControlRotation().Yaw, 0)), Value);
}

void ACharacterBase::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void ACharacterBase::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ACharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	GetWorldTimerManager().SetTimerForNextTick(this, &ACharacterBase::SetLoadoutWeaponry);
}

void ACharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ACharacterBase, CurrentWeapon, COND_SimulatedOnly);
}

UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ACharacterBase::AddWeaponToCharacter(AWeapon* NewWeapon, bool bEquipWeapon)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		return;
	}

	CharacterCurrentWeaponry.Add(NewWeapon);
	NewWeapon->SetOwningCharacter(this);
	NewWeapon->AddAbilities();

	if (bEquipWeapon)
	{
		EquipWeapon(NewWeapon);
		ClientSyncCurrentWeapon(CurrentWeapon);
	}
}

void ACharacterBase::AddCharacterAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !IsValid(AbilitySystemComponent) || AbilitySystemComponent->bCharacterAbilitiesGiven)
	{
		return;
	}

	for (TSubclassOf<UFPSmthGameplayAbility>& StartupAbility : CharacterAbilities)
	{
		AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(StartupAbility, 1, static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this));
	}

	AbilitySystemComponent->bCharacterAbilitiesGiven = true;
}

void ACharacterBase::RemoveCharacterAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !IsValid(AbilitySystemComponent) || !AbilitySystemComponent->bCharacterAbilitiesGiven)
	{
		return;
	}

	TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if ((Spec.SourceObject == this) && CharacterLoadoutWeaponry.Contains(Spec.Ability->GetClass()))
		{
			AbilitiesToRemove.Add(Spec.Handle);
		}
	}

	for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
	{
		AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
	}

	AbilitySystemComponent->bCharacterAbilitiesGiven = false;
}

void ACharacterBase::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerEquipWeapon(WeaponToEquip);
		SetCurrentWeapon(WeaponToEquip, CurrentWeapon);
		bChangedWeaponLocally = true;
	}
	else
	{
		SetCurrentWeapon(WeaponToEquip, CurrentWeapon);
	}
}

void ACharacterBase::ServerEquipWeapon_Implementation(AWeapon* WeaponToEquip)
{
	EquipWeapon(WeaponToEquip);
}

bool ACharacterBase::ServerEquipWeapon_Validate(AWeapon* WeaponToEquip)
{
	return true;
}

AWeapon* ACharacterBase::GetCurrentWeapon() const
{
	return CurrentWeapon;
}

void ACharacterBase::ServerSyncCurrentWeapon_Implementation()
{
	ClientSyncCurrentWeapon(CurrentWeapon);
}

bool ACharacterBase::ServerSyncCurrentWeapon_Validate()
{
	return true;
}

void ACharacterBase::ClientSyncCurrentWeapon_Implementation(AWeapon* Weapon)
{
	AWeapon* LastWeapon = CurrentWeapon;
	CurrentWeapon = Weapon;
	OnRep_CurrentWeapon(LastWeapon);
}

bool ACharacterBase::ClientSyncCurrentWeapon_Validate(AWeapon* Weapon)
{
	return true;
}

FName ACharacterBase::GetWeaponAttachPoint()
{
	return WeaponAttachPoint;
}

USkeletalMeshComponent* ACharacterBase::GetFirstPersonMesh() const
{
	return FirstPersonMesh;
}

USkeletalMeshComponent* ACharacterBase::GetThirdPersonMesh() const
{
	return GetMesh();
}

void ACharacterBase::Die()
{
	RemoveCharacterAbilities();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->GravityScale = 0;
	GetCharacterMovement()->Velocity = FVector(0);

	OnCharacterDied.Broadcast(this);

	if (IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->CancelAllAbilities();

		FGameplayTagContainer EffectTagsToRemove;
		int32 NumEffectsRemoved = AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectTagsToRemove);

		AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
	}

	FinishDying();
}

void ACharacterBase::FinishDying()
{
	Destroy();
}

void ACharacterBase::SetCurrentWeapon(AWeapon* NewWeapon, AWeapon* LastWeapon)
{
	if (NewWeapon == LastWeapon)
	{
		return;
	}

	if (AbilitySystemComponent)
	{
		FGameplayTagContainer AbilityTagsToCancel = FGameplayTagContainer(WeaponAbilityTag);
		AbilitySystemComponent->CancelAbilities(&AbilityTagsToCancel);
	}

	UnequipWeapon(LastWeapon);

	if (NewWeapon)
	{
		if (AbilitySystemComponent)
		{
			AbilitySystemComponent->RemoveLooseGameplayTag(CurrentWeaponTag);
		}

		CurrentWeapon = NewWeapon;
		CurrentWeapon->SetOwningCharacter(this);
		CurrentWeapon->Equip();
		CurrentWeaponTag = CurrentWeapon->WeaponTag;

		if (AbilitySystemComponent)
		{
			AbilitySystemComponent->AddLooseGameplayTag(CurrentWeaponTag);
		}

		UAnimMontage* Equip1PMontage = CurrentWeapon->GetFPEquipMontage();
		if (Equip1PMontage && GetFirstPersonMesh())
		{
			GetFirstPersonMesh()->GetAnimInstance()->Montage_Play(Equip1PMontage);
		}

		UAnimMontage* Equip3PMontage = CurrentWeapon->GetTPEquipMontage();
		if (Equip3PMontage && GetThirdPersonMesh())
		{
			GetThirdPersonMesh()->GetAnimInstance()->Montage_Play(Equip3PMontage);
		}
	}
}

void ACharacterBase::UnequipWeapon(AWeapon* WeaponToUnequip)
{
	if (WeaponToUnequip)
	{
		WeaponToUnequip->Unequip();
	}
}

void ACharacterBase::OnRep_Controller()
{
	Super::OnRep_Controller();

	SetDefaultPlayerPerspective();
}

void ACharacterBase::OnRep_CurrentWeapon(AWeapon* LastWeapon)
{
	bChangedWeaponLocally = false;
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
}

float ACharacterBase::GetMoveSpeed() const
{
	if (IsValid(AttributeSet))
	{
		return AttributeSet->GetMoveSpeed();
	}

	return 0.0f;
}