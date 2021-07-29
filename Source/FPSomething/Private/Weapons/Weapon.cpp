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
	ClipAmmo = 0;
	MaxClipAmmo = 0;
	ReserveAmmo = 0;
	MaxReserveAmmo = 0;
	bInfiniteAmmo = false;

	WeaponMesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(FName("WeaponMesh1P"));
	WeaponMesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh1P->CastShadow = false;
	WeaponMesh1P->SetVisibility(false, true);
	WeaponMesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;

	WeaponMesh3PickupRelativeLocation = FVector(0.0f, -25.0f, 0.0f);

	WeaponMesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(FName("WeaponMesh3P"));
	WeaponMesh3P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh3P->SetRelativeLocation(WeaponMesh3PickupRelativeLocation);
	WeaponMesh3P->CastShadow = true;
	WeaponMesh3P->SetVisibility(true, true);
	WeaponMesh3P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;

	WeaponIsFiringTag = FGameplayTag::RequestGameplayTag("Weapon.IsFiring");
}

UAbilitySystemComponent* AWeapon::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

USkeletalMeshComponent* AWeapon::GetWeaponMesh1P() const
{
	return WeaponMesh1P;
}

USkeletalMeshComponent* AWeapon::GetWeaponMesh3P() const
{
	return WeaponMesh3P;
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AWeapon, OwningCharacter, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AWeapon, ClipAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AWeapon, MaxClipAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AWeapon, ReserveAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AWeapon, MaxReserveAmmo, COND_OwnerOnly);
}

void AWeapon::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);

	DOREPLIFETIME_ACTIVE_OVERRIDE(AWeapon, ClipAmmo, (IsValid(AbilitySystemComponent) && !AbilitySystemComponent->HasMatchingGameplayTag(WeaponIsFiringTag)));
	DOREPLIFETIME_ACTIVE_OVERRIDE(AWeapon, ReserveAmmo, (IsValid(AbilitySystemComponent) && !AbilitySystemComponent->HasMatchingGameplayTag(WeaponIsFiringTag)));
}

void AWeapon::SetOwningCharacter(ACharacterBase* InOwningCharacter)
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
			WeaponMesh3P->CastShadow = false;
			WeaponMesh3P->SetVisibility(true, true);
			WeaponMesh3P->SetVisibility(false, true);
		}
	}
	else
	{
		AbilitySystemComponent = nullptr;
		SetOwner(nullptr);
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
}

void AWeapon::Equip()
{
	if (!OwningCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("%s %s OwningCharacter is nullptr"), *FString(__FUNCTION__), *GetName());
		return;
	}

	FName AttachPoint = OwningCharacter->GetWeaponAttachPoint();

	if (WeaponMesh1P)
	{
		WeaponMesh1P->AttachToComponent(OwningCharacter->GetFirstPersonMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, AttachPoint);
		WeaponMesh1P->SetRelativeLocation(WeaponMesh1PEquippedRelativeLocation);
		WeaponMesh1P->SetRelativeRotation(FRotator(0, 0, -90.0f));
		WeaponMesh1P->SetVisibility(true, true);

	}
}

void AWeapon::Unequip()
{
	if (OwningCharacter == nullptr)
	{
		return;
	}

	// Necessary to detach so that when toggling perspective all meshes attached won't become visible.

	WeaponMesh1P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	WeaponMesh1P->SetVisibility(false, true);

	WeaponMesh3P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	WeaponMesh3P->CastShadow = false;
	WeaponMesh3P->bCastHiddenShadow = false;
	WeaponMesh3P->SetVisibility(true, true); // Without this, the unequipped weapon's 3p shadow hangs around
	WeaponMesh3P->SetVisibility(false, true);
}

void AWeapon::AddAbilities()
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

void AWeapon::RemoveAbilities()
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

	// Remove abilities, but only on the server	
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	for (FGameplayAbilitySpecHandle& SpecHandle : AbilitySpecHandles)
	{
		ASC->ClearAbility(SpecHandle);
	}
}

int32 AWeapon::GetClipAmmo() const
{
	return ClipAmmo;
}

int32 AWeapon::GetMaxClipAmmo() const
{
	return MaxClipAmmo;
}

int32 AWeapon::GetReserveAmmo() const
{
	return ReserveAmmo;
}

int32 AWeapon::GetMaxReserveAmmo() const
{
	return MaxReserveAmmo;
}

void AWeapon::SetClipAmmo(int32 NewClipAmmo)
{
	int32 OldClipAmmo = ClipAmmo;
	ClipAmmo = NewClipAmmo;
	OnClipAmmoChanged.Broadcast(OldClipAmmo, ClipAmmo);
}

void AWeapon::SetMaxClipAmmo(int32 NewMaxClipAmmo)
{
	int32 OldMaxClipAmmo = MaxClipAmmo;
	MaxClipAmmo = NewMaxClipAmmo;
	OnMaxClipAmmoChanged.Broadcast(OldMaxClipAmmo, MaxClipAmmo);
}

void AWeapon::SetReserveAmmo(int32 NewReserveAmmo)
{
	int32 OldReserveAmmo = ReserveAmmo;
	ReserveAmmo = NewReserveAmmo;
	OnReserveAmmoChanged.Broadcast(OldReserveAmmo, ReserveAmmo);
}

void AWeapon::SetMaxReserveAmmo(int32 NewMaxReserveAmmo)
{
	int32 OldMaxReserveAmmo = MaxReserveAmmo;
	MaxReserveAmmo = NewMaxReserveAmmo;
	OnMaxReserveAmmoChanged.Broadcast(OldMaxReserveAmmo, MaxReserveAmmo);
}

UAnimMontage* AWeapon::GetEquip1PMontage() const
{
	return Equip1PMontage;
}

UAnimMontage* AWeapon::GetEquip3PMontage() const
{
	return Equip3PMontage;
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

void AWeapon::OnRep_ClipAmmo(int32 OldClipAmmo)
{
	OnClipAmmoChanged.Broadcast(OldClipAmmo, ClipAmmo);
}

void AWeapon::OnRep_MaxClipAmmo(int32 OldMaxClipAmmo)
{
	OnMaxClipAmmoChanged.Broadcast(OldMaxClipAmmo, MaxClipAmmo);
}

void AWeapon::OnRep_ReserveAmmo(int32 OldReserveAmmo)
{
	OnReserveAmmoChanged.Broadcast(OldReserveAmmo, ReserveAmmo);
}

void AWeapon::OnRep_MaxReserveAmmo(int32 OldMaxReserveAmmo)
{
	OnMaxReserveAmmoChanged.Broadcast(OldMaxReserveAmmo, MaxReserveAmmo);
}


