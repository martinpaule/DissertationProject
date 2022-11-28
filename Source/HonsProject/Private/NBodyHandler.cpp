// Fill out your copyright notice in the Description page of Project Settings.


#include "NBodyHandler.h"
#include <Kismet/GameplayStatics.h>
#include "Misc/DateTime.h"
#include "GameFramework/DefaultPawn.h"
#include <Kismet/KismetMathLibrary.h>
#include "DrawDebugHelpers.h"

//#include "gra"
// Sets default values
ANBodyHandler::ANBodyHandler()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	timeMultiplier = 1.0f;

	
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
	
	//UGameplayStatics::GetAllActorsOfClass()
	
	
	for (int n = 0; n < FoundActors.Num(); n++) {
		AGravBody* ref = Cast<AGravBody>(FoundActors[n]);
		myGravBodies.Add(ref);
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, std::to_string(n).c_str());
	}


}

//direct integration of gravitational dynamics using Newtonian formulae
void ANBodyHandler::calculateAllVelocityChanges(double dt) {


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
			}
		}

		//apply change in velocity to body I
		deltaVelocity = dt* sumOfForces;
		myGravBodies[i]->velocity += deltaVelocity;

		//PRINT
		if (i == 1) {
			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, deltaVelocity.ToString().Append(" is deltaVelocity to body ").Append(myGravBodies[i]->GetActorLabel()));
			if (deltaVelocity.Length() == 0.0f) {
				GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, "length is 0!!!");
			}
			else {
				GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, std::to_string(deltaVelocity.Length()).c_str());
			}

			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, myGravBodies[i]->velocity.ToString().Append(" is current velocity of body").Append(myGravBodies[i]->GetActorLabel()));
		}
	}
}

// Called every frame
void ANBodyHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//gradually spawn bodies to avoid a large lag spike at the start

	
	if (showPlanetNames) {
		for (int i = 0; i < myGravBodies.Num(); i++)
		{
			DrawDebugString(GetWorld(), myGravBodies[i]->GetActorLocation() += FVector(0.0f, 0.0f, myGravBodies[i]->GetActorScale3D().X * 50.0f + 50.0f), myGravBodies[i]->GetActorLabel(), this, FColor::White, 0.0f, false, 2.0f);
		}
	}

	if(spawningBodies)
	{
		graduallySpawnBodies(SpawnsPerFrame);
	}
	else if(notPaused) { //tick

		//dt influenced by simulation time scale
		double updatedDT = DeltaTime * timeMultiplier;
		SimulationElapsedTime += updatedDT;


		//step 0: destroy overlapping bodies from previous step - must be done before force calculation otherwise the current step will be inaccurate
		for (int i = 0; i < myGravBodies.Num(); i++)
		{
			if (myGravBodies[i]->toBeDestroyed) {
				myGravBodies[i]->Destroy();
				myGravBodies.RemoveAt(i);
				BodiesInSimulation = myGravBodies.Num();
				i--;
			}
			if (showPlanetNames) {

				DrawDebugString(GetWorld(), myGravBodies[i]->GetActorLocation()+= FVector(0.0f,0.0f,myGravBodies[i]->GetActorScale3D().X *50.0f + 50.0f), myGravBodies[i]->GetActorLabel(), this, FColor::White, 0.0f, false, 2.0f);
			}
		}

		//step 1: direct integration of gravitational calculations, find delta velocity of all bodies
		//first dt pass
		calculateAllVelocityChanges(updatedDT);


		//step 2: move bodies using their updated velocity, also destroy ones that 
		for (int i = 0; i < myGravBodies.Num(); i++)
		{
			//second dt pass
			myGravBodies[i]->MoveBody(updatedDT);
		}
	}
}


// setup function for spawning bodies - creates a new body with specified parameters
void ANBodyHandler::spawnBodyAt(FVector position_, FVector velocity_, double mass_, std::string name_, float radius_)
{

	FActorSpawnParameters SpawnInfo;
	FRotator myRot(0, 0, 0);

	//assign body's variables
	AGravBody * newBody = GetWorld()->SpawnActor<AGravBody>(position_*1000.0f, myRot, SpawnInfo);
	newBody->velocity = velocity_;
	newBody->mass = mass_;
	newBody->position = position_;
	newBody->radius = radius_;

	//currently more for display purposes
	newBody->SetActorScale3D(FVector(radius_, radius_, radius_));


	newBody->toBeDestroyed = false;
	newBody->SetActorLabel(name_.c_str());
	myGravBodies.Add(newBody);

}

