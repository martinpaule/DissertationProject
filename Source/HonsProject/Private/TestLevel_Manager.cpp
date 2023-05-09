// Fill out your copyright notice in the Description page of Project Settings.

#include "TestLevel_Manager.h"

#include <Kismet/KismetMathLibrary.h>


// Sets default values
ASimulationManager::ASimulationManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

//clear out the second ghost simulation
void ASimulationManager::removeGhostSim() {
	if (ghostSim_ref) {
		while (!ghostSim_ref->myGravBodies.IsEmpty())
		{
			ghostSim_ref->myGravBodies[0]->GetOwner()->Destroy();
			ghostSim_ref->myGravBodies[0]->DestroyComponent();
			ghostSim_ref->myGravBodies.RemoveAt(0);
		}


	}
}

//begin gradual spawning 
void ASimulationManager::startSpawning(int amount, FVector centre, float extent, float MaxVelocity_, double MaxMass_) {
	spawningBodies = true;
	bodiesToSpawn = amount;
	InitialSpawnCentre = centre;
	SpawnLocationBounds = extent;
	SpawnInitialMaxSpeed = MaxVelocity_;
	SpawnInitialMaxMass = MaxMass_;
	gradualSpawnerIndex = 0;
}

//add a ghost simulation by copying over the current simulation
void ASimulationManager::addGhostSim() {

	if(debugging)GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "Added Ghost Sim");

	FTransform tr;
	tr.SetIdentity();

	//if the ghost sim already exists, ensure that it is clear
	if (ghostSim_ref) {
		while (!ghostSim_ref->myGravBodies.IsEmpty())
		{
			ghostSim_ref->myGravBodies[0]->GetOwner()->Destroy();
			ghostSim_ref->myGravBodies[0]->DestroyComponent();
			ghostSim_ref->myGravBodies.RemoveAt(0);
		}


	}
	else {//if no ghost sim exists, create & assign a new ghost Nbody handler
		ghostSim_ref = Cast<UNBodyHandler>(this->AddComponentByClass(UNBodyHandler::StaticClass(), false, tr, true));
		ghostSim_ref->RegisterComponent();
	}

	
	ghostSim_ref->handlerID = 1;

	//copy over bodies from main Nbody
	for (int i = 0; i < BodyHandler_ref->myGravBodies.Num(); i++)
	{
		
		//create ghost's planet name
		FString name_ = BodyHandler_ref->myGravBodies[i]->GetOwner()->GetActorLabel();
		name_.Append("_ghost");

		//add this planet to the ghost sim handler
		spawnPlanetAt(BodyHandler_ref->myGravBodies[i]->position, BodyHandler_ref->myGravBodies[i]->velocity, BodyHandler_ref->myGravBodies[i]->mass, BodyHandler_ref->myGravBodies[i]->myCol, name_ , BodyHandler_ref->myGravBodies[i]->radius, ghostSim_ref);

		//setup ghost relevant values
		ATestPlanet * asTP = Cast<ATestPlanet>(ghostSim_ref->myGravBodies.Last()->GetOwner());
		asTP->handlerID = 1;
		asTP->myMat->SetScalarParameterValue(TEXT("Opacity"), 0.1f);

		//assign reference to ghost
		Cast<ATestPlanet>(BodyHandler_ref->myGravBodies[i]->GetOwner())->ghostRef = asTP;

	}
}

//initializing function that sets up all the necessary components of the simulation
void ASimulationManager::createSimComponents() {

	FTransform tr;
	tr.SetIdentity();

	//create Nbody handler
	BodyHandler_ref = Cast<UNBodyHandler>(this->AddComponentByClass(UNBodyHandler::StaticClass(), false, tr, true));
	BodyHandler_ref->RegisterComponent();

	//setup Nbody handler
	BodyHandler_ref->useTreeCodes_ = useTreeCodes;

	BodyHandler_ref->constructTreeHandler();

	//create accuracy tester
	accuracyTester_ref = Cast<UAccuracyModule>(this->AddComponentByClass(UAccuracyModule::StaticClass(), false, tr, true));
	accuracyTester_ref->RegisterComponent();
	accuracyTester_ref->resetTime = resetTime;
	accuracyTester_ref->shouldResetTest = ShouldReset;
	accuracyTester_ref->bodyHandlerBodies = &BodyHandler_ref->myGravBodies;
}

