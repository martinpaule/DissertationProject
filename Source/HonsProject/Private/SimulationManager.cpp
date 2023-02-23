// Fill out your copyright notice in the Description page of Project Settings.


#include "SimulationManager.h"

// Sets default values
ASimulationManager::ASimulationManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}



void ASimulationManager::recordFinalPositions() {

	TArray<planet> a;
	accuracyTester_ref->planets.Add(a);

	for (int i = 0; i < BodyHandler_ref->myGravBodies.Num(); i++) {
		std::string name_ = std::string(TCHAR_TO_UTF8(*BodyHandler_ref->myGravBodies[i]->GetActorLabel()));
		accuracyTester_ref->notePlanet(name_, BodyHandler_ref->myGravBodies[i]->position, BodyHandler_ref->myGravBodies[i]->velocity, BodyHandler_ref->myGravBodies[i]->mass);
	}

	accuracyTester_ref->printResultToTXT();
}


void ASimulationManager::removeGhostSim() {
	if (ghostSim_ref) {
		while (!ghostSim_ref->myGravBodies.IsEmpty())
		{
			ghostSim_ref->myGravBodies[0]->Destroy();
			ghostSim_ref->myGravBodies.RemoveAt(0);
		}


	}
}

void ASimulationManager::addGhostSim() {

	FTransform tr;
	tr.SetIdentity();

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "Added Ghost Sim");


	if (ghostSim_ref) {
		while (!ghostSim_ref->myGravBodies.IsEmpty())
		{
			ghostSim_ref->myGravBodies[0]->Destroy();
			ghostSim_ref->myGravBodies.RemoveAt(0);
		}


	}
	else {
		//create & assign ghost Nbody handler
		ghostSim_ref = Cast<UNBodyHandler>(this->AddComponentByClass(UNBodyHandler::StaticClass(), false, tr, true));
		ghostSim_ref->RegisterComponent();
	}

	
	ghostSim_ref->bodiesToSpawn = 0;
	ghostSim_ref->handlerID = 1;
	ghostSim_ref->spawningBodies = false;

	//copy over bodies from main Nbody
	for (int i = 0; i < BodyHandler_ref->myGravBodies.Num(); i++)
	{
		BodyHandler_ref->myGravBodies[i]->SetActorEnableCollision(false);
		
		FString name_ = BodyHandler_ref->myGravBodies[i]->GetActorLabel();
		name_.Append("_ghost");

		ghostSim_ref->spawnBodyAt(BodyHandler_ref->myGravBodies[i]->position, BodyHandler_ref->myGravBodies[i]->velocity, BodyHandler_ref->myGravBodies[i]->mass, BodyHandler_ref->myGravBodies[i]->myCol, name_ , BodyHandler_ref->myGravBodies[i]->radius);

		ghostSim_ref->myGravBodies.Last()->handlerID = 1;
		BodyHandler_ref->myGravBodies[i]->ghostRef = ghostSim_ref->myGravBodies.Last();


		ghostSim_ref->myGravBodies.Last()->myMat->SetScalarParameterValue(TEXT("Opacity"), 0.1f);
		BodyHandler_ref->myGravBodies[i]->SetActorEnableCollision(true);


	}
}


void ASimulationManager::createSimComponents() {
	FTransform tr;
	tr.SetIdentity();

	//create Nbody handler
	BodyHandler_ref = Cast<UNBodyHandler>(this->AddComponentByClass(UNBodyHandler::StaticClass(), false, tr, true));
	BodyHandler_ref->RegisterComponent();

	//setup Nbody handler
	BodyHandler_ref->SpawnsPerFrame_ = SpawnsPerFrame;
	BodyHandler_ref->bodiesToSpawn = bodiesToSpawn;
	BodyHandler_ref->bSpawnTestPlanets = ShouldSpawnTestPlanets;
	BodyHandler_ref->bSpawnSolarSystem = shouldSpawnSolarSystem;
	BodyHandler_ref->useTreeCodes_ = useTreeCodes;
	BodyHandler_ref->spawnExtent = SpawnLocationBounds;
	BodyHandler_ref->SpawnMaxSpeed = SpawnInitialMaxSpeed;
	BodyHandler_ref->SpawnMaxMass = SpawnInitialMaxMass;
	BodyHandler_ref->SpawnCentre = InitialSpawnCentre;



	//create accuracy tester
	accuracyTester_ref = Cast<UAccuracyModule>(this->AddComponentByClass(UAccuracyModule::StaticClass(), false, tr, true));
	accuracyTester_ref->RegisterComponent();
	accuracyTester_ref->resetTime = resetTime;
	accuracyTester_ref->shouldResetTest = ShouldReset;

	//create tree code handler
	TreeHandler_ref = Cast<UTreeHandler>(this->AddComponentByClass(UTreeHandler::StaticClass(), false, tr, true));
	TreeHandler_ref->RegisterComponent();
	TreeHandler_ref->bodyHandlerBodies = &BodyHandler_ref->myGravBodies;

	//assign tree code handler
	BodyHandler_ref->treeHandlerRef = TreeHandler_ref;
}

// Called when the game starts or when spawned
void ASimulationManager::BeginPlay()
{
	Super::BeginPlay();

	createSimComponents();
	

}

