// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UObject/ConstructorHelpers.h"
#include <string>
#include "TreeHandler.h"
#include "AccuracyModule.h"
#include <chrono>
#include "NBodyHandler.generated.h"


UCLASS()
class HONSPROJECT_API UNBodyHandler : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UNBodyHandler();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


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

	UNBodyHandler* ghostSim = nullptr;

	//tree code handler reference
	UPROPERTY(Category = "SimulationRelevant", BlueprintReadWrite)
	UTreeHandler* treeHandlerRef;
	UAccuracyModule* accuracyCompRef;

	//spawning
	void spawnBodyAt(FVector position_, FVector velocity_, double mass_, std::string name_ = "GravBody", float radius_ = 0.0f, FVector4 colour_ = FVector4(0.0f,0.0f,0.0f,0.0f));
	void spawnSolarSystem(int SolarPlanetToSpawn);
	void spawnTestPlanets();
	void graduallySpawnBodies(int spawnsPerFrame = 1);

	void moveBodies(bool alsoMoveActor, double updated_dt);

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


	
	float fixedFrameTime = 0.1f;
	float elapsedFrameTime = 0.0f;
	


	//double bigG = 0.000000000066743f; //when using kg,m and s
	double bigG = 39.4784f; //when using SolarMass, AU and Years
	int gradualSpawnerIndex = 0;
	bool spawningBodies = true;
	


	float timeTakenDI = 0.0f;
	float timeTakenRecalc = 0.0f;
	float timeTakenTC = 0.0f;
	float timeTakenMove = 0.0f;
	int perfITR = 0;

	UPROPERTY(Category = "SimulationRelevant", BlueprintReadWrite)
	bool useTreeCodes_ = false;
	int solarPlanetToSpawn = 100;
	int bodiesToSpawn = 100;
	bool bSpawnTestPlanets = false;
	bool bSpawnSolarSystem = false;
	int SpawnsPerFrame_ = 10.0f;
	float spawnExtent = 10.0f;
	float SpawnMaxSpeed = 5.0f;
	float SpawnMaxMass = 5.0f;
	FVector SpawnCentre = FVector(0.0f,0.0f,0.0f);

	bool shouldAddToGhost = false;

	int handlerID = 0;

	//remove
	bool onlyMove = false;
	bool resetEm = false;
};
