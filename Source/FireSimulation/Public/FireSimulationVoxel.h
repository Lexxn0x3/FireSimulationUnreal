// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FireSimulationVoxel.generated.h"

UCLASS()
class FIRESIMULATION_API AFireSimulationVoxel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFireSimulationVoxel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Temperature;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float OxygenLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EnergyRemaining;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float IgnitionTemp;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
