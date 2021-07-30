// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CustomMovementComponent.generated.h"

/**
 *
 */

UCLASS()
class FPSOMETHING_API UCustomMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	class FCustomSavedMove : public FSavedMove_Character
	{
	public:

		typedef FSavedMove_Character Super;

		virtual void Clear() override;
		virtual uint8 GetCompressedFlags() const override;
		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;
		virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;
		virtual void PrepMoveFor(class ACharacter* Character) override;

		uint8 SavedRequestToTurnOnSlowFall : 1;
	};

	class FCustomNetworkPredictionData_Client : public FNetworkPredictionData_Client_Character
	{
	public:
		FCustomNetworkPredictionData_Client(const UCharacterMovementComponent& ClientMovement);

		typedef FNetworkPredictionData_Client_Character Super;

		virtual FSavedMovePtr AllocateNewMove() override;
	};

public:
	UCustomMovementComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SlowFallGravityMultiplier;

	uint8 RequestToTurnOnSlowFall : 1;

	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual float GetGravityZ() const override;

	UFUNCTION(BlueprintCallable)
		void TurnOnSlowFall();
	UFUNCTION(BlueprintCallable)
		void TurnOffSlowFall();
	UFUNCTION(BlueprintCallable)
		bool CanTurnOnSlowFall() const;
	UFUNCTION(BlueprintCallable)
		void SetMaxJumps(int32 NewMaxJumps);


};
