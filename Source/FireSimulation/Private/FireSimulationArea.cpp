// Fill out your copyright notice in the Description page of Project Settings.


#include "FireVoxel2.h"
#include "FireSimulationVoxel.h"
#include "FireSimulationArea.h"
#include "DrawDebugHelpers.h"

// Sets default values
AFireSimulationArea::AFireSimulationArea()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFireSimulationArea::BeginPlay()
{
	Super::BeginPlay();
    RegisterConsoleCommands();
    DivideSpace();
	
}

void AFireSimulationArea::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    UnregisterConsoleCommands();
}

// Called every frame
void AFireSimulationArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    TotalEnergy = 0;

    UpdateCombustion(DeltaTime);
    UpdateNeighbours(DeltaTime);

    DebugBoxCount = IConsoleManager::Get().FindConsoleVariable(TEXT("DebugBoxCount"))->GetInt();
    if (bDebugOverlayEnabled)
        DrawDebugOverlay();
}

void AFireSimulationArea::UpdateCombustion(float DeltaTime)
{
    for (UFireVoxel2* Voxel : Voxels)
    {
        TotalEnergy += Voxel->FuelRemaining * 11.f + Voxel->Temperature;
        

        Voxel->Update(DeltaTime);
    }
    if (TotalEnergy < MaxMinEnergy.Key)
        MaxMinEnergy.Key = TotalEnergy;
    if (TotalEnergy > MaxMinEnergy.Value)
        MaxMinEnergy.Value = TotalEnergy;

    FString EnergyText = FString::Printf(TEXT("Energy: %.1f\nMax: %f\nMin: %f\nEfficiency: %f %"), TotalEnergy, MaxMinEnergy.Value, MaxMinEnergy.Key, (MaxMinEnergy.Key/MaxMinEnergy.Value)*100);  

    DrawDebugString(GetWorld(), GetActorLocation(), EnergyText, nullptr, FColor::White, 0.f, true);
}

void AFireSimulationArea::UpdateNeighbours(float DeltaTime)
{
    const FVector Directions[6] = {
        FVector(1, 0, 0),  // Right
        FVector(-1, 0, 0), // Left
        FVector(0, 1, 0),  // Up
        FVector(0, -1, 0), // Down
        FVector(0, 0, 1),  // Forward
        FVector(0, 0, -1)  // Backward
    };

    TMap<FVector, UFireVoxel2*> VoxelMap;
    for (UFireVoxel2* Voxel : Voxels)
    {
        VoxelMap.Add(Voxel->Position, Voxel);
    }

    for (UFireVoxel2* Voxel : Voxels)
    {
        for (const FVector& Direction : Directions)
        {
            FVector NeighborPosition = Voxel->Position + Direction;
            UFireVoxel2** NeighborVoxelPtr = VoxelMap.Find(NeighborPosition);
            if (NeighborVoxelPtr)
            {
                UFireVoxel2* NeighborVoxel = *NeighborVoxelPtr;
                // Apply heat transfer equation
                float HeatTransfer = CalculateHeatTransfer(Voxel->Temperature, NeighborVoxel->Temperature, DeltaTime, Voxel->delta);
                Voxel->Temperature -= HeatTransfer;
                NeighborVoxel->Temperature += HeatTransfer;
            }
        }
    }
}

float AFireSimulationArea::CalculateHeatTransfer(float Temperature1, float Temperature2, float DeltaTime, float delta)
{
    // Implement your heat transfer equation here
    float HeatTransfer = delta * (Temperature1 - Temperature2) * DeltaTime;
    return HeatTransfer;
}

