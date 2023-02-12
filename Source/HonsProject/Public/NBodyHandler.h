// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "UObject/ConstructorHelpers.h"
#include <string>
#include "TreeHandler.h"
#include "AccuracyModule.h"
#include <chrono>
#include "NBodyHandler.generated.h"


UCLASS()
class HONSPROJECT_API ANBodyHandler : public APawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANBodyHandler();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	//UI called functions
	UFUNCTION(BlueprintCallable, Category = "SimSpeed")
	void raiseSimulationSpeed();
	UFUNCTION(BlueprintCallable, Category = "SimSpeed")
	void lowerSimulationSpeed();
	UFUNCTION(BlueprintCallable, Category = "SimSpeed")
	void pauseSimulation();
	UFUNCTION(BlueprintCallable, Category = "SimCentre")
	void RecentreSimulation();
	UFUNCTION(BlueprintCallable, Category = "SimCentre")
	void doubleAllScales();
	UFUNCTION(BlueprintCallable, Category = "ClearSim")
	void ClearSimulation();

	//array holding a reference to all bodies
	UPROPERTY(Category = "SimulationRelevant", BlueprintReadWrite)
	TArray<AGravBody*> myGravBodies;

	//tree code handler reference
	UPROPERTY(Category = "SimulationRelevant", BlueprintReadWrite)
	ATreeHandler* treeHandlerRef;
	AAccuracyModule* accuracyCompRef;



	//spawning
	void spawnBodyAt(FVector position_, FVector velocity_, double mass_, std::string name_ = "GravBody", float radius_ = 0.0f, FVector4 colour_ = FVector4(0.0f,0.0f,0.0f,0.0f));
	void spawnSolarSystem();
	void spawnTestPlanets();
	void graduallySpawnBodies(int spawnsPerFrame = 1);

	//direct integration of gravitational calculations
	void calculateAllVelocityChanges(double dt);
	//tree codes calculaton
	void calculateWithTree(double dt);

	//record current posisition, masses and directions to a txt file
	void recordFinalPositions();


	//general things
	UPROPERTY(Category = "SimulationRelevant", BlueprintReadWrite, EditAnywhere)
	float timeMultiplier = 1.0f;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
	bool notPaused = true;

	//UI variables
	UPROPERTY(Category = "forUI", BlueprintReadWrite)
	float simulationElapsedTime = 0.0f;
	UPROPERTY(Category = "forUI", BlueprintReadWrite)
	int bodiesInSimulation = 0;
	UPROPERTY(Category = "forUI", BlueprintReadWrite)
	int gravCalculations = 0;

	//testing
	UPROPERTY(Category = "Testing", BlueprintReadWrite, EditAnywhere)
	bool ShouldReset = false;
	UPROPERTY(Category = "Testing", BlueprintReadWrite, EditAnywhere)
	float resetTime = 1.0f;
	UPROPERTY(Category = "Testing", BlueprintReadWrite, EditAnywhere)
	float fixedFrameTime = 0.1f;
	float elapsedFrameTime = 0.0f;

	//simulation dependant variables
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
	bool useTreeCodes = false;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		bool shouldSpawnSolarSystem = false;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		bool ShouldSpawnTestPlanets = false;

	
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
	int bodiesToSpawn = 100;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
	int SpawnsPerFrame = 50;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
	FVector InitialSpawnCentre = FVector(0,0,-10);
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
	float SpawnLocationBounds = 10;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
	float SpawnInitialMaxSpeed = 10;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
	float SpawnInitialMaxMass = 2;

	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
	int SolarPlanetToSpawn = 100;//0-7 to spawn a specific planet+ sun,anything else for all of them


	//double bigG = 0.000000000066743f; //when using kg,m and s
	double bigG = 39.4784f; //when using SolarMass, AU and Years
	int gradualSpawnerIndex = 0;
	bool spawningBodies = true;
	


	float timeTakenDI = 0.0f;
	float timeTakenRecalc = 0.0f;
	float timeTakenTC = 0.0f;
	float timeTakenMove = 0.0f;
	int perfITR = 0;
};
