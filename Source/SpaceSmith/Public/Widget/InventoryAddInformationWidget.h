// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <ListView.h>
#include "ItemDataTable.h"
#include "InventoryAddInformationWidget.generated.h"


UCLASS(BlueprintType)
class UInventoryAddInformation : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemRow Row;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Amount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LastAddTime;
};

/**
 * 
 */
UCLASS()
class SPACESMITH_API UInventoryAddInformationWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Add(UInventoryAddInformation* Information);
	void Clear();

	UPROPERTY(meta = (BindWidget))
	class UListView* InformationList;
	
};