void AFireSimulationArea::DivideSpace()
{
    const int32 XCount = FMath::CeilToInt(GridSize.X / CubeSize);
    const int32 YCount = FMath::CeilToInt(GridSize.Y / CubeSize);
    const int32 ZCount = FMath::CeilToInt(GridSize.Z / CubeSize);

    FVector AreaPosition = GetActorLocation();

    for (int32 X = 0; X < XCount; ++X)
    {
        for (int32 Y = 0; Y < YCount; ++Y)
        {
            for (int32 Z = 0; Z < ZCount; ++Z)
            {
                FVector CubePosition = FVector(X, Y , Z);
                //AFireSimulationVoxel* NewCube = GetWorld()->SpawnActor<AFireSimulationVoxel>(CubeClass, CubePosition, FRotator::ZeroRotator);
                //NewCube->SetActorLocation(CubePosition);

                UFireVoxel2* NewVoxel = NewObject<UFireVoxel2>(this);
                NewVoxel->Position = CubePosition;

                if (CubePosition.X == CubePosition.Y && CubePosition.Y == CubePosition.Z && CubePosition.Z == 0)
                {
                    NewVoxel->Temperature = FMath::RandRange(550.f, 1250.f);  // Random temperature between 950 and 1050
                }
                else
                {
                    NewVoxel->Temperature = FMath::RandRange(10.f, 30.f);  // Random temperature between 10 and 30
                }
                NewVoxel->Ignited = false;
                NewVoxel->IgnitionTemp = FMath::RandRange(350.f, 1550.f);  // Random ignition temperature between 450 and 550
                NewVoxel->FuelRemaining = FMath::RandRange(500.f, 11000.f);  // Random fuel remaining between 9000 and 11000
                NewVoxel->OxygenLevel = FMath::RandRange(900.f, 1100.f);  // Random oxygen level between 900 and 1100
                NewVoxel->delta = FMath::RandRange(0.05f, 0.3f);  // Random delta between 0.1 and 0.3


                Voxels.Add(NewVoxel);
            }
        }
    }
}

void AFireSimulationArea::DrawDebugOverlay()
{
    FVector Origin = GetActorLocation();

    FVector Extent(GridSize.X / 2, GridSize.Y / 2, GridSize.Z / 2);  // Half the size of the simulation area along each axis

    FVector Center = Origin + Extent; //Center of simulation Area, origin is at 0,0,0

    DrawDebugSphere(GetWorld(), GetActorLocation(), 10.0, 50, FColor::Blue, false, -1.f, 2.f);
    //Green box
    DrawDebugBox(GetWorld(), Center, Extent, FColor::Green, false, -1.f, 0, 1.f);

    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    FVector CameraLocation;
    if (PlayerController && PlayerController->PlayerCameraManager)
    {
        CameraLocation = PlayerController->PlayerCameraManager->GetCameraLocation();
        //UE_LOG(LogTemp, Warning, TEXT("%.1f,%.1f"), CameraLocation.X, CameraLocation.Y);
    }
    else
    {
        // Handle error (e.g., log a warning and return early)
        UE_LOG(LogTemp, Warning, TEXT("No PlayerController or PlayerCameraManager found."));
        return;
    }
    struct FVoxelDistance
    {
        UFireVoxel2* Voxel;
        float DistanceSquared;
    };

    // Array to hold the 100 closest voxels
    TArray<FVoxelDistance> ClosestVoxels;
    ClosestVoxels.Reserve(DebugElements);  // Reserve space for 100 elements

    MaxTemperature = Voxels[0]->Temperature;
    MinTemperature = Voxels[0]->Temperature;

    for (UFireVoxel2* Voxel : Voxels)
    {
        if (Voxel->Temperature > MaxTemperature)
        {
            MaxTemperature = Voxel->Temperature;
        }
        if (Voxel->Temperature < MinTemperature)
        {
            MinTemperature = Voxel->Temperature;
        }
            

        float DistanceSquared = FVector::DistSquared(GetOffsetVoxel(Voxel->Position), CameraLocation);
        FVoxelDistance VoxelDistance = { Voxel, DistanceSquared };

        // If we have fewer than 100 voxels, just add the voxel to the array
        if (ClosestVoxels.Num() < DebugElements)
        {
            ClosestVoxels.Add(VoxelDistance);
        }
        else
        {
            // Find the farthest voxel among the closest voxels
            int32 FarthestVoxelIndex = 0;
            for (int32 i = 1; i < DebugElements; ++i)
            {
                if (ClosestVoxels[i].DistanceSquared > ClosestVoxels[FarthestVoxelIndex].DistanceSquared)
                {
                    FarthestVoxelIndex = i;
                }
            }

            // If the current voxel is closer than the farthest voxel, replace it
            if (DistanceSquared < ClosestVoxels[FarthestVoxelIndex].DistanceSquared)
            {
                ClosestVoxels[FarthestVoxelIndex] = VoxelDistance;
            }
        }
    }

    // Draw strings for the closest voxels
    for (const FVoxelDistance& VoxelDistance : ClosestVoxels)
    {
        UFireVoxel2* Voxel = VoxelDistance.Voxel;
        FVector CubePosition = GetOffsetVoxel(Voxel->Position);
        FString TemperatureText = FString::Printf(TEXT("Temperature: %.1f\nFuel: %.1f"), Voxel->Temperature, Voxel->FuelRemaining);

        if (bDebugOverlayTextEnabled)
            DrawDebugString(GetWorld(), CubePosition, TemperatureText, nullptr, FColor::White, 0.f, true);

        DrawDebugBox(GetWorld(), CubePosition, FVector(CubeSize) / 2.f, GetHeatMapColorForVoxel(Voxel->Temperature), false, 0.f, 0, 2.f);
        DrawDebugPoint(GetWorld(), CubePosition, 8.f, Voxel->Ignited ? FColor::Red : FColor::White, false, 0, 1);
    }

}

