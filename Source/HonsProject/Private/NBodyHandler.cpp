// Fill out your copyright notice in the Description page of Project Settings.


#include "NBodyHandler.h"
#include <Kismet/GameplayStatics.h>
#include "Misc/DateTime.h"
#include "GameFramework/DefaultPawn.h"
#include <Kismet/KismetMathLibrary.h>
#include <fstream>
#include "DrawDebugHelpers.h"

// Sets default values
ANBodyHandler::ANBodyHandler()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
}


void ANBodyHandler::recordFinalPositions() {

	TArray<planet> a;
	accuracyCompRef->planets.Add(a);

	for (int i = 0; i < myGravBodies.Num(); i++) {
		std::string name_ = std::string(TCHAR_TO_UTF8(*myGravBodies[i]->GetActorLabel()));
		accuracyCompRef->notePlanet(name_, myGravBodies[i]->position, myGravBodies[i]->velocity, myGravBodies[i]->mass);
	}

	if (accuracyCompRef->planets.Num() >= 5) {
		accuracyCompRef->printResultToTXT();
	}
}

// Called when the game starts or when spawned
void ANBodyHandler::BeginPlay()
{
	Super::BeginPlay();

	


	//example how to bind functions to input in code, keep to avoid looking this up again
	//ADefaultPawn * pawn_ = Cast<ADefaultPawn>(UGameplayStatics::GetActorOfClass(GetWorld(), ADefaultPawn::StaticClass()));
	//UInputComponent * inp_ = pawn_->InputComponent;
	//if (inp_)
	//{
	//	// Bind an action to it
	//	inp_->BindAxis
	//	(
	//		"MoveToSimMid", // The input identifier (specified in DefaultInput.ini)
	//		this, // The object instance that is going to react to the input
	//		&ANBodyHandler::moveToSimulationCore // The function that will fire when input is received
	//	);
	//	EnableInput(GetWorld()->GetFirstPlayerController());
	//}


	//add manually placed bodies to the array
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGravBody::StaticClass(), FoundActors);	
	for (int n = 0; n < FoundActors.Num(); n++) {
		AGravBody* ref = Cast<AGravBody>(FoundActors[n]);
		myGravBodies.Add(ref);
	}

	//spawn tree code handler
	FActorSpawnParameters SpawnInfo;
	treeHandlerRef = GetWorld()->SpawnActor<ATreeHandler>(SpawnInfo);
	treeHandlerRef->bodyHandlerBodies = &myGravBodies;
	treeHandlerRef->shouldCalculateTC = useTreeCodes;

	FActorSpawnParameters SpawnInfoAH;
	accuracyCompRef = GetWorld()->SpawnActor<AAccuracyModule>(SpawnInfoAH);
}

//direct integration of gravitational dynamics using Newtonian formulae
void ANBodyHandler::calculateAllVelocityChanges(double dt) {

	gravCalculations = 0;

	//declarations
	double distance = 0.0f;
	double distanceCubed = 0.0f;
	FVector direction = FVector(0.0f, 0.0f, 0.0f);
	FVector iteratedBodyForce = FVector(0.0f, 0.0f, 0.0f);
	FVector sumOfForces = FVector(0.0f, 0.0f, 0.0f);
	FVector deltaVelocity = FVector(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < myGravBodies.Num(); i++)
	{
		sumOfForces = FVector(0.0f, 0.0f, 0.0f);

		//calulate combined forces acting on body I
		for (int j = 0; j < myGravBodies.Num(); j++)
		{
			if (i != j) //ignore the body's own force on itself
			{


				direction = myGravBodies[j]->position - myGravBodies[i]->position;
				distance = direction.Length();
				distanceCubed = distance * distance * distance;
				iteratedBodyForce = direction * bigG * myGravBodies[j]->mass;
				iteratedBodyForce /= distanceCubed;

				//add this to the sum of forces acting on body I
				sumOfForces += iteratedBodyForce;
				gravCalculations++;
			}
		}

		//apply change in velocity to body I
		deltaVelocity = dt* sumOfForces;
		myGravBodies[i]->velocity += deltaVelocity;

	}
}


