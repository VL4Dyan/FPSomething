// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserWidgetBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWidgetFinishes);

UCLASS()
class FPSOMETHING_API UUserWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
		FWidgetFinishes OnUIFinish;
};
