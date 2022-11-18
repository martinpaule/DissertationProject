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


	//example how to bind functions in code, keep to avoid looking this up again
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
		myGravBodies.Add(Cast<AGravBody>(FoundActors[n]));
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, "added manually placed body to array");
	}


}

//direct integration of gravitational dynamics using Newtonian formulae
void ANBodyHandler::calculateAllVelocityChanges(float dt) {
	for (int i = 0; i < myGravBodies.Num(); i++)
	{
		FVector sumOfForces = FVector(0.0f, 0.0f, 0.0f);
		for (int j = 0; j < myGravBodies.Num(); j++)
		{
			if (i != j) //ignore the body's own force on itself
			{
				FVector distance = myGravBodies[j]->GetActorLocation() - myGravBodies[i]->GetActorLocation();
				float length_ = distance.Length();
				sumOfForces += bigG * myGravBodies[j]->mass * distance / length_ * length_ * length_;
			}
		}
		myGravBodies[i]->velocity += dt * (sumOfForces) / myGravBodies[i]->mass;
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
		float updatedDT = DeltaTime * timeMultiplier;
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
void ANBodyHandler::spawnBodyAt(FVector position_, FVector velocity_, float mass_)
{

	FActorSpawnParameters SpawnInfo;
	FRotator myRot(0, 0, 0);

	AGravBody * newBody = GetWorld()->SpawnActor<AGravBody>(position_, myRot, SpawnInfo);
	newBody->velocity = velocity_;
	newBody->mass = mass_;
	float scale_ = cbrt(mass_);
	newBody->SetActorScale3D(FVector(scale_, scale_, scale_));
	newBody->toBeDestroyed = false;
	myGravBodies.Add(newBody);

}

//function that allows gradual spawn of initial bodies rather than all at once, avoiding a big lag spike when handlingodies
void ANBodyHandler::graduallySpawnBodies(int spawnsPerFrame) {


	for (int s = 0; s < spawnsPerFrame; s++) { // spawn all the bodies for this frame

		if (gradualSpawnerIndex >= bodiesToSpawn) {
			spawningBodies = false;
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, "Spawned all desired bodies");
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

		spawnBodyAt(myLoc, speed_, mass_);
		BodiesInSimulation = myGravBodies.Num();
		gradualSpawnerIndex++;
		
	}

	return;
}