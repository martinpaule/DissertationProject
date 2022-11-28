// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GravBody.h"
#include "UObject/ConstructorHelpers.h"
#include <string>
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


	//UI functions
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
	UFUNCTION(BlueprintCallable, Category = "ClearSim")
	void flipPlanetNames();

	//array holding a reference to all bodies
	TArray<AGravBody*> myGravBodies;


	//spawning
	void spawnBodyAt(FVector position_, FVector velocity_, double mass_, std::string name_ = "GravBody", float radius_ = 0.0f);
	void graduallySpawnBodies(int spawnsPerFrame = 1);

	//direct integration of gravitational calculations
	void calculateAllVelocityChanges(double dt);

	void spawnSolarSystem();

	//UI variables
	UPROPERTY(Category = "SimulationRelevant", BlueprintReadWrite)
	float timeMultiplier = 1.0f;
	UPROPERTY(Category = "SimulationRelevant", BlueprintReadWrite)
	float SimulationElapsedTime = 0.0f;
	UPROPERTY(Category = "SimulationRelevant", BlueprintReadWrite)
	int BodiesInSimulation = 0;

	//simulation dependant variables
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
	int bodiesToSpawn = 20;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
	int SpawnsPerFrame = 5;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
	FVector InitialSpawnCentre = FVector(0,0,0);
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
	int SpawnLocationBounds = 50000;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
	int SpawnInitialMaxSpeed = 200;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
	float SpawnInitialMaxMass = 300;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
	bool shouldSpawnSolarSystem = false;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
	bool showPlanetNames = false;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
	int SolarPlanetToSpawn = 0;//0-7 to spawn a specific planet,- 1 for all



	//long double bigG = 0.000000000066743f;
	double bigG = 39.4784f; //when using AU and Years
	bool notPaused = false;
	int gradualSpawnerIndex = 7;
	bool spawningBodies = true;
	
};
