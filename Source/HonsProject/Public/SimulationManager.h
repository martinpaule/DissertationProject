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



	UPROPERTY(Category = "UI_references", EditAnywhere, BlueprintReadWrite)
	UNBodyHandler* BodyHandler_ref;
	UPROPERTY(Category = "UI_references", EditAnywhere, BlueprintReadWrite)
	UNBodyHandler* ghostSim_ref;
	UPROPERTY(Category = "UI_references", EditAnywhere, BlueprintReadWrite)
	UAccuracyModule* accuracyTester_ref;
	UPROPERTY(Category = "UI_references", EditAnywhere, BlueprintReadWrite)
	UTreeHandler* TreeHandler_ref;

	//UI called functions
	UFUNCTION(BlueprintCallable, Category = "SimSpeed")
		void raiseSimulationSpeed();
	UFUNCTION(BlueprintCallable, Category = "SimSpeed")
		void lowerSimulationSpeed();
	UFUNCTION(BlueprintCallable, Category = "SimSpeed")
		void pauseSimulation();
	UFUNCTION(BlueprintCallable, Category = "ClearSim")
		void ClearSimulation();
	
	
	UFUNCTION(BlueprintCallable, Category = "TESTING")
	void addGhostSim();

	UFUNCTION(BlueprintCallable, Category = "TESTING")
		void removeGhostSim();

	void createSimComponents();
	void recordFinalPositions();//<- move to accuracy class


	//simulation dependant variables
	UPROPERTY(Category = "SimulationType", EditAnywhere, BlueprintReadWrite)
		bool useTreeCodes = true;
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
	float elapsedFrameTime = 0.0f;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		int SolarPlanetToSpawn = 100;//0-7 to spawn a specific planet+ sun,anything else for all of them

	UPROPERTY(Category = "Testing", BlueprintReadWrite, EditAnywhere)
		bool ShouldReset = false;
	UPROPERTY(Category = "Testing", BlueprintReadWrite, EditAnywhere)
		float resetTime = 1.0f;
	UPROPERTY(Category = "Testing", BlueprintReadWrite, EditAnywhere)
		bool shouldGhostAccuracy = false;

	UPROPERTY(Category = "Testing", BlueprintReadWrite, EditAnywhere)
		bool Paused = false;
	
	//UI variables
	UPROPERTY(Category = "forUI", BlueprintReadWrite)
		float simulationElapsedTime = 0.0f;
	UPROPERTY(Category = "forUI", BlueprintReadWrite, EditAnywhere)
		int bodiesInSimulation = 0;
	UPROPERTY(Category = "forUI", BlueprintReadWrite, EditAnywhere)
		float timeMultiplier = 1.0f;



	UPROPERTY(Category = "Testing", BlueprintReadWrite, EditAnywhere)
		float averagePosError = 0.0f;

	void handleAveragePosError();
	UPROPERTY(Category = "Testing", BlueprintReadWrite, EditAnywhere)
		bool showGhosPlanetErrors = false;
	UPROPERTY(Category = "Testing", BlueprintReadWrite, EditAnywhere)
		bool calcAveragePosError = false;
	UPROPERTY(Category = "Testing", BlueprintReadWrite)
		bool doFrameCalc = false;


};

