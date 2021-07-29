// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/CustomMovementComponent.h"
#include "Characters/CharacterBase.h"

UCustomMovementComponent::UCustomMovementComponent()
{
	SlowFallGravityMultiplier = 0.5f;
}

void UCustomMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	RequestToTurnOnSlowFall = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
}

FNetworkPredictionData_Client* UCustomMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner != NULL);

	if (!ClientPredictionData)
	{
		UCustomMovementComponent* MutableThis = const_cast<UCustomMovementComponent*>(this);

		MutableThis->ClientPredictionData = new FCustomNetworkPredictionData_Client(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}

	return ClientPredictionData;
}

void UCustomMovementComponent::TurnOnSlowFall()
{
	RequestToTurnOnSlowFall = true;
}

void UCustomMovementComponent::TurnOffSlowFall()
{
	RequestToTurnOnSlowFall = false;
}

void UCustomMovementComponent::FCustomSavedMove::Clear()
{
	Super::Clear();

	SavedRequestToTurnOnSlowFall = false;
}

uint8 UCustomMovementComponent::FCustomSavedMove::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (SavedRequestToTurnOnSlowFall)
	{
		Result |= FLAG_Custom_0;
	}

	return Result;
}

bool UCustomMovementComponent::FCustomSavedMove::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const
{
	//Set which moves can be combined together. This will depend on the bit flags that are used.
	if (SavedRequestToTurnOnSlowFall != ((FCustomSavedMove*)&NewMove)->SavedRequestToTurnOnSlowFall)
	{
		return false;
	}

	return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

void UCustomMovementComponent::FCustomSavedMove::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	UCustomMovementComponent* CharacterMovement = Cast<UCustomMovementComponent>(Character->GetCharacterMovement());
	if (CharacterMovement)
	{
		SavedRequestToTurnOnSlowFall = CharacterMovement->RequestToTurnOnSlowFall;
	}
}

void UCustomMovementComponent::FCustomSavedMove::PrepMoveFor(ACharacter* Character)
{
	Super::PrepMoveFor(Character);

	UCustomMovementComponent* CharacterMovement = Cast<UCustomMovementComponent>(Character->GetCharacterMovement());
	if (CharacterMovement)
	{
	}
}

UCustomMovementComponent::FCustomNetworkPredictionData_Client::FCustomNetworkPredictionData_Client(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{
}

FSavedMovePtr UCustomMovementComponent::FCustomNetworkPredictionData_Client::AllocateNewMove()
{
	return FSavedMovePtr(new FCustomSavedMove());
}

bool UCustomMovementComponent::CanTurnOnSlowFall() const
{
	return IsFalling();
}

float UCustomMovementComponent::GetGravityZ() const
{
	if (RequestToTurnOnSlowFall)
	{
		return Super::GetGravityZ() * SlowFallGravityMultiplier;
	}

	return Super::GetGravityZ();
}