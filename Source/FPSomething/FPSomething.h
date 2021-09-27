// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EAbilityInputID : uint8
{
	None UMETA(DisplayName = "None"),
	MainAttack UMETA(DisplayName = "MainAttack"),
	AltAttack UMETA(DisplayName = "AltAttack"),
	Reload UMETA(DisplayName = "Reload"),
	Jump UMETA(DisplayName = "Jump"),
	Interact UMETA(DisplayName = "Interact"),
	NextWeapon UMETA(DisplayName = "Next Weapon"),
	PreviousWeapon UMETA(DisplayName = "Previous Weapon"),
	Gear1 UMETA(DisplayName = "Gear1"),
	Gear2 UMETA(DisplayName = "Gear2"),
	Gear3 UMETA(DisplayName = "Gear3"),
	Gear4 UMETA(DisplayName = "Gear4"),
	Gear5 UMETA(DisplayName = "Gear5")
};