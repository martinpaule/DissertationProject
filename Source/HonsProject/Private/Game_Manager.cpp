// Fill out your copyright notice in the Description page of Project Settings.


#include "Game_Manager.h"
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
			if (drawDebugs)GEngine->AddOnScreenDebugMessage(-1, 0.4f, FColor::Green, "FOUND PAWN");
			playerRef = FoundActors[i];
		}
	}
	

	//create Nbody handler
	BodyHandler_ref = Cast<UNBodyHandler>(this->AddComponentByClass(UNBodyHandler::StaticClass(), false, tr, true));
	BodyHandler_ref->RegisterComponent();

	//setup Nbody handler
	BodyHandler_ref->useTreeCodes_ = useTreeCodes;
	BodyHandler_ref->drawDebugs = drawDebugs;


	BodyHandler_ref->constructTreeHandler();


	BodyHandler_ref->treeHandler->setManualTreeRoot(SimulationCentre, simulationRadius*1.2f);
}

//clears up bodies from the simulation
void AGameManager::handleDestroyingAsteroids() {

	bool somethingChanged = false;

	for (int i = 0; i < BodyHandler_ref->myGravBodies.Num(); i++)
	{
		UGravBodyComponent* CompIT = BodyHandler_ref->myGravBodies[i];

		//nullptr fix - actor mightv'e been deleted thru BP's
		if (!CompIT) {
			BodyHandler_ref->myGravBodies.RemoveAt(i);
			if(drawDebugs)GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "Destroyed GravBodyComponent Cause of NullPTR");
			somethingChanged = true;
			break;
		}


		//destroy body if it's out of bounds or it's supposed to be destroyed
		//if (CompIT->toBeDestroyed || (CompIT->position - (playerRef->GetActorLocation() / 1000.0f)).Length() > simulationRadius) {
		if (CompIT->toBeDestroyed || (CompIT->position - SimulationCentre).Length() > simulationRadius) {

			if (drawDebugs)GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Orange, "Destroyed GravBodyComponent Cause OutOfBounds or ToBeDestroyed");
			
			//TreeHandler_ref->updateAvgPosCombMassOfAllSectorsContaining(BodyHandler_ref->myGravBodies[i]);//delete cleanup function



			CompIT->GetOwner()->Destroy();
			BodyHandler_ref->myGravBodies[i]->DestroyComponent();
			BodyHandler_ref->myGravBodies.RemoveAt(i);
			somethingChanged = true;
			i--;
			
		}

	}

	if (somethingChanged) {
		BodyHandler_ref->treeHandler->setManualTreeRoot(SimulationCentre, simulationRadius * 1.2f);
		if (drawDebugs)GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Orange, "Had To Manually recalculate");
	}
}

//ensures that there's always the desired amount of asteroids in the simulation
void AGameManager::inGameAsteroidHandle()
{

	if (BodyHandler_ref->myGravBodies.Num() < SimulationDesiredBodies) {
		spawnAsteroidToGame();
		//if (drawDebugs)GEngine->AddOnScreenDebugMessage(-1, 0.4f, FColor::Green, "added new asteroid");

	}

}


// Called every frame
void AGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (drawDebugs)DrawDebugSphere(GetWorld(), SimulationCentre*1000.0f, simulationRadius * 1000, 10, FColor::Green,false, 0.0f);


	//step 0: destroy overlapping bodies from previous step - must be done before force calculation otherwise the current step will be inaccurate
	handleDestroyingAsteroids();
	inGameAsteroidHandle();

	//step 1: Gravitational calculations using fixed time updates
	//dt influenced by simulation time scale 
	double updatedDT = DeltaTime * timeMultiplier;	

	BodyHandler_ref->calculateWithTree(updatedDT, false, true);

	//step 2: move bodies using their updated velocity, also destroy ones that 
	BodyHandler_ref->moveBodies(true, updatedDT);

		

		
	


}


//spawns asteroid to the simulation following the defined scene logic 
void AGameManager::spawnAsteroidToGame()
{
	FActorSpawnParameters SpawnInfo;
	FRotator myRot(0, 0, 0);



	//random location
	FVector myLoc(0, 0, 0);
	myLoc.X += FMath::FRandRange(-simulationRadius, simulationRadius);
	myLoc.Y += FMath::FRandRange(-simulationRadius, simulationRadius);
	myLoc.Z += FMath::FRandRange(-simulationRadius, simulationRadius);
	myLoc *= 0.9f;
	myLoc += playerRef->GetActorLocation() / 1000.0f; //translate it to desired spawn centre

	//random speed
	FVector speed_ = FVector(0, 0, 0);
	speed_.X += FMath::FRandRange(-1.0f, 1.0f);
	speed_.Y += FMath::FRandRange(-1.0f, 1.0f);
	speed_.Z += FMath::FRandRange(-1.0f, 1.0f);

	speed_.Normalize();
	speed_ *= FMath::FRandRange(SpawnSpeed.X,SpawnSpeed.Y);
	
	//random mass
	float mass_ = 0.001f;
	mass_ += FMath::FRandRange(SpawnMass.X, SpawnMass.Y);

	//name
	FString bodName = "Body ";
	bodName.Append(std::to_string(OverallSpawnerIndex).c_str());



	//assign body's variables
	AAsteroid* newBody = GetWorld()->SpawnActor<AAsteroid>(myLoc * 1000.0f, myRot, SpawnInfo);
	newBody->GravComp = BodyHandler_ref->addGravCompAt(myLoc, speed_, mass_, newBody);

	float scale_ = cbrt(mass_);
	scale_ /= 3.0f;
	//currently more for display purposes
	newBody->SetActorScale3D(FVector(scale_, scale_, scale_));
	
	
	OverallSpawnerIndex++;
}