//tree code calculations of gravitational dynamics
void ANBodyHandler::calculateWithTree(double dt) {


	auto RecalcStart = std::chrono::high_resolution_clock::now();

	treeHandlerRef->RecalculatePartitioning();

	auto RecalcEnd = std::chrono::high_resolution_clock::now();


	float msTakenRecalc = std::chrono::duration_cast<std::chrono::microseconds>(RecalcEnd - RecalcStart).count();

	timeTakenRecalc += msTakenRecalc;


	treeHandlerRef->gravCalcs = 0;

	FVector deltaVelocity = FVector(0.0f, 0.0f, 0.0f);


	auto TCstart = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < myGravBodies.Num(); i++)
	{
		

		//apply change in velocity to body I
		deltaVelocity = dt * treeHandlerRef->getApproxForce(myGravBodies[i],treeHandlerRef->treeNodeRoot);
		myGravBodies[i]->velocity += deltaVelocity;

	}

	auto TCend = std::chrono::high_resolution_clock::now();


	float msTakenTC = std::chrono::duration_cast<std::chrono::microseconds>(TCend - TCstart).count();

	timeTakenTC += msTakenTC;


}

// Called every frame
void ANBodyHandler::Tick(float DeltaTime)              
{
	Super::Tick(DeltaTime);

	



	//gradually spawn bodies to avoid a large lag spike at the start
	if(spawningBodies)
	{
		graduallySpawnBodies(SpawnsPerFrame);
	}
	else if(notPaused) { //tick

		
		elapsedFrameTime += DeltaTime;


		//step 0: destroy overlapping bodies from previous step - must be done before force calculation otherwise the current step will be inaccurate
		for (int i = 0; i < myGravBodies.Num(); i++)
		{
			if (myGravBodies[i]->toBeDestroyed) {
				myGravBodies[i]->Destroy();
				myGravBodies.RemoveAt(i);
				bodiesInSimulation = myGravBodies.Num();
				i--;
			}
		}

		//step 1: Gravitational calculations using desired method
		//first dt pass
		if (elapsedFrameTime >= fixedFrameTime) {



			perfITR++;

			if (perfITR >= 10) {
				perfITR = 0;

				std::string printStrRecalc = "Recalculating TC took ";
				printStrRecalc += std::to_string(timeTakenRecalc/10.0f);
				printStrRecalc += " ms. \n";
				GEngine->AddOnScreenDebugMessage(14, 5.0f, FColor::Yellow, printStrRecalc.c_str());

				std::string printStrTC = "TC gravs took ";
				printStrTC += std::to_string(timeTakenTC / 10.0f);
				printStrTC += " ms. \n";
				GEngine->AddOnScreenDebugMessage(15, 5.0f, FColor::Yellow, printStrTC.c_str());

				std::string printStrDI = "direct integration took ";
				printStrDI += std::to_string(timeTakenDI / 10.0f);
				printStrDI += " ms. \n";
				GEngine->AddOnScreenDebugMessage(13, 5.0f, FColor::Yellow, printStrDI.c_str());

				std::string printStr = "moving bodies took ";
				printStr += std::to_string(timeTakenMove / 10.0f);
				printStr += " ms. \n";
				GEngine->AddOnScreenDebugMessage(12, 5.0f, FColor::Yellow, printStr.c_str());

				timeTakenRecalc = 0.0f;
				timeTakenTC = 0.0f;
				timeTakenDI = 0.0f;
				timeTakenMove = 0.0f;

			}

			//dt influenced by simulation time scale 0.45 0.2 
			double updatedDT = fixedFrameTime * timeMultiplier * 0.027f; //0.027 makes the time as 10 days/s		


			simulationElapsedTime += updatedDT * int(elapsedFrameTime / fixedFrameTime);
			for (int j = 0; j < int(elapsedFrameTime / fixedFrameTime);j++) {
				


				
				if (useTreeCodes) {
					calculateWithTree(updatedDT);
				}
				else {
					auto startDI = std::chrono::high_resolution_clock::now();
					calculateAllVelocityChanges(updatedDT);
					auto stopDI = std::chrono::high_resolution_clock::now();


					float msTakenCALCTC = std::chrono::duration_cast<std::chrono::microseconds>(stopDI - startDI).count();

					timeTakenDI += msTakenCALCTC;

					

				}


				auto moveTIMEstart = std::chrono::high_resolution_clock::now();

				//step 2: move bodies using their updated velocity, also destroy ones that 
				for (int i = 0; i < myGravBodies.Num(); i++)
				{
					//second dt pass
					myGravBodies[i]->MoveBody(updatedDT);
					

				}

				auto moveTIMEstop = std::chrono::high_resolution_clock::now();

				float msTakenMOVE = std::chrono::duration_cast<std::chrono::microseconds>(moveTIMEstop - moveTIMEstart).count();

				timeTakenMove += msTakenMOVE;

				
			}


			elapsedFrameTime -= int(elapsedFrameTime / fixedFrameTime) * fixedFrameTime;
		}


		//optional recording of locations 
		if (simulationElapsedTime >= resetTime && ShouldReset) {
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "recorded positions");

			recordFinalPositions();
			ClearSimulation();


		}
	}

	if (treeHandlerRef->showTreeBoxes) {

		if (!useTreeCodes) {
			treeHandlerRef->RecalculatePartitioning();
		}

		treeHandlerRef->DisplaySectors(treeHandlerRef->treeNodeRoot);
	}

}


