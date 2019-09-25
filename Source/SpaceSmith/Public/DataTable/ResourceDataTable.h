// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Engine/DataTable.h>
#include "ResourceDataTable.generated.h"

class ABaseResource;

USTRUCT(BlueprintType)
struct FResourceAbsorbInformation
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle Row;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Num;
};

USTRUCT(BlueprintType)
struct FResourceRow : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	FResourceRow();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABaseResource> Class;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AbsorbTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FResourceAbsorbInformation AbsorbInformation;
};

UCLASS()
class SPACESMITH_API AResourceDataTable : public AActor
{
	GENERATED_BODY()
};
