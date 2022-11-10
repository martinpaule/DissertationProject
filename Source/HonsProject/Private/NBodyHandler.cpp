// Fill out your copyright notice in the Description page of Project Settings.


#include "NBodyHandler.h"
#include <Kismet/GameplayStatics.h>
#include <string>
#include "Misc/DateTime.h"
#include "GameFramework/DefaultPawn.h"
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
	ADefaultPawn * pawn_ = Cast<ADefaultPawn>(UGameplayStatics::GetActorOfClass(GetWorld(), ADefaultPawn::StaticClass()));
	UInputComponent * inp_ = pawn_->InputComponent;
	if (inp_)
	{

		// Bind an action to it
		//inp_->BindAxis
		//(
		//	"MoveToSimMid", // The input identifier (specified in DefaultInput.ini)
		//	this, // The object instance that is going to react to the input
		//	&ANBodyHandler::moveToSimulationCore // The function that will fire when input is received
		//);
		//EnableInput(GetWorld()->GetFirstPlayerController());
	}


	//random initial spawn
	//spawnBodyAt(FVector(-20000, 0, 0), FVector(0, 0, 0), 20000);
	

	if (bodiesToSpawn == 0) {
		spawningBodies = false;
	}

}

//TODO ad actual collision
bool ANBodyHandler::mergeGravBodies()
{
	//TArray<AActor*> myGravBodies;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGravBody::StaticClass(), myGravBodies);
		
	for(int i = 0; i < myGravBodies.Num();i++)
	{
		for(int j = 0; j < myGravBodies.Num();j++)
		{
			if(i != j)
			{
				FVector distance = myGravBodies[j]->GetActorLocation() - myGravBodies[i]->GetActorLocation();
				float length_ = distance.Length();
				float combineThreshHold = (myGravBodies[i]->GetActorScale3D().X + myGravBodies[j]->GetActorScale3D().X)*45;
				if(length_ < combineThreshHold)
				{
					AGravBody * bodyAref = myGravBodies[i];
					AGravBody * bodyBref = myGravBodies[j];
					//a = 200
					//b = 10
					float speedMultOnA = 0.0f;
					float speedMultOnB = 0.0f;

					if (bodyAref->mass > bodyBref->mass) {
						speedMultOnA = 1.0f;
						speedMultOnB = bodyBref->mass / bodyAref->mass;
					}
					else {
						speedMultOnB = 1.0f;
						speedMultOnA = bodyAref->mass / bodyBref->mass;
						bodyAref->SetActorLocation(bodyBref->GetActorLocation());
					}

					bodyAref->speed = bodyAref->speed * speedMultOnA + bodyBref->speed * speedMultOnB;

					bodyAref->mass += bodyBref->mass;
					float scale_ = cbrt(bodyAref->mass);
					myGravBodies[i]->SetActorScale3D(FVector(scale_,scale_,scale_));

					myGravBodies.RemoveAt(j);
					bodyBref->Destroy();
					FDateTime nowTime = FDateTime::Now();
					std::string printStr = "(";
					printStr += std::to_string(nowTime.GetHour()) + ":" + std::to_string(nowTime.GetMinute()) + ":" + std::to_string(nowTime.GetSecond()) + "." + std::to_string(nowTime.GetMillisecond())+ ") MERGED 2 BODIES";
					GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, printStr.c_str());
					return false;
				}
			}
		}
	}

	return true;
}
// Called every frame
void ANBodyHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(spawningBodies)
	{
		
		graduallySpawnBodies(SpawnsPerFrame);
	}
	else if(notPaused) {

		float updatedDT = DeltaTime * timeMultiplier;

		SimulationElapsedTime += updatedDT;

		//calculate acceleration
		for (int i = 0; i < myGravBodies.Num(); i++)
		{
			FVector sumOfForces = FVector(0.0f, 0.0f, 0.0f);
			for (int j = 0; j < myGravBodies.Num(); j++)
			{
				if (i != j)
				{
					FVector distance = myGravBodies[j]->GetActorLocation() - myGravBodies[i]->GetActorLocation();
					float length_ = distance.Length();
					sumOfForces += myGravBodies[j]->mass * distance / length_ * length_ * length_;
				}
			}
			myGravBodies[i]->speed += bigG * (sumOfForces) / myGravBodies[i]->mass;
		}

		//apply updated velocity
		for (int i = 0; i < myGravBodies.Num(); i++)
		{
			myGravBodies[i]->MoveBody(updatedDT);
		}


		//handle merging -- TODO: move into tick of gravbody
		bool FinishedMerging = false;

		while (!FinishedMerging)
		{
			FinishedMerging = mergeGravBodies();
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


void ANBodyHandler::moveToSimulationCore() {
	

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, resultMoveCect.ToString());
	FVector dispVector = FVector(0, 0, 0);


	for (int i = 0; i < myGravBodies.Num(); i++)
	{
		dispVector += myGravBodies[i]->GetActorLocation();
	}
	dispVector /= myGravBodies.Num();

	FVector camToSimVector = dispVector - UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCameraManager::StaticClass())->GetActorLocation();


	APlayerCameraManager* CameraManagerRef = Cast<APlayerCameraManager>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCameraManager::StaticClass()));
	auto POV_ = CameraManagerRef->GetCameraCachePOV();
	POV_.Rotation = camToSimVector.GetSafeNormal().Rotation();
	CameraManagerRef->SetCameraCachePOV(POV_);



	dispVector *= -1;

	UGameplayStatics::GetActorOfClass(GetWorld(), ADefaultPawn::StaticClass())->AddActorWorldOffset(dispVector);


	for (int i = 0; i < myGravBodies.Num(); i++)
	{
		myGravBodies[i]->AddActorWorldOffset(dispVector);
	}
}

// Called when the game starts or when spawned
void ANBodyHandler::spawnBodyAt(FVector position, FVector velocity, float mass)
{

	FActorSpawnParameters SpawnInfo;
	FRotator myRot(0, 0, 0);
	//FVector myLoc(0, 0, 0);


	AGravBody* newBody = GetWorld()->SpawnActor<AGravBody>(position, myRot, SpawnInfo);
	newBody->spawnSetup(velocity, mass);

	myGravBodies.Add(newBody);

}

void ANBodyHandler::graduallySpawnBodies(int spawnsPerFrame) {

	int spawnBounds = 50000;

	for (int s = 0; s < spawnsPerFrame; s++) {

		FVector myLoc(-20000 - spawnBounds / 2, -spawnBounds / 2, -spawnBounds / 2);
		myLoc.X += FMath::RandRange(0, spawnBounds);
		myLoc.Y += FMath::RandRange(0, spawnBounds);
		myLoc.Z += FMath::RandRange(0, spawnBounds);


		int speedBounds = 200;

		FVector speed_ = FVector(-speedBounds / 2, -speedBounds / 2, -speedBounds / 2);
		speed_.X += FMath::RandRange(0, speedBounds);
		speed_.Y += FMath::RandRange(0, speedBounds);
		speed_.Z += FMath::RandRange(0, speedBounds);

		float mass_ = FMath::FRandRange(0.0f, 300.0f);

		spawnBodyAt(myLoc, speed_, mass_);

		gradualSpawnerIndex++;
		if (gradualSpawnerIndex >= bodiesToSpawn) {
			spawningBodies = false;
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, "Spawned all desired bodies");
			return;
		}
	}

	
}