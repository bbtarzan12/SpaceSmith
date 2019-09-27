// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Engine/DataTable.h>
#include "CraftingRuleDataTable.generated.h"

USTRUCT(BlueprintType)
struct FCraftingRulePair
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle Handle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Num;
};

USTRUCT(BlueprintType)
struct FCraftingRule
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCraftingRulePair> In;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCraftingRulePair> Out;
};

USTRUCT(BlueprintType)
struct FCraftingRuleRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCraftingRule Info;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RequireEnergy;
};

UCLASS()
class SPACESMITH_API ACraftingRuleDataTable : public AActor
{
	GENERATED_BODY()

};
