// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UObject/ConstructorHelpers.h"
#include <string>
#include "TreeHandler.h"
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


	
	UFUNCTION(BlueprintCallable, Category = "SimCentre")
	void RecentreSimulation();
	UFUNCTION(BlueprintCallable, Category = "SimCentre")
	void doubleAllScales();


	//array holding a reference to all bodies
	UPROPERTY(Category = "SimulationRelevant", BlueprintReadWrite)
	TArray<AGravBody*> myGravBodies;


	//tree code handler reference
	UPROPERTY(Category = "SimulationRelevant", BlueprintReadWrite)
	UTreeHandler* treeHandlerRef;

	//spawning
	UFUNCTION(BlueprintCallable, Category = "PlanetSpawn")
	void spawnBodyAt(FVector position_, FVector velocity_, double mass_, FVector4 colour_, FString name_ = "GravBody", float radius_ = 0.0f);
	UFUNCTION(BlueprintCallable, Category = "PlanetSpawn")
	void spawnSolarSystem(FVector SunPosition_);
	void spawnTestPlanets();
	void graduallySpawnBodies(int spawnsPerFrame = 1);

	void moveBodies(bool alsoMoveActor, double updated_dt);

	//direct integration of gravitational calculations
	void calculateAllVelocityChanges(double dt);
	//tree codes calculaton
	void calculateWithTree(double dt, bool calculateError = false);

	UPROPERTY(Category = "SimulationRelevant", BlueprintReadWrite)
	float VelCalcAverageError = 0.0f;

	

	UPROPERTY(Category = "forUI", BlueprintReadWrite)
	int gravCalculations = 0;

	UFUNCTION(BlueprintCallable, Category = "PlanetSpawn")
	void startSpawning(int amount, FVector centre, float extent, float MaxVelocity_, double MaxMass_) {
		spawningBodies = true;
		bodiesToSpawn = amount;
		SpawnCentre = centre;
		spawnExtent = extent;
		SpawnMaxSpeed = MaxVelocity_;
		SpawnMaxMass = MaxMass_;
		gradualSpawnerIndex = 0;
	}
	
	//float fixedFrameTime = 0.1f;
	//float elapsedFrameTime = 0.0f;
	


	//double bigG = 0.000000000066743f; //when using kg,m and s
	double bigG = 39.4784f; //when using SolarMass, AU and Years
	int gradualSpawnerIndex = 0;
	bool spawningBodies = true;
	

	UPROPERTY(Category = "SimulationRelevant", BlueprintReadWrite)
	bool useTreeCodes_ = false;
	int bodiesToSpawn = 100;
	bool bSpawnTestPlanets = false;
	bool bSpawnSolarSystem = false;
	int SpawnsPerFrame_ = 100;
	float spawnExtent = 10.0f;
	float SpawnMaxSpeed = 5.0f;
	float SpawnMaxMass = 5.0f;
	FVector SpawnCentre = FVector(0.0f,0.0f,0.0f);

	int handlerID = 0;
};
