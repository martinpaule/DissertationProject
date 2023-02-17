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


	FTransform tr;
	tr.SetIdentity();

	//spawn tree code handler
	BodyHandler = Cast<UNBodyHandler>(this->AddComponentByClass(UNBodyHandler::StaticClass(), false, tr, true));
	BodyHandler->RegisterComponent();

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


	

	BodyHandler->accuracyCompRef = Cast<UAccuracyModule>(this->AddComponentByClass(UAccuracyModule::StaticClass(), false, tr, true));
	BodyHandler->accuracyCompRef->RegisterComponent();

	BodyHandler->accuracyCompRef->resetTime = resetTime;
	BodyHandler->accuracyCompRef->shouldResetTest = ShouldReset;


	BodyHandler->onlyMove = testOnlyMove;

	if (useTreeCodes) {


		//spawn tree code handler
		UTreeHandler * treeRef = Cast<UTreeHandler>(this->AddComponentByClass(UTreeHandler::StaticClass(), false, tr, true));
		treeRef->RegisterComponent();
		treeRef->bodyHandlerBodies = &BodyHandler->myGravBodies;

		BodyHandler->treeHandlerRef = treeRef;

		if (shouldGhostAccuracy) {
			BodyHandler->shouldAddToGhost = true;

			BodyHandler->ghostSim = Cast<UNBodyHandler>(this->AddComponentByClass(UNBodyHandler::StaticClass(), false, tr, true));
			BodyHandler->ghostSim->RegisterComponent();

			BodyHandler->ghostSim->fixedFrameTime = fixedFrameTime;
			//ghostSim->shouldAddToGhost = true;
			BodyHandler->ghostSim->bodiesToSpawn = 0;
			BodyHandler->ghostSim->handlerID = 1;
		}
		

	}

}

// Called every frame
void ASimulationManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

