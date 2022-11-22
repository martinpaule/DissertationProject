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
	for (int i = 0; i < myGravBodies.Num(); i++)
	{
		FVector sumOfForces = FVector(0.0f, 0.0f, 0.0f);
		for (int j = 0; j < myGravBodies.Num(); j++)
		{
			if (i != j) //ignore the body's own force on itself
			{
				FVector distance = myGravBodies[j]->GetActorLocation() - myGravBodies[i]->GetActorLocation();
				double length_ = distance.Length();
				double cubicLength = length_ * length_ * length_;
				double disEditor = bigG * myGravBodies[j]->mass / cubicLength;
				FVector deltaForce = distance * disEditor;
				//FVector deltaForce = bigG * myGravBodies[j]->mass * distance / cubicLength; //PROBLEM
				sumOfForces += deltaForce;
				if (i == 1) {
					GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, deltaForce.ToString().Append(" is delta Force to body").Append(myGravBodies[i]->GetActorLabel()));
				}
			}
		}
		FVector deltaVelocity = dt* (sumOfForces) / myGravBodies[i]->mass;

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
		if (enambleDispCalc) {
			calculateAllVelocityChanges(updatedDT);
		}

		//step 2: move bodies using their updated velocity, also destroy ones that 
		for (int i = 0; i < myGravBodies.Num(); i++)
		{
			//second dt pass
			myGravBodies[i]->MoveBody(updatedDT);
		}
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

// setup function for spawning bodies - creates a new body with specified parameters
void ANBodyHandler::spawnBodyAt(FVector position_, FVector velocity_, double mass_, std::string name_, float radius_)
{

	FActorSpawnParameters SpawnInfo;
	FRotator myRot(0, 0, 0);

	AGravBody * newBody = GetWorld()->SpawnActor<AGravBody>(position_, myRot, SpawnInfo);
	newBody->velocity = velocity_;
	newBody->mass = mass_;
	float scale_;

	if (radius_ == 0.0f) { //universal scale for a fast, intuitive N-Body simulation
		scale_ = cbrt(mass_);
	}
	else {
		scale_ = radius_ * 2.0f / 100.0f; //specified scale
		newBody->universalDensity = false;

	}
	newBody->SetActorScale3D(FVector(scale_, scale_, scale_));
	newBody->radius = radius_;
	newBody->toBeDestroyed = false;
	//std::string a = "banana";
	newBody->SetActorLabel(name_.c_str());
	myGravBodies.Add(newBody);

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
		FVector myLoc( -SpawnLocationBounds / 2, -SpawnLocationBounds / 2, -SpawnLocationBounds / 2);
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

void ANBodyHandler::spawnSolarSystem() {

	//values obtained from Nasa's Horizons for the date of 18/11/2022

	//Sun
	spawnBodyAt(FVector(-1.35896f * pow(10, 6), 7.36638f * pow(10, 4), 3.10470f * pow(10, 4)), FVector(6.57942f * pow(10, -4), -1.56718f * pow(10, -2), 1.15770f * pow(10, -4)), 1988500.0f * pow(10, 24), "Sun", 695700.0f);

	//Mercury
	spawnBodyAt(FVector(-2.2649f * pow(10, 7), -6.62515f * pow(10, 7), -3.43616f * pow(10, 6)), FVector(36.59884f, -12.48037f, -4.37549f), 3.302f * pow(10, 23),"Mercury", 2440.0f);
	
	//Venus
	spawnBodyAt(FVector(-3.69384f * pow(10, 7), -1.02494f * pow(10, 8), 6.75944f * pow(10, 5)), FVector(32.84882f, -11.64133f, -2.0549f), 48.685f * pow(10, 23),"Venus", 6051.84f);
	
	//Earth
	spawnBodyAt(FVector(8.28377f * pow(10, 7), 1.21670f * pow(10, 8), 2.39333f * pow(10, 4)), FVector(-24.97096f, 16.8404f, -5.61289f * pow(10, -5)), 5.97219f * pow(10, 24),"Earth", 6371.01f);
	
	//Mars
	spawnBodyAt(FVector(9.32897f * pow(10, 7), 2.04846f * pow(10, 8), 2.00101f * pow(10, 6)), FVector(-21.07414f, 12.21325f, 0.77336f), 6.4171f * pow(10, 23),"Mars", 3389.92f);
	
	//Jupiter
	spawnBodyAt(FVector(7.31791f * pow(10, 8), 1.05173f * pow(10, 8), -1.68084f * pow(10, 7)), FVector(-2.00956f, 13.54647f, -0.01125f), 1898.18722f * pow(10, 24),"Jupiter", 69911.0f);
	
	//Saturn 
	spawnBodyAt(FVector(1.19859f * pow(10, 9), -8.53635f * pow(10, 8), -3.28788f * pow(10, 7)), FVector(5.06329f, 7.8505f, -0.33867f), 5.6834f * pow(10, 26),"Saturn", 58232.0f);
	
	//Uranus
	spawnBodyAt(FVector(2.01783f * pow(10, 9), 2.14180f * pow(10, 9), -1.81867f * pow(10, 7)), FVector(-5.00658f, 4.35256f, 0.08107f), 86.813f * pow(10, 24),"Uranus", 25362.0f);
	
	//Neptune
	spawnBodyAt(FVector(4.44886f * pow(10, 9), -4.60904f * pow(10, 8), -9.30371f * pow(10, 7)), FVector(0.52454f, 5.43826f, -0.12470), 102.409f * pow(10, 24), "Neptune", 24624.0f);
	//905
}

void ANBodyHandler::doubleAllScales() {
	for (int i = 0; i < myGravBodies.Num(); i++)
	{
		float scale_ = myGravBodies[i]->GetActorScale().X;
		myGravBodies[i]->SetActorScale3D(FVector(scale_*2, scale_*2, scale_*2));
	}
}