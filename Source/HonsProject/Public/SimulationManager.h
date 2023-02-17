// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NBodyHandler.h"
#include "AccuracyModule.h"


#include "SimulationManager.generated.h"

UCLASS()
class HONSPROJECT_API ASimulationManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASimulationManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UNBodyHandler* BodyHandler;
	//UNBodyHandler* ghostSim;

	//simulation dependant variables
	UPROPERTY(Category = "SimulationType", EditAnywhere, BlueprintReadWrite)
		bool useTreeCodes = false;
	UPROPERTY(Category = "SimulationType", EditAnywhere, BlueprintReadWrite)
		bool shouldSpawnSolarSystem = false;
	UPROPERTY(Category = "SimulationType", EditAnywhere, BlueprintReadWrite)
		bool ShouldSpawnTestPlanets = false;

	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		int bodiesToSpawn = 100;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		int SpawnsPerFrame = 50;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		FVector InitialSpawnCentre = FVector(0, 0, -10);
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		float SpawnLocationBounds = 10;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		float SpawnInitialMaxSpeed = 10;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		float SpawnInitialMaxMass = 2;
	UPROPERTY(Category = "SimulationRelevant", BlueprintReadWrite, EditAnywhere)
		float fixedFrameTime = 0.1f;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		int SolarPlanetToSpawn = 100;//0-7 to spawn a specific planet+ sun,anything else for all of them

	UPROPERTY(Category = "Testing", BlueprintReadWrite, EditAnywhere)
		bool ShouldReset = false;
	UPROPERTY(Category = "Testing", BlueprintReadWrite, EditAnywhere)
		float resetTime = 1.0f;
	UPROPERTY(Category = "Testing", BlueprintReadWrite, EditAnywhere)
		bool shouldGhostAccuracy = false;

	UPROPERTY(Category = "Testing", BlueprintReadWrite, EditAnywhere)
		bool testOnlyMove = false;
	
};
