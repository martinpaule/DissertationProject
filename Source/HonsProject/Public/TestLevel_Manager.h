// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NBody_Handler.h"
#include "Accuracy_Handler.h"
#include <chrono>



#include "TestLevel_Manager.generated.h"

UCLASS()
class HONSPROJECT_API ASimulationManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASimulationManager();
	
	// ----- Functionality components
	UPROPERTY(Category = "UI_references", EditAnywhere, BlueprintReadWrite)
		UNBodyHandler* BodyHandler_ref;
	UPROPERTY(Category = "UI_references", EditAnywhere, BlueprintReadWrite)
		UNBodyHandler* ghostSim_ref;
	UPROPERTY(Category = "UI_references", EditAnywhere, BlueprintReadWrite)
		UAccuracyModule* accuracyTester_ref;

	// ----- simulation dependant variables
	UPROPERTY(Category = "SimulationType", EditAnywhere, BlueprintReadWrite)
		bool useTreeCodes = false; //<- can move into nbody handler since 
	UPROPERTY(Category = "SimulationRelevant", BlueprintReadWrite, EditAnywhere)
		float fixedFrameTime = 0.1f;
	float elapsedFrameTime = 0.0f;
	UPROPERTY(Category = "Testing", BlueprintReadWrite, EditAnywhere)
		bool Paused = false;
	UPROPERTY(Category = "forUI", BlueprintReadWrite)
		float simulationElapsedTime = 0.0f;
	UPROPERTY(Category = "forUI", BlueprintReadWrite, EditAnywhere)
		int bodiesInSimulation = 0;
	UPROPERTY(Category = "forUI", BlueprintReadWrite, EditAnywhere)
		float timeMultiplier = 1.0f;
	double bigG = 39.4784f; //when using SolarMass, AU and Years


	// ----- spawning relevant variables
	UPROPERTY(Category = "SimulationType", EditAnywhere, BlueprintReadWrite)
		bool shouldSpawnSolarSystem = false;
	UPROPERTY(Category = "SimulationType", EditAnywhere, BlueprintReadWrite)
		bool ShouldSpawnTestPlanets = false;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		int bodiesToSpawn = 100;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		int SpawnsPerFrame = 100;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		FVector InitialSpawnCentre = FVector(0, 0, -10);
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		float SpawnLocationBounds = 10;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		float SpawnInitialMaxSpeed = 10;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		float SpawnInitialMaxMass = 2;
	int gradualSpawnerIndex = 0;
	bool spawningBodies = false;

	// ----- testing related variables
	UPROPERTY(Category = "Testing", BlueprintReadWrite, EditAnywhere)
		bool ShouldReset = false;
	UPROPERTY(Category = "Testing", BlueprintReadWrite, EditAnywhere)
		float resetTime = 1.0f;

	UPROPERTY(Category = "Testing", BlueprintReadWrite, EditAnywhere)
		bool showGhosPlanetErrors = false;

	UPROPERTY(Category = "Testing", BlueprintReadWrite, EditAnywhere)
		float averagePosError = 0.0f;
	UPROPERTY(Category = "Testing", BlueprintReadWrite, EditAnywhere)
		bool calcAveragePosError = false;
	UPROPERTY(Category = "Testing", BlueprintReadWrite, EditAnywhere)
		bool doFrameCalc = false;

	bool debugging = true;

	//EXPERIMENTAL
	UPROPERTY(Category = "Testing", BlueprintReadWrite, EditAnywhere)
		bool newTrees = false;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
private:

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	//construction
	void createSimComponents();

	// ----- Time related functions
	UFUNCTION(BlueprintCallable, Category = "SimSpeed")
		void raiseSimulationSpeed();
	UFUNCTION(BlueprintCallable, Category = "SimSpeed")
		void lowerSimulationSpeed();
	UFUNCTION(BlueprintCallable, Category = "SimSpeed")
		void pauseSimulation();


	// ----- planet spawning & destroyingfunctions
	UFUNCTION(BlueprintCallable, Category = "PlanetSpawn")
		void spawnPlanetAt(FVector position_, FVector velocity_, double mass_, FVector4 colour_, FString name_, float radius_, UNBodyHandler* handlerToAddInto);
	UFUNCTION(BlueprintCallable, Category = "PlanetSpawn")
		void spawnSolarSystem(FVector SunPosition_);
	UFUNCTION(BlueprintCallable, Category = "PlanetSpawn")
		void startSpawning(int amount, FVector centre, float extent, float MaxVelocity_, double MaxMass_);
	UFUNCTION(BlueprintCallable, Category = "ClearSim")
		void ClearSimulation();
	void spawnTestPlanets();
	void graduallySpawnBodies(int spawnsPerFrame = 1);
	void deleteDestroyedBodies();
	void deletePlanetInHandler(UGravBodyComponent* ref_, bool deleteLeafRef);

	// ----- real-time testing related functions
	UFUNCTION(BlueprintCallable, Category = "TESTING")
		void addGhostSim();
	UFUNCTION(BlueprintCallable, Category = "TESTING")
		void removeGhostSim();
	void handleAveragePosError();



	
};

