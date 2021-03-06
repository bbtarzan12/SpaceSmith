// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Engine/DataTable.h>
#include "ItemDataTable.generated.h"

class ABaseItem;

USTRUCT(BlueprintType)
struct FItemRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	FItemRow();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Thumbnail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABaseItem> Class;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bStack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFire;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = bFire))
	float CoolTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFuel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = bFuel))
	float FuelEfficiency;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	bool operator==(const FItemRow& OtherItem) const;

	friend uint32 GetTypeHash(const FItemRow& Other)
	{
		return GetTypeHash(Other.ItemID);
	}
};



UCLASS()
class SPACESMITH_API AItemDataTable : public AActor
{
	GENERATED_BODY()
};