void ANBodyHandler::spawnSolarSystem() {

	//values obtained from Nasa's Horizons for the date of 18/11/2022

	//Sun

	////specify revelavant values
	//FVector bodyONE_pos = FVector(0.0f, 0.0f, 0.0f);
	//FVector bodyTWO_pos = FVector(1000.0f, 0.0f, 0.0f);
	//float massOne = 1000.0f;
	//float distance = (bodyONE_pos - bodyTWO_pos).Length();
	//
	//float YVel = bigG * massOne;
	//YVel /= distance;
	//YVel = sqrt(YVel);
	//
	//FVector dir = bodyONE_pos - bodyTWO_pos;
	//dir.Normalize();
	//FVector RightVel = UKismetMathLibrary::GetRightVector(dir.Rotation()) * YVel;
	//
	//spawnBodyAt(bodyONE_pos, FVector(0.0f, 0.0f, 0.0f), massOne, "HIDEME", 3.0f);
	////spawnBodyAt(bodyTWO_pos, FVector(0.0f, YVel, 0.0f), 5.0f, "smallBody", 1.0f);
	//spawnBodyAt(bodyTWO_pos, RightVel, 5.0f, "smallBody", 1.0f);

	timeMultiplier = 0.125f;
	float sizeScaler = 1000.0f;

	FVector bodyONE_pos = FVector(-9.0841f * pow(10, -3), 4.9241f * pow(10, -4), 2.0754f * pow(10, -4));
	float massOne = 1.0f;

	spawnBodyAt(bodyONE_pos, FVector(0.0f, 0.0f, 0.0f), 1.0f, "Sun", 5.0f);

	std::string bodyTwoName_;
	float massTwo;
	float bodyTwoScale;
	FVector bodyTWO_pos;

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

	for (it_begin; it_begin <= it_end; it_begin++) {
		//which planet to simulate orbiting around the sun
		switch (it_begin) {

		case 0: //
			bodyTwoName_ = "Jupiter";
			massTwo = 0.9545f * pow(10, -3);
			bodyTwoScale = 2.2f;
			bodyTWO_pos = FVector(4.8917f, 7.0304f * pow(10, -1), -1.1236f * pow(10, -1));
			break;
		case 1: //
			bodyTwoName_ = "Saturn";
			massTwo = 2.859 * pow(10, -4);
			bodyTwoScale = 1.9f;
			bodyTWO_pos = FVector(8.0121f, -5.7062f, -2.1978f * pow(10, -1));
			break;
		case 2: //
			bodyTwoName_ = "Neptune";
			massTwo = 5.15f * pow(10, -5);
			bodyTwoScale = 1.4f;
			bodyTWO_pos = FVector(2.9739f * pow(10, 1), -3.081f, -6.2191f * pow(10, -1));
			break;
		case 3: //
			bodyTwoName_ = "Uranus";
			massTwo = 4.364f * pow(10, -5);
			bodyTwoScale = 1.5f;
			bodyTWO_pos = FVector(1.3488f * pow(10, 1), 1.4317f * pow(10, 1), -1.2157f * pow(10, -1));
			break;
		case 4: //
			bodyTwoName_ = "Earth";
			massTwo = 3.003f * pow(10, -6);
			bodyTwoScale = 1.1f;
			bodyTWO_pos = FVector(5.5374f * pow(10, -1), 8.1332f * pow(10, -1), 1.5998f * pow(10, -4));
			break;
		case 5: //
			bodyTwoName_ = "Venus";
			massTwo = 2.447f * pow(10, -6);
			bodyTwoScale = 0.9f;
			bodyTWO_pos = FVector(-2.4692f * pow(10, -1), -6.8513f * pow(10, -1), 4.5184f * pow(10, -3));
			break;
		case 6: //
			bodyTwoName_ = "Mars";
			massTwo = 3.226f * pow(10, -7);
			bodyTwoScale = 0.6f;
			bodyTWO_pos = FVector(6.2360f * pow(10, -1), 1.3693f, 1.3376f * pow(10, -2));
			break;
		case 7:
			bodyTwoName_ = "Mercury";
			massTwo = 1.66f * pow(10, -7);
			bodyTwoScale = 0.5f;
			bodyTWO_pos = FVector(-1.514f * pow(10, -1), -4.4286f * pow(10, -1), -2.2969f * pow(10, -2));
			break;
		}


		//v^2 = m1 * G / distance

		FVector dir = bodyONE_pos - bodyTWO_pos;
		float distance = dir.Length();

		float YVel = bigG * massOne;
		YVel /= distance;
		YVel = sqrt(YVel);

		dir.Normalize();
		FVector RightVel = UKismetMathLibrary::GetRightVector(dir.Rotation()) * YVel;
		spawnBodyAt(bodyTWO_pos, RightVel, massTwo, bodyTwoName_, bodyTwoScale);
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
			BodiesInSimulation = myGravBodies.Num();
			return;
		}

		//random location
		FVector myLoc(-SpawnLocationBounds / 2, -SpawnLocationBounds / 2, -SpawnLocationBounds / 2);
		myLoc.X += FMath::RandRange(0, SpawnLocationBounds);
		myLoc.Y += FMath::RandRange(0, SpawnLocationBounds);
		myLoc.Z += FMath::RandRange(0, SpawnLocationBounds);
		myLoc += InitialSpawnCentre; //translate it to desired spawn centre

		//random speed
		FVector speed_ = FVector(-SpawnInitialMaxSpeed / 2, -SpawnInitialMaxSpeed / 2, -SpawnInitialMaxSpeed / 2);
		speed_.X += FMath::RandRange(0, SpawnInitialMaxSpeed);
		speed_.Y += FMath::RandRange(0, SpawnInitialMaxSpeed);
		speed_.Z += FMath::RandRange(0, SpawnInitialMaxSpeed);

		//random mass
		float mass_ = 0.01f;
		mass_ += FMath::FRandRange(0.0f, SpawnInitialMaxMass);

		std::string bodName = "Body ";
		bodName += std::to_string(gradualSpawnerIndex);
		spawnBodyAt(myLoc, speed_, mass_, bodName);
		BodiesInSimulation = myGravBodies.Num();
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
	timeMultiplier = 1.0f;
	SimulationElapsedTime = 0.0f;
	BodiesInSimulation = 0;
	notPaused = false;
	gradualSpawnerIndex = 0;
	spawningBodies = true;

	while(!myGravBodies.IsEmpty())
	{
		myGravBodies[0]->Destroy();
		myGravBodies.RemoveAt(0);
	}

}

void ANBodyHandler::flipPlanetNames() {
	showPlanetNames = !showPlanetNames;
}