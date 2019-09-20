// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Engine/DataTable.h>
#include "MachineDataTable.generated.h"


USTRUCT(BlueprintType)
struct FMachineRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	FMachineRow();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Thumbnail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bInventory"))
	int32 Capacity;

	bool operator==(const FMachineRow& Other) const;
};

UCLASS()
class SPACESMITH_API AMachineDataTable : public AActor
{
	GENERATED_BODY()
};