// setup function for spawning bodies - creates a new body with specified parameters
void ANBodyHandler::spawnBodyAt(FVector position_, FVector velocity_, double mass_, std::string name_, float radius_, FVector4 colour_)
{

	FActorSpawnParameters SpawnInfo;
	FRotator myRot(0, 0, 0);

	//assign body's variables
	AGravBody * newBody = GetWorld()->SpawnActor<AGravBody>(position_*1000.0f, myRot, SpawnInfo);
	newBody->velocity = velocity_;
	newBody->mass = mass_;
	newBody->position = position_;
	newBody->radius = radius_;
	newBody->toBeDestroyed = false;
	newBody->SetActorLabel(name_.c_str());
	//newBody->lastTrailPos = position_;

	if (radius_ == 0.0f) {
		radius_ = cbrt(mass_);
	}
	//currently more for display purposes
	newBody->SetActorScale3D(FVector(radius_, radius_, radius_));

	//option to set colour too
	if (colour_ != FVector4(0.0f, 0.0f, 0.0f, 0.0f)) {
		newBody->myMat->SetVectorParameterValue(TEXT("Colour"), colour_);
		//newBody.
	}


	myGravBodies.Add(newBody);

}

void ANBodyHandler::spawnSolarSystem() {


	//define and spawn the sun
	FVector bodyONE_pos = FVector(-9.0841f * pow(10, -3), 4.9241f * pow(10, -4), 2.0754f * pow(10, -4));
	float massOne = 1.0f;
	spawnBodyAt(bodyONE_pos + InitialSpawnCentre/2, FVector(0.0f, 0.0f, 0.0f), 1.0f, "Sun", 5.0f,FVector4(1.0f,1.0f,0.0f,1.0f));


	std::string bodyTwoName_;
	float massTwo = 0.0f;
	float bodyTwoScale = 0.0f;
	FVector bodyTWO_pos;
	FVector4 planetColor;


	//define which body (or if all) to spawn
	int it_begin;
	int it_end;
	if (SolarPlanetToSpawn < 0 || SolarPlanetToSpawn > 7) {
		it_begin = 0;
		it_end = 7;
	}
	else {
		it_begin = SolarPlanetToSpawn;
		it_end = SolarPlanetToSpawn;
	}

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
		spawnBodyAt(bodyTWO_pos + InitialSpawnCentre/2, RightVel, massTwo, bodyTwoName_, bodyTwoScale,planetColor);
	}

}

void ANBodyHandler::spawnTestPlanets()
{
	//20,20,2
	FVector positions[15] = {FVector(20.0f, 0.0f, 0.0f),FVector(0.0f, 5.0f,20.0f), FVector(-5.0f,20.0f,10.0f), FVector(17.0f,-10.0f,4.0f), FVector(5.0f,0.0f, 5.0f), FVector(3.0f, -17.0f, 6.0f),FVector(11.0f,11.0f,11.0f), FVector(-6.0f,12.0f,-18.0f), FVector(-17.0f,1.0f,7.0f), FVector(8.0f,-16.0f,16.0f), FVector(9.0f,3.0f,20.0f),FVector(5.0f,-10.0f,15.0f), FVector(11.0f, -19.0f,7.0f), FVector(-7.0f,17.0f,19.0f), FVector(4.0f,-10.0f,16.0f) };
	FVector directions[15] = { FVector(-5.0f,20.0f,10.0f),FVector(5.0f,-10.0f,15.0f), FVector(11.0f, -19.0f,7.0f), FVector(-7.0f,17.0f,19.0f),  FVector(-17.0f,1.0f,7.0f), FVector(8.0f,-16.0f,16.0f),FVector(17.0f,-10.0f,4.0f), FVector(5.0f,0.0f, 5.0f), FVector(20.0f, 0.0f, 0.0f),FVector(0.0f, 5.0f,20.0f), FVector(3.0f, -17.0f, 6.0f),FVector(11.0f,11.0f,11.0f), FVector(-6.0f,12.0f,-18.0f), FVector(9.0f,3.0f,20.0f), FVector(4.0f,-10.0f,16.0f) };
	float masses[15] = { 1.0f,0.3f, 0.4f, 0.5f, 0.6f, 0.2f,0.3f, 1.2f, 1.4f, 2.0f, 1.8f,0.2f, 0.5f, 0.7f, 0.4f };

	for (int i = 0; i < 15; i++) {
		std::string bodName = "Body ";
		bodName += std::to_string(i);
		spawnBodyAt(positions[i]*0.8f, directions[i]*0.5f, masses[i]*2.0f, bodName);
	}

}


