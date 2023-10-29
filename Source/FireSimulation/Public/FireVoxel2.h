// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FireVoxel2.generated.h"

/**
 * 
 */
UCLASS()
class FIRESIMULATION_API UFireVoxel2 : public UObject
{
	GENERATED_BODY()
	

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double Temperature;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float OxygenLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double FuelRemaining;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double IgnitionTemp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Ignited;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Position;

	double delta;

	virtual void Update(float DeltaTime);

protected:
	virtual bool isBurning();

};
