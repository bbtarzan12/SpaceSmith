// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceDataTable.h"
#include "Resource/BaseResource.h"


FResourceRow::FResourceRow()
{
	Class = ABaseResource::StaticClass();
}
