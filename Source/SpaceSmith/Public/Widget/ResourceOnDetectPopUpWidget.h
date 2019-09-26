// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Tickable.h>
#include "ResourceOnDetectPopUpWidget.generated.h"

/**
 * 
 */
UCLASS()
class SPACESMITH_API UResourceOnDetectPopUpWidget : public UUserWidget, public FTickableGameObject
{
	GENERATED_BODY()
	
public:
	// Implementing FTickableGameObject functions
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;


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