//function that allows gradual spawn of initial bodies rather than all at once, avoiding a big lag spike when handlingodies
void ANBodyHandler::graduallySpawnBodies(int spawnsPerFrame) {


	for (int s = 0; s < spawnsPerFrame; s++) { // spawn all the bodies for this frame

		//Exi the spawning loop
		if (gradualSpawnerIndex >= bodiesToSpawn) {
			spawningBodies = false;
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, "Spawned all desired bodies");
			if (shouldSpawnSolarSystem) {
				spawnSolarSystem();
			}
			if (ShouldSpawnTestPlanets) {
				spawnTestPlanets();
			}
			bodiesInSimulation = myGravBodies.Num();
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

		std::string bodName = "Body ";
		bodName += std::to_string(gradualSpawnerIndex);
		spawnBodyAt(myLoc, speed_, mass_, bodName);
		bodiesInSimulation = myGravBodies.Num();
		gradualSpawnerIndex++;

	}

	return;
}


void ANBodyHandler::doubleAllScales() {
	for (int i = 0; i < myGravBodies.Num(); i++)
	{
		float scale_ = myGravBodies[i]->GetActorScale().X;
		myGravBodies[i]->SetActorScale3D(FVector(scale_*2, scale_*2, scale_*2));

	}
}


void ANBodyHandler::pauseSimulation() {
	if (notPaused) {
		notPaused = false;
	}
	else {
		notPaused = true;
	}
}

void ANBodyHandler::raiseSimulationSpeed()
{
	timeMultiplier *= 2.0f;
	notPaused = true;
}


void ANBodyHandler::lowerSimulationSpeed()
{
	timeMultiplier /= 2.0f;
	notPaused = true;
}

//Work in progress
void ANBodyHandler::RecentreSimulation() {


	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, resultMoveCect.ToString());
	FVector AvgBodyPos = FVector(0, 0, 0);


	for (int i = 0; i < myGravBodies.Num(); i++)
	{
		AvgBodyPos += myGravBodies[i]->GetActorLocation();
	}
	AvgBodyPos /= myGravBodies.Num();

	FVector camToSimVector = AvgBodyPos - UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCameraManager::StaticClass())->GetActorLocation();


	APlayerCameraManager* CameraManagerRef = Cast<APlayerCameraManager>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCameraManager::StaticClass()));
	auto POV_ = CameraManagerRef->GetCameraCachePOV();
	POV_.Rotation = camToSimVector.GetSafeNormal().Rotation();
	CameraManagerRef->SetCameraCachePOV(POV_);

	//displacement vector to move it back is an inverted position vector
	FVector dispVector = -1 * AvgBodyPos;

	camToSimVector.Normalize();
	UGameplayStatics::GetActorOfClass(GetWorld(), ADefaultPawn::StaticClass())->SetActorLocation(-camToSimVector * 2000);


	for (int i = 0; i < myGravBodies.Num(); i++)
	{
		myGravBodies[i]->AddActorWorldOffset(dispVector);
	}
}

void ANBodyHandler::ClearSimulation() {
	//timeMultiplier = 1.0f;
	simulationElapsedTime = 0.0f;
	bodiesInSimulation = 0;
	//notPaused = false;
	gradualSpawnerIndex = 0;
	spawningBodies = true;
	elapsedFrameTime = 0.0f;


	while(!myGravBodies.IsEmpty())
	{
		myGravBodies[0]->Destroy();
		myGravBodies.RemoveAt(0);
	}

}