// Called every frame
void ASimulationManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	

	if (!Paused && !BodyHandler_ref->spawningBodies) {

		elapsedFrameTime += DeltaTime;

		if (elapsedFrameTime >= fixedFrameTime) {


			//step 0: destroy overlapping bodies from previous step - must be done before force calculation otherwise the current step will be inaccurate
			for (int i = 0; i < BodyHandler_ref->myGravBodies.Num(); i++)
			{
				if (BodyHandler_ref->myGravBodies[i]->toBeDestroyed) {

					if (BodyHandler_ref->myGravBodies[i]->ghostRef) {
						BodyHandler_ref->myGravBodies[i]->ghostRef->ghostRef = NULL;
					}
					
					BodyHandler_ref->myGravBodies[i]->Destroy();
					BodyHandler_ref->myGravBodies.RemoveAt(i);
					i--;
				}
			}
			if (ghostSim_ref) {
				for (int i = 0; i < ghostSim_ref->myGravBodies.Num(); i++)
				{
					if (ghostSim_ref->myGravBodies[i]->toBeDestroyed) {
						if (ghostSim_ref->myGravBodies[i]->ghostRef) {
							ghostSim_ref->myGravBodies[i]->ghostRef->ghostRef = NULL;
						}
						ghostSim_ref->myGravBodies[i]->Destroy();
						ghostSim_ref->myGravBodies.RemoveAt(i);
						i--;
					}
				}
			}
			
			bodiesInSimulation = BodyHandler_ref->myGravBodies.Num();


			//dt influenced by simulation time scale 
			double updatedDT = fixedFrameTime * timeMultiplier * 0.027f; //0.027 makes the time as 10 days/s		

			int iterations = int(elapsedFrameTime / fixedFrameTime);

			simulationElapsedTime += updatedDT * iterations;

			for (int j = 0; j < iterations; j++) {

				//check whether to move bodies on this it too
				bool last = false;
				if (j == iterations - 1) {
					last = true;
				}

				//step 1: Gravitational calculations using desired method
				if (!useTreeCodes) {
					BodyHandler_ref->calculateAllVelocityChanges(updatedDT);
					
				}
				else {
					//time taken code
					//auto startDI = std::chrono::high_resolution_clock::now();
					//auto stopDI = std::chrono::high_resolution_clock::now();
					//float msTakenCALCTC = std::chrono::duration_cast<std::chrono::microseconds>(stopDI - startDI).count();

					BodyHandler_ref->calculateWithTree(updatedDT,doFrameCalc);
					if (ghostSim_ref) {
						ghostSim_ref->calculateAllVelocityChanges(updatedDT);
						ghostSim_ref->moveBodies(last, updatedDT);
					}

				}
				//step 2: move bodies using their updated velocity, also destroy ones that 
				BodyHandler_ref->moveBodies(last, updatedDT);


				

			}


			elapsedFrameTime -= int(elapsedFrameTime / fixedFrameTime) * fixedFrameTime;

		}
	}

	if (ghostSim_ref) {
		handleAveragePosError();
	}

	if (TreeHandler_ref->showTreeBoxes) {

		if (!useTreeCodes) {
			TreeHandler_ref->RecalculatePartitioning();
		}

		TreeHandler_ref->DisplaySectors(TreeHandler_ref->treeNodeRoot);
	}
}



void ASimulationManager::pauseSimulation() {
	Paused = !Paused;
}

void ASimulationManager::raiseSimulationSpeed()
{
	timeMultiplier *= 2.0f;
	Paused = false;
}


void ASimulationManager::lowerSimulationSpeed()
{
	timeMultiplier /= 2.0f;
	Paused = false;
}

void ASimulationManager::ClearSimulation() {
	//timeMultiplier = 1.0f;
	//notPaused = false;
	simulationElapsedTime = 0.0f;
	elapsedFrameTime = 0.0f;
	
	bodiesInSimulation = 0;
	BodyHandler_ref->gradualSpawnerIndex = 0;
	BodyHandler_ref->spawningBodies = true;


	while (!BodyHandler_ref->myGravBodies.IsEmpty())
	{
		BodyHandler_ref->myGravBodies[0]->Destroy();
		BodyHandler_ref->myGravBodies.RemoveAt(0);
	}

	if (ghostSim_ref) {
		while (!ghostSim_ref->myGravBodies.IsEmpty())
		{
			ghostSim_ref->myGravBodies[0]->Destroy();
			ghostSim_ref->myGravBodies.RemoveAt(0);
		}
	}
}


void ASimulationManager::handleAveragePosError(){


	if (!showGhosPlanetErrors && !calcAveragePosError) {
		return;
	}

	averagePosError = 0.0f;



	for (int i = 0; i < BodyHandler_ref->myGravBodies.Num(); i++)
	{
		if (BodyHandler_ref->myGravBodies[i]->ghostRef) {
			FVector dir = BodyHandler_ref->myGravBodies[i]->ghostRef->GetActorLocation() - GetActorLocation();
			float dist = dir.Length();
			FColor lineCol;

			float badDist = 10000;

			if (dist > badDist) {
				lineCol = FColor::Red;
			}
			else {
				lineCol.R = (dist / badDist) * 255;
				lineCol.G = 255 - (dist / badDist) * 255;
				lineCol.B = 0;
			}
			if (showGhosPlanetErrors) {
				DrawDebugLine(GetWorld(), BodyHandler_ref->myGravBodies[i]->GetActorLocation(), BodyHandler_ref->myGravBodies[i]->ghostRef->GetActorLocation(), lineCol, false, 0.0f, 0, 13.0f);
			}
			if (calcAveragePosError) {
				averagePosError += (BodyHandler_ref->myGravBodies[i]->ghostRef->position - BodyHandler_ref->myGravBodies[i]->position).Length();
			}
		}
		else {
			if (showGhosPlanetErrors) {
				DrawDebugBox(GetWorld(), BodyHandler_ref->myGravBodies[i]->position * 1000.0f, FVector(100, 100, 100) * GetActorScale3D().X, FColor::Red, false, 0.0f, 0U, 13.0f);
			}
			if (calcAveragePosError) {
				averagePosError += 10000;
			}
		}
	}
	averagePosError /= BodyHandler_ref->myGravBodies.Num();
}
