// Fill out your copyright notice in the Description page of Project Settings.


#include "NBodyHandler.h"
#include <Kismet/GameplayStatics.h>
#include "Misc/DateTime.h"
#include "GameFramework/DefaultPawn.h"
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

	FVector distance;
	double length_;
	double cubicLength;
	double disEditor;
	FVector deltaForce;
	FVector sumOfForces;
	for (int i = 0; i < myGravBodies.Num(); i++)
	{
		sumOfForces = FVector(0.0f, 0.0f, 0.0f);
		for (int j = 0; j < myGravBodies.Num(); j++)
		{
			if (i != j) //ignore the body's own force on itself
			{
				distance = myGravBodies[j]->position - myGravBodies[i]->position;
				length_ = distance.Length();
				cubicLength = length_ * length_ * length_;
				disEditor = bigG * myGravBodies[j]->mass / cubicLength;
				deltaForce = distance * disEditor;
				//FVector deltaForce = bigG * myGravBodies[j]->mass * distance / cubicLength; //PROBLEM
				sumOfForces += deltaForce;
			}
		}
		FVector deltaVelocity = dt* (sumOfForces);

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
		//myGravBodies[i]->velocity += FVector(1.0f, 1.0f, 1.0f) * dt;
	}
}

// Called every frame
void ANBodyHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(spawningBodies) //gradually spawn bodies to avoid a large lag spike at the start
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
		}

		//step 1: direct integration of gravitational calculations
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

	AGravBody * newBody = GetWorld()->SpawnActor<AGravBody>(position_*100.0f, myRot, SpawnInfo);
	newBody->velocity = velocity_;
	newBody->mass = mass_;
	newBody->position = position_;
	float scale_;

	//if (radius_ == 0.0f) { //universal scale for a fast, intuitive N-Body simulation
	//	scale_ = cbrt(mass_);
	//}
	//else {
	//	scale_ = radius_ * 2.0f / 100.0f; //specified scale
	//	newBody->universalDensity = false;
	//
	//}

	scale_ = 1.0f;

	newBody->SetActorScale3D(FVector(scale_, scale_, scale_));
	newBody->radius = radius_;
	newBody->toBeDestroyed = false;
	//std::string a = "banana";
	newBody->SetActorLabel(name_.c_str());
	myGravBodies.Add(newBody);

}

void ANBodyHandler::spawnSolarSystem() {

	//values obtained from Nasa's Horizons for the date of 18/11/2022

	//Sun
	//v^2 = m1 * G / distance
	//spawnBodyAt(FVector(0.0f,0.0f,0.0f), FVector(0.0f, 0.0f, 0.0f), 100.0f, "BigBody", 1.0f);
	//
	//spawnBodyAt(FVector(1000.0f,0.0f,0.0f), FVector(0.0f, sqrt(0.1f * bigG), 0.0f), 5.0f, "BigBody", 1.0f);
	
	// X =-9.080183500751722E-03 Y = 4.109701817603900E-04 Z = 2.081268463124045E-04
	spawnBodyAt(FVector(-9.0802f * pow(10,-3), 4.1097f * pow(10, -4), 2.0813f * pow(10, -4)), FVector(1.7915f * pow(10,-4), -3.302f * pow(10,-3),2.3419 * pow(10,-5)), 1.0f, "Sun", 1.0f);

	spawnBodyAt(FVector(4.7912f * pow(10,-2), -4.5368f * pow(10, -1), -4.2128f * pow(10, -2)),FVector(8.1296f, 1.8023f,-0.598f), 1.66f * pow(10,-7), "Mercury", 1.0f);

	spawnBodyAt(FVector(0.512f, 1.428f, 1.7345f * pow(10, -2)),FVector(-4.604f, 2.1827f, 0.1587f), 3.226f * pow(10,-7), "Mars", 1.0f);

	spawnBodyAt(FVector(-7.0539f * pow(10,-2), -7.2366f * pow(10, -1), -6.1861 * pow(10, -3)),FVector(7.307f, -0.6558f,-0.4305f), 2.447f * pow(10,-6), "Venus", 1.0f);

	spawnBodyAt(FVector(4.1744f * pow(10,-1), 8.9039f * pow(10, -1), 1.6229f * pow(10, -4)),FVector(-5.767f, 2.6862f, 3.035f * pow(10,-5)), 3.003f * pow(10,-6), "Earth", 1.0f);

	spawnBodyAt(FVector(13.462f, 14.3397f, -0.1211f),FVector(-1.0571f, 0.9157f, 1.7131f * pow(10,-2)), 4.364f * pow(10,-5), "Uranus", 1.0f);

	spawnBodyAt(FVector(29.7415f, -3.0527f, -0.6225f),FVector(0.1094f, 1.1466f, -2.6132f * pow(10,-2)), 5.15f * pow(10,-5), "Naptune", 1.0f);

	spawnBodyAt(FVector(8.0383f, -5.6653f, -0.2215f),FVector(1.0591f, 1.6602f,-7.0919 * pow(10,-2)), 4.364f* pow(10,-4), "Saturn", 1.0f);

	spawnBodyAt(FVector(4.8808f, 0.7734f, -0.1124f),FVector(-0.4627f, 2.8497f,-1.4735f*pow(10,-3)), 0.9545f* pow(10,-3), "Jupiter", 1.0f);

}


//function that allows gradual spawn of initial bodies rather than all at once, avoiding a big lag spike when handlingodies
void ANBodyHandler::graduallySpawnBodies(int spawnsPerFrame) {


	for (int s = 0; s < spawnsPerFrame; s++) { // spawn all the bodies for this frame

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
