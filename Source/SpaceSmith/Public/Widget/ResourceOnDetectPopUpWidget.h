// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResourceOnDetectPopUpWidget.generated.h"

/**
 * 
 */
UCLASS()
class SPACESMITH_API UResourceOnDetectPopUpWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere)
	class ABaseResource* Resource;
	
	UPROPERTY(meta = (BindWidget))
	class UImage* Image;

	UFUNCTION(BlueprintCallable)
	UTexture2D* GetTexture() const;

	UFUNCTION(BlueprintImplementableEvent)
	void OnInitDetect();

	UFUNCTION(BlueprintImplementableEvent)
	void OnEndDetect();


};
