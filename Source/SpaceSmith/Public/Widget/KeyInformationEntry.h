// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <IUserObjectListEntry.h>
#include "KeyInformationEntry.generated.h"

/**
 * 
 */
UCLASS()
class SPACESMITH_API UKeyInformationEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	virtual void SetListItemObjectInternal(UObject* InObject) override;

private:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Key;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* KeyInformation;

	UPROPERTY(VisibleAnywhere)
	class UKeyInformation* Information;

	UFUNCTION(BlueprintCallable)
	FText GetKeyText() const;

	UFUNCTION(BlueprintCallable)
	FText GetKeyInformationText() const;
	
};
