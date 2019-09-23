// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <ListView.h>
#include "KeyInformationWidget.generated.h"

UCLASS(BlueprintType)
class UKeyInformation : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	FText Key;

	UPROPERTY(VisibleAnywhere)
	FText KeyInformation;

};

/**
 * 
 */
UCLASS()
class SPACESMITH_API UKeyInformationWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Add(UKeyInformation* Information);
	void Clear();

	UPROPERTY(meta = (BindWidget))
	class UListView* InformationList;
	
};