// Called when the game starts or when spawned
void ASimulationManager::BeginPlay()
{
	Super::BeginPlay();

	createSimComponents();
	

}

//clearup of to-be-removed bodies
void ASimulationManager::deleteDestroyedBodies() {
	
	for (int i = 0; i < BodyHandler_ref->myGravBodies.Num(); i++)
	{
		UGravBodyComponent* CompIT = BodyHandler_ref->myGravBodies[i];
		if (CompIT->toBeDestroyed) {

			deletePlanetInHandler(CompIT, useTreeCodes);
			BodyHandler_ref->myGravBodies.RemoveAt(i);
			i--;
		}
	}

	if (ghostSim_ref) {
		for (int i = 0; i < ghostSim_ref->myGravBodies.Num(); i++)
		{
			UGravBodyComponent* CompIT_ = ghostSim_ref->myGravBodies[i];
			if (CompIT_->toBeDestroyed) {
				deletePlanetInHandler(CompIT_, false);
				ghostSim_ref->myGravBodies.RemoveAt(i);
				i--;
			}
		}
	}
}


// Called every frame
void ASimulationManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



	if (!Paused && !spawningBodies) {

		elapsedFrameTime += DeltaTime;

		if (elapsedFrameTime >= fixedFrameTime) {

			std::chrono::steady_clock::time_point startPoint;

			deleteDestroyedBodies();


			bodiesInSimulation = BodyHandler_ref->myGravBodies.Num();


			//step 0.5 recalculate partitioning
			if (useTreeCodes) {
				//step 0.5 recalculate partitioning
				BodyHandler_ref->treeHandler->RecalculatePartitioning(newTrees);

			}




			//step 1: Gravitational calculations using fixed time updates

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

				if (useTreeCodes) {
					BodyHandler_ref->calculateWithTree(updatedDT);


					//move testing ghost sim
					if (ghostSim_ref) {
						ghostSim_ref->calculateAllVelocityChanges(updatedDT);
						ghostSim_ref->moveBodies(last, updatedDT);
					}
				}
				else {
					BodyHandler_ref->calculateAllVelocityChanges(updatedDT);

				}
				//step 2: move bodies 
				BodyHandler_ref->moveBodies(last, updatedDT);



			}


			elapsedFrameTime -= int(elapsedFrameTime / fixedFrameTime) * fixedFrameTime;

		}
	}
	else if (spawningBodies) {
		graduallySpawnBodies(SpawnsPerFrame);
	}

	//testing
	if (ghostSim_ref) {
		handleAveragePosError();
	}

	if (BodyHandler_ref->treeHandler->showTreeBoxes) {

		if (!useTreeCodes) {
			BodyHandler_ref->treeHandler->RecalculatePartitioning(false);
		}

		BodyHandler_ref->treeHandler->DisplaySectors(BodyHandler_ref->treeHandler->treeNodeRoot);
	}


	
}

// pause simulation time
void ASimulationManager::pauseSimulation() {
	Paused = !Paused;
}

// raise speed of simulation time
void ASimulationManager::raiseSimulationSpeed()
{
	timeMultiplier *= 2.0f;
	Paused = false;
}

// lower speed of simulation time
void ASimulationManager::lowerSimulationSpeed()
{
	timeMultiplier /= 2.0f;
	Paused = false;
}

// clear simulation
void ASimulationManager::ClearSimulation() {

	simulationElapsedTime = 0.0f;
	elapsedFrameTime = 0.0f;
	
	bodiesInSimulation = 0;
	gradualSpawnerIndex = 0;
	//spawningBodies = true;


	while (!BodyHandler_ref->myGravBodies.IsEmpty())
	{

		BodyHandler_ref->myGravBodies[0]->GetOwner()->Destroy();
		BodyHandler_ref->myGravBodies[0]->DestroyComponent();
		BodyHandler_ref->myGravBodies.RemoveAt(0);
	}

	if (ghostSim_ref) {
		while (!ghostSim_ref->myGravBodies.IsEmpty())
		{
			ghostSim_ref->myGravBodies[0]->GetOwner()->Destroy();
			ghostSim_ref->myGravBodies[0]->DestroyComponent();
			ghostSim_ref->myGravBodies.RemoveAt(0);
		}
	}
}

