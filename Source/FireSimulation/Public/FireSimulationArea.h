// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FireVoxel2.h"
#include "FireSimulationVoxel.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FireSimulationArea.generated.h"


UCLASS()
class FIRESIMULATION_API AFireSimulationArea : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFireSimulationArea();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void DrawDebugOverlay();
	virtual void DivideSpace();
	virtual void UpdateNeighbours(float DeltaTime);
	virtual void UpdateCombustion(float DeltaTime);
	virtual FVector GetOffsetVoxel(FVector CubePosition);
	virtual void RegisterConsoleCommands();
	virtual void UnregisterConsoleCommands();
	virtual void ToggleDebugOverlay();
	virtual void ToggleDebugTextOverlay();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual float CalculateHeatTransfer(float Temperature1, float Temperature2, float DeltaTime, float delta);
	virtual FColor GetHeatMapColorForVoxel(float Temperature);


public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
	FVector GridSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
	float CubeSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Simulation")
	TArray<UFireVoxel2*> Voxels;

	int DebugElements = 1000;

	UPROPERTY()
	bool bDebugOverlayEnabled = false;
	UPROPERTY()
	bool bDebugOverlayTextEnabled = false;
	int32 DebugBoxCount;

	double MinTemperature = 20.0;
	double MaxTemperature = 100000.0;
	//double currentMaxTemp = 0;

	double TotalEnergy;

	TTuple<double, double> MaxMinEnergy = TTuple<float, float>(999999999999, 0);

	//UPROPERTY(EditAnywhere, Category = "Simulation")
	//TSubclassOf<UFireVoxel2> CubeClass;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
