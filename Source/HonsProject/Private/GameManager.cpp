// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager.h"
#include "Asteroid.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AGameManager::AGameManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGameManager::BeginPlay()
{
	Super::BeginPlay();
	

	FTransform tr;
	tr.SetIdentity();

	TArray<AActor*> FoundActors;
	//init game variables
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawn::StaticClass(),FoundActors);
	for (int i = 0; i < FoundActors.Num(); i++) {
		if (FoundActors[i]->GetActorLabel().Contains("PlayerPawn"))
		{
			GEngine->AddOnScreenDebugMessage(-1, 0.4f, FColor::Green, "FOUND PAWN");
			playerRef = FoundActors[i];
		}
	}
	

	//create Nbody handler
	BodyHandler_ref = Cast<UNBodyHandler>(this->AddComponentByClass(UNBodyHandler::StaticClass(), false, tr, true));
	BodyHandler_ref->RegisterComponent();

	//setup Nbody handler
	BodyHandler_ref->useTreeCodes_ = useTreeCodes;

	//create tree code handler
	TreeHandler_ref = Cast<UTreeHandler>(this->AddComponentByClass(UTreeHandler::StaticClass(), false, tr, true));
	TreeHandler_ref->RegisterComponent();
	TreeHandler_ref->bodyHandlerBodies = &BodyHandler_ref->myGravBodies;

	//assign tree code handler
	BodyHandler_ref->treeHandlerRef = TreeHandler_ref;

	startSpawning(SimulationDesiredBodies, simulationRadius, SpawnMaxSpeed, SpawnMaxMass);
}

//clears up bodies from the simulation
void AGameManager::deleteDestroyedBodies() {

	// 1 0
	// 1 0
	for (int i = 0; i < BodyHandler_ref->myGravBodies.Num(); i++)
	{
		UGravBodyComponent* CompIT = BodyHandler_ref->myGravBodies[i];

		//nullptr fix - actor mightv'e been deleted thru BP's
		if (!CompIT) {
			BodyHandler_ref->myGravBodies.RemoveAt(i);
			if(drawDebugs)GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "DeleteDestroyed Exited Cause of NullPTR");
			return;
		}


		//destroy body if it's out of bounds or it's supposed to be destroyed
		if (CompIT->toBeDestroyed || (CompIT->position - (playerRef->GetActorLocation() / 1000.0f)).Length() > simulationRadius) {

			CompIT->GetOwner()->Destroy();
			BodyHandler_ref->myGravBodies[i]->DestroyComponent();
			BodyHandler_ref->myGravBodies.RemoveAt(i);
			i--;
			//return;
		}

	}
}

//ensures that there's always the desired amount of asteroids in the simulation
void AGameManager::inGameAsteroidHandle()
{

	if (BodyHandler_ref->myGravBodies.Num() < SimulationDesiredBodies) {
		spawnAsteroidToGame();
		if (drawDebugs)GEngine->AddOnScreenDebugMessage(-1, 0.4f, FColor::Green, "added new asteroid");

	}

}


// Called every frame
void AGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



	if (!spawningBodies) {

		//DrawDebugSphere(GetWorld(), playerRef->GetActorLocation(), simulationRadius * 1000, 10, FColor::Green,false, 0.0f);


		//step 0: destroy overlapping bodies from previous step - must be done before force calculation otherwise the current step will be inaccurate
		deleteDestroyedBodies();
		inGameAsteroidHandle();

		//step 1: Gravitational calculations using fixed time updates
		//dt influenced by simulation time scale 
		double updatedDT = DeltaTime * timeMultiplier * 0.027f; //0.027 makes the time as 10 days/s		

		if (!useTreeCodes) {
			BodyHandler_ref->calculateAllVelocityChanges(updatedDT);

		}
		else {
			BodyHandler_ref->calculateWithTree(updatedDT, false, false);

		}
		//step 2: move bodies using their updated velocity, also destroy ones that 
		BodyHandler_ref->moveBodies(true, updatedDT);

		
	}
	else{
		graduallySpawnBodies(5);
	}

}




//function that allows gradual spawn of initial bodies rather than all at once, avoiding a big lag spike when handlingodies
void AGameManager::graduallySpawnBodies(int spawnsPerFrame) {


	for (int s = 0; s < spawnsPerFrame; s++) { // spawn all the bodies for this frame

		//Exi the spawning loop
		if (gradualSpawnerIndex >= bodiesToSpawn) {

			spawningBodies = false;
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, "Spawned all desired bodies");

			return;
		}

		spawnAsteroidToGame();
		gradualSpawnerIndex++;

	}

	return;
}


// setup function for spawning bodies - creates a new body with specified parameters
void AGameManager::spawnAsteroidAt(FVector position_, FVector velocity_, double mass_, FVector4 colour_)
{

	FActorSpawnParameters SpawnInfo;
	FRotator myRot(0, 0, 0);

	//assign body's variables
	AAsteroid* newBody = GetWorld()->SpawnActor<AAsteroid>(position_ * 1000.0f, myRot, SpawnInfo);

	FTransform tr;
	tr.SetIdentity();

	//create Nbody handler
	newBody->GravComp = Cast<UGravBodyComponent>(newBody->AddComponentByClass(UGravBodyComponent::StaticClass(), true, tr, true));
	newBody->GravComp->RegisterComponent();

	newBody->GravComp->toBeDestroyed = false;
	newBody->GravComp->position = position_;
	newBody->GravComp->velocity = velocity_;
	newBody->GravComp->mass = mass_;
	//newBody->SetActorLabel(name_);


	//if (radius_ == 0.0f) {
	//	radius_ = cbrt(mass_);
	//}

	float scale_ = cbrt(mass_);
	scale_ /= 2.5f;
	//currently more for display purposes
	newBody->SetActorScale3D(FVector(scale_, scale_, scale_));

	//option to set colour too
	colour_.X = FMath::FRandRange(0.3f, 1.0f);
	colour_.Y = FMath::FRandRange(0.3f, 1.0f);
	colour_.Z = FMath::FRandRange(0.3f, 1.0f);
	newBody->myMat->SetVectorParameterValue(TEXT("Colour"), colour_);
	newBody->GravComp->myCol = colour_;

	BodyHandler_ref->myGravBodies.Add(newBody->GravComp);




}

//spawns asteroid to the simulation following the defined scene logic 
void AGameManager::spawnAsteroidToGame()
{
	//random location
	FVector myLoc(0, 0, 0);
	myLoc.X += FMath::FRandRange(-simulationRadius, simulationRadius);
	myLoc.Y += FMath::FRandRange(-simulationRadius, simulationRadius);
	myLoc.Z += FMath::FRandRange(-simulationRadius, simulationRadius);
	myLoc += playerRef->GetActorLocation() / 1000.0f; //translate it to desired spawn centre

	//random speed
	FVector speed_ = FVector(0, 0, 0);
	speed_.X += FMath::FRandRange(-SpawnMaxSpeed, SpawnMaxSpeed);
	speed_.Y += FMath::FRandRange(-SpawnMaxSpeed, SpawnMaxSpeed);
	speed_.Z += FMath::FRandRange(-SpawnMaxSpeed, SpawnMaxSpeed);

	//random mass
	float mass_ = 0.001f;
	mass_ += FMath::FRandRange(0.0f, SpawnMaxMass);

	FString bodName = "Body ";
	bodName.Append(std::to_string(OverallSpawnerIndex).c_str());
	spawnAsteroidAt(myLoc, speed_, mass_, FVector4(1.0f, 0.0f, 1.0f, 1.0f));
	OverallSpawnerIndex++;
}
