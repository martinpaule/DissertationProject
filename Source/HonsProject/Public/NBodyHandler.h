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


	//array holding a reference to all bodies
	TArray<AGravBody*> myGravBodies;

	//spawning
	void spawnBodyAt(FVector position_, FVector velocity_, float mass_, float radius_ = 0.0f, char * name_ = "GravBody");
	void graduallySpawnBodies(int spawnsPerFrame = 1);

	//direct integration of gravitational calculations
	void calculateAllVelocityChanges(float dt);

	void spawnSolarSystem();

	//UI variables
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
	float timeMultiplier = 1.0f;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
	float SimulationElapsedTime = 0.0f;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
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

	long double bigG = 0.000000000066743f;
	bool notPaused = true;
	int gradualSpawnerIndex = 0;
	bool spawningBodies = true;
	
};
