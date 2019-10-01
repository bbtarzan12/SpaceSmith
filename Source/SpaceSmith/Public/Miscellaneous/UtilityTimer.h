// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class SPACESMITH_API UtilityTimer
{
	int64 TickTime = 0;
	int64 TockTime = 0;
public:
	int64 UnixTimeNow()
	{
		FDateTime timeUtc = FDateTime::UtcNow();
		return timeUtc.ToUnixTimestamp() * 1000 + timeUtc.GetMillisecond();
	}

	void Tick()
	{
		TickTime = UnixTimeNow();
	}

	int32 Tock()
	{
		TockTime = UnixTimeNow();
		return TockTime - TickTime;
	}
};


