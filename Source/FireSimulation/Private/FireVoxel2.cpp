// Fill out your copyright notice in the Description page of Project Settings.


#include "FireVoxel2.h"

bool UFireVoxel2::isBurning()
{
	return Temperature >= IgnitionTemp && FuelRemaining > 100.f && OxygenLevel > 0;
}

void UFireVoxel2::Update(float DeltaTime)
{
	Ignited = isBurning();

	if (Ignited)
	{
		if (FuelRemaining > 100.f)
		{
			FuelRemaining -= DeltaTime * 100.f;
			Temperature += DeltaTime * 1100.f;
		}
	}
}
