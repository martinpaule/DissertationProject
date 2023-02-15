// Fill out your copyright notice in the Description page of Project Settings.


#include "SimulationManager.h"

// Sets default values
ASimulationManager::ASimulationManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASimulationManager::BeginPlay()
{
	Super::BeginPlay();


	//spawn tree code handler
	FActorSpawnParameters SpawnInfo;
	BodyHandler = GetWorld()->SpawnActor<ANBodyHandler>(SpawnInfo);

	BodyHandler->solarPlanetToSpawn = SolarPlanetToSpawn;
	BodyHandler->SpawnsPerFrame_ = SpawnsPerFrame;
	BodyHandler->bodiesToSpawn = bodiesToSpawn;
	BodyHandler->bSpawnTestPlanets = ShouldSpawnTestPlanets;
	BodyHandler->bSpawnSolarSystem = shouldSpawnSolarSystem;
	BodyHandler->useTreeCodes_ = useTreeCodes;
	BodyHandler->spawnExtent = SpawnLocationBounds;
	BodyHandler->SpawnMaxSpeed = SpawnInitialMaxSpeed;
	BodyHandler->SpawnMaxMass = SpawnInitialMaxMass;
	BodyHandler->SpawnCentre = InitialSpawnCentre;
	BodyHandler->fixedFrameTime = fixedFrameTime;


	FActorSpawnParameters SpawnInfoAH;
	BodyHandler->accuracyCompRef = GetWorld()->SpawnActor<AAccuracyModule>(SpawnInfoAH);
	BodyHandler->accuracyCompRef->resetTime = resetTime;
	BodyHandler->accuracyCompRef->shouldResetTest = ShouldReset;

	if (shouldGhostAccuracy && useTreeCodes) {
		BodyHandler->shouldAddToGhost = true;

		FActorSpawnParameters SpawnInfoT;
		BodyHandler->ghostSim = GetWorld()->SpawnActor<ANBodyHandler>(SpawnInfoT);
		BodyHandler->ghostSim->fixedFrameTime = fixedFrameTime;
		//ghostSim->shouldAddToGhost = true;
		BodyHandler->ghostSim->bodiesToSpawn = 0;
		BodyHandler->ghostSim->handlerID = 1;

	}

}

// Called every frame
void ASimulationManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