// testing function
void ASimulationManager::handleAveragePosError(){


	if (!showGhosPlanetErrors && !calcAveragePosError) {
		return;
	}

	averagePosError = 0.0f;



	for (int i = 0; i < BodyHandler_ref->myGravBodies.Num(); i++)
	{

		ATestPlanet* asTP_ = Cast<ATestPlanet>(BodyHandler_ref->myGravBodies[i]->GetOwner());


		if (asTP_->ghostRef) {
			FVector dir = asTP_->ghostRef->GetActorLocation() - GetActorLocation();
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
				DrawDebugLine(GetWorld(), asTP_->GetActorLocation(), asTP_->ghostRef->GetActorLocation(), lineCol, false, 0.0f, 0, 13.0f);
			}
			if (calcAveragePosError) {
				averagePosError += (asTP_->ghostRef->GravComp->position - BodyHandler_ref->myGravBodies[i]->position).Length();
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

// setup function for spawning bodies - creates a new body with specified parameters
void ASimulationManager::spawnPlanetAt(FVector position_, FVector velocity_, double mass_, FVector4 colour_, FString name_, float radius_, UNBodyHandler* handlerToAddInto)
{

	FActorSpawnParameters SpawnInfo;
	FRotator myRot(0, 0, 0);

	//assign body's variables
	ATestPlanet* newBody = GetWorld()->SpawnActor<ATestPlanet>(position_ * 1000.0f, myRot, SpawnInfo);
	//newBody->SetActorEnableCollision(true);


	
	//create Nbody handler
	newBody->GravComp = handlerToAddInto->addGravCompAt(position_, velocity_, mass_, newBody);

	newBody->SetActorLabel(name_);


	if (radius_ == 0.0f) {
		radius_ = cbrt(mass_);
	}

	//currently more for display purposes
	newBody->SetActorScale3D(FVector(radius_, radius_, radius_));

	//option to set colour too
	newBody->myMat->SetVectorParameterValue(TEXT("Colour"), colour_);
	newBody->GravComp->myCol = colour_;




}

//spawn solar system given a desired position
void ASimulationManager::spawnSolarSystem(FVector SunPosition_) {


	//define and spawn the sun
	FVector bodyONE_pos = FVector(-9.0841f * pow(10, -3), 4.9241f * pow(10, -4), 2.0754f * pow(10, -4));
	float massOne = 1.0f;
	spawnPlanetAt(bodyONE_pos + SunPosition_ / 1000.0f, FVector(0.0f, 0.0f, 0.0f), 1.0f, FVector4(1.0f, 1.0f, 0.0f, 1.0f), "Sun", 5.0f,BodyHandler_ref);


	FString bodyTwoName_;
	float massTwo = 0.0f;
	float bodyTwoScale = 0.0f;
	FVector bodyTWO_pos;
	FVector4 planetColor;


	//define which body (or if all) to spawn
	int it_begin = 0;
	int it_end = 7;


	//define values
	for (it_begin; it_begin <= it_end; it_begin++) {
		//which planet to simulate orbiting around the sun
		switch (it_begin) {

		case 0: //
			bodyTwoName_ = "Jupiter";
			massTwo = 0.9545f * pow(10, -3);
			bodyTwoScale = 2.2f;
			bodyTWO_pos = FVector(4.8917f, 7.0304f * pow(10, -1), -1.1236f * pow(10, -1));
			planetColor = FVector4(0.8f, 0.8f, 0.7f, 1.0f);
			break;
		case 1: //
			bodyTwoName_ = "Saturn";
			massTwo = 2.859 * pow(10, -4);
			bodyTwoScale = 1.9f;
			bodyTWO_pos = FVector(8.0121f, -5.7062f, -2.1978f * pow(10, -1));
			planetColor = FVector4(0.82f, 0.72f, 0.55f, 1.0f);
			break;
		case 2: //
			bodyTwoName_ = "Neptune";
			massTwo = 5.15f * pow(10, -5);
			bodyTwoScale = 1.4f;
			bodyTWO_pos = FVector(2.9739f * pow(10, 1), -3.081f, -6.2191f * pow(10, -1));
			planetColor = FVector4(0.4f, 0.4f, 1.0f, 1.0f);
			break;
		case 3: //
			bodyTwoName_ = "Uranus";
			massTwo = 4.364f * pow(10, -5);
			bodyTwoScale = 1.5f;
			bodyTWO_pos = FVector(1.3488f * pow(10, 1), 1.4317f * pow(10, 1), -1.2157f * pow(10, -1));
			planetColor = FVector4(0.6f, 0.6f, 0.95f, 1.0f);
			break;
		case 4: //
			bodyTwoName_ = "Earth";
			massTwo = 3.003f * pow(10, -6);
			bodyTwoScale = 1.1f;
			bodyTWO_pos = FVector(5.5374f * pow(10, -1), 8.1332f * pow(10, -1), 1.5998f * pow(10, -4));
			planetColor = FVector4(0.1f, 0.1f, 0.95f, 1.0f);
			break;
		case 5: //
			bodyTwoName_ = "Venus";
			massTwo = 2.447f * pow(10, -6);
			bodyTwoScale = 0.9f;
			bodyTWO_pos = FVector(-2.4692f * pow(10, -1), -6.8513f * pow(10, -1), 4.5184f * pow(10, -3));
			planetColor = FVector4(0.95f, 0.2f, 0.95f, 1.0f);
			break;
		case 6: //
			bodyTwoName_ = "Mars";
			massTwo = 3.226f * pow(10, -7);
			bodyTwoScale = 0.6f;
			bodyTWO_pos = FVector(6.2360f * pow(10, -1), 1.3693f, 1.3376f * pow(10, -2));
			planetColor = FVector4(0.8f, 0.3f, 0.05f, 1.0f);
			break;
		case 7:
			bodyTwoName_ = "Mercury";
			massTwo = 1.66f * pow(10, -7);
			bodyTwoScale = 0.5f;
			bodyTWO_pos = FVector(-1.514f * pow(10, -1), -4.4286f * pow(10, -1), -2.2969f * pow(10, -2));
			planetColor = FVector4(0.95f, 0.6f, 0.0f, 1.0f);
			break;
		}

		//calculate the values into a tangential velocity.
		//v^2 = m1 * G / distance

		FVector dir = bodyONE_pos - bodyTWO_pos;
		float distance = dir.Length();

		float YVel = bigG * massOne;
		YVel /= distance;
		YVel = sqrt(YVel);

		dir.Normalize();
		FVector RightVel = UKismetMathLibrary::GetRightVector(dir.Rotation()) * YVel;
		spawnPlanetAt(bodyTWO_pos + SunPosition_ / 1000.0f, RightVel, massTwo, planetColor, bodyTwoName_, bodyTwoScale,BodyHandler_ref);
	}

	if (useTreeCodes) {
		BodyHandler_ref->treeHandler->RecalculatePartitioning(false);
	}

}

//spawn a pre-determined cluster of test planets
void ASimulationManager::spawnTestPlanets()
{
	//20,20,2
	FVector positions[15] = { FVector(20.0f, 0.0f, 0.0f),FVector(0.0f, 5.0f,20.0f), FVector(-5.0f,20.0f,10.0f), FVector(17.0f,-10.0f,4.0f), FVector(5.0f,0.0f, 5.0f), FVector(3.0f, -17.0f, 6.0f),FVector(11.0f,11.0f,11.0f), FVector(-6.0f,12.0f,-18.0f), FVector(-17.0f,1.0f,7.0f), FVector(8.0f,-16.0f,16.0f), FVector(9.0f,3.0f,20.0f),FVector(5.0f,-10.0f,15.0f), FVector(11.0f, -19.0f,7.0f), FVector(-7.0f,17.0f,19.0f), FVector(4.0f,-10.0f,16.0f) };
	FVector directions[15] = { FVector(-5.0f,20.0f,10.0f),FVector(5.0f,-10.0f,15.0f), FVector(11.0f, -19.0f,7.0f), FVector(-7.0f,17.0f,19.0f),  FVector(-17.0f,1.0f,7.0f), FVector(8.0f,-16.0f,16.0f),FVector(17.0f,-10.0f,4.0f), FVector(5.0f,0.0f, 5.0f), FVector(20.0f, 0.0f, 0.0f),FVector(0.0f, 5.0f,20.0f), FVector(3.0f, -17.0f, 6.0f),FVector(11.0f,11.0f,11.0f), FVector(-6.0f,12.0f,-18.0f), FVector(9.0f,3.0f,20.0f), FVector(4.0f,-10.0f,16.0f) };
	float masses[15] = { 1.0f,0.3f, 0.4f, 0.5f, 0.6f, 0.2f,0.3f, 1.2f, 1.4f, 2.0f, 1.8f,0.2f, 0.5f, 0.7f, 0.4f };

	for (int i = 0; i < 15; i++) {

		FString bodName = "Body ";
		bodName.Append(std::to_string(i).c_str());
		spawnPlanetAt(positions[i] * 0.8f, directions[i] * 0.5f, masses[i] * 2.0f, FVector4(1.0f, 0.0f, 1.0f, 1.0f), bodName,0,BodyHandler_ref);
	}

}

//function that allows gradual spawn of initial bodies rather than all at once, avoiding a big lag spike when handlingodies
void ASimulationManager::graduallySpawnBodies(int spawnsPerFrame) {


	for (int s = 0; s < spawnsPerFrame; s++) { // spawn all the bodies for this frame

		//Exi the spawning loop
		if (gradualSpawnerIndex >= bodiesToSpawn) {
			
			spawningBodies = false;
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, "Spawned all desired bodies");


			return;
		}

		//random location
		FVector myLoc(-SpawnLocationBounds / 2, -SpawnLocationBounds / 2, -SpawnLocationBounds / 2);
		myLoc.X += FMath::FRandRange(0, SpawnLocationBounds);
		myLoc.Y += FMath::FRandRange(0, SpawnLocationBounds);
		myLoc.Z += FMath::FRandRange(0, SpawnLocationBounds);
		myLoc += InitialSpawnCentre; //translate it to desired spawn centre

		//random speed
		FVector speed_ = FVector(-SpawnInitialMaxSpeed / 2, -SpawnInitialMaxSpeed / 2, -SpawnInitialMaxSpeed / 2);
		speed_.X += FMath::FRandRange(0, SpawnInitialMaxSpeed);
		speed_.Y += FMath::FRandRange(0, SpawnInitialMaxSpeed);
		speed_.Z += FMath::FRandRange(0, SpawnInitialMaxSpeed);

		//random mass
		float mass_ = 0.001f;
		mass_ += FMath::FRandRange(0.0f, SpawnInitialMaxMass);

		FString bodName = "Body ";
		bodName.Append(std::to_string(gradualSpawnerIndex).c_str());

		FVector4 randomColour = FVector4(0.0f, 0.0f, 0.0f, 1.0f);
		randomColour.X += FMath::FRandRange(0.0f, 1.0f);
		randomColour.Y += FMath::FRandRange(0.0f, 1.0f);
		randomColour.Z += FMath::FRandRange(0.0f, 1.0f);

		spawnPlanetAt(myLoc, speed_, mass_, randomColour, bodName, 0 ,BodyHandler_ref);
		gradualSpawnerIndex++;

	}

	return;
}

//planet deleting function
void ASimulationManager::deletePlanetInHandler(UGravBodyComponent* ref_, bool deleteLeafRef) {

	ATestPlanet* asTP = Cast<ATestPlanet>(ref_->GetOwner());


	if (asTP->ghostRef) {
		asTP->ghostRef->ghostRef = NULL;
	}

	if (deleteLeafRef) {
		ref_->leaf_ref->bodies.Empty();
	}

	ref_->GetOwner()->Destroy();
	ref_->DestroyComponent();

}