FColor AFireSimulationArea::GetHeatMapColorForVoxel(float Temperature)
{
    // Normalize the temperature to a value between 0 and 1
    float NormalizedTemperature = FMath::Clamp((Temperature - MinTemperature) / (MaxTemperature - MinTemperature), 0.f, 1.f);

    // Interpolate between blue and red based on the normalized temperature
    FLinearColor Color = FMath::Lerp(FLinearColor::Blue, FLinearColor::Red, NormalizedTemperature);

    // Convert the linear color to a gamma-corrected color
    FColor GammaCorrectedColor = Color.ToFColor(true);

    return GammaCorrectedColor;
}

FVector AFireSimulationArea::GetOffsetVoxel(FVector CubePosition)
{
    FVector Origin = GetActorLocation();

    return FVector(CubePosition.X * CubeSize + CubeSize / 2 + Origin.X, CubePosition.Y * CubeSize + CubeSize / 2 + Origin.Y, CubePosition.Z * CubeSize + CubeSize / 2 + Origin.Z);
}

void AFireSimulationArea::RegisterConsoleCommands()
{
    if (IConsoleManager::Get().FindConsoleVariable(TEXT("ToggleDebugOverlay")) == nullptr)
    {
        IConsoleManager::Get().RegisterConsoleCommand(
            TEXT("ToggleDebugOverlay"),
            TEXT("Toggle the debug overlay for the fire simulation area."),
            FConsoleCommandDelegate::CreateUObject(this, &AFireSimulationArea::ToggleDebugOverlay),
            ECVF_Default
        );
    }
    if (IConsoleManager::Get().FindConsoleVariable(TEXT("ToggleDebugOverlayText")) == nullptr)
    {
        IConsoleManager::Get().RegisterConsoleCommand(
            TEXT("ToggleDebugOverlayText"),
            TEXT("Toggle the debug overlay text for the fire simulation area."),
            FConsoleCommandDelegate::CreateUObject(this, &AFireSimulationArea::ToggleDebugTextOverlay),
            ECVF_Default
        );
    }
    if (IConsoleManager::Get().FindConsoleVariable(TEXT("DebugBoxCount")) == nullptr)
    {
        IConsoleManager::Get().RegisterConsoleVariable(
            TEXT("DebugBoxCount"),
            5000,
            TEXT("Number of debug boxes to display."),
            ECVF_Default
        );
    }
}

void AFireSimulationArea::UnregisterConsoleCommands()
{
    IConsoleManager::Get().UnregisterConsoleObject(TEXT("ToggleDebugOverlay"));
    IConsoleManager::Get().UnregisterConsoleObject(TEXT("ToggleDebugOverlayText"));
    IConsoleManager::Get().UnregisterConsoleObject(TEXT("DebugBoxCount"), false);
}

void AFireSimulationArea::ToggleDebugOverlay()
{
    bDebugOverlayEnabled = !bDebugOverlayEnabled;
}
void AFireSimulationArea::ToggleDebugTextOverlay()
{
    bDebugOverlayTextEnabled = !bDebugOverlayTextEnabled;
}