// Fill out your copyright notice in the Description page of Project Settings.


#include "NBodyHandler.h"
#include <Kismet/GameplayStatics.h>
#include "Misc/DateTime.h"
#include "GameFramework/DefaultPawn.h"
#include <Kismet/KismetMathLibrary.h>
#include <fstream>
#include "DrawDebugHelpers.h"

// Sets default values
UNBodyHandler::UNBodyHandler()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryComponentTick.bCanEverTick = true;

	
	
}




// Called when the game starts or when spawned
void UNBodyHandler::BeginPlay()
{
	Super::BeginPlay();


	
	//treeHandlerRef->shouldCalculateTC = useTreeCodes;

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
	//		&UNBodyHandler::moveToSimulationCore // The function that will fire when input is received
	//	);
	//	EnableInput(GetWorld()->GetFirstPlayerController());
	//}


	//add manually placed bodies to the array
	//TArray<AActor*> FoundActors;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGravBody::StaticClass(), FoundActors);	
	//for (int n = 0; n < FoundActors.Num(); n++) {
	//	AGravBody* ref = Cast<AGravBody>(FoundActors[n]);
	//	myGravBodies.Add(ref);
	//}

	
}

//direct integration of gravitational dynamics using Newtonian formulae
void UNBodyHandler::calculateAllVelocityChanges(double dt) {

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
void UNBodyHandler::calculateWithTree(double dt, bool calculateError) {



	treeHandlerRef->RecalculatePartitioning();


	treeHandlerRef->gravCalcs = 0;

	FVector TreeSumOfForces = FVector(0.0f, 0.0f, 0.0f);
	FVector RealSumOfForces = FVector(0.0f, 0.0f, 0.0f);

	VelCalcAverageError = 0.0f;


	//declarations
	double distance = 0.0f;
	double distanceCubed = 0.0f;
	FVector direction = FVector(0.0f, 0.0f, 0.0f);
	FVector iteratedBodyForce = FVector(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < myGravBodies.Num(); i++)
	{
		

		//apply change in velocity to body I
		TreeSumOfForces = treeHandlerRef->getApproxForce(myGravBodies[i],treeHandlerRef->treeNodeRoot);
		myGravBodies[i]->velocity += dt * TreeSumOfForces;

		if (calculateError) {
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
					RealSumOfForces += iteratedBodyForce;
				}
			}
			
			VelCalcAverageError += (RealSumOfForces - TreeSumOfForces).Length();
		}

	}
	if (calculateError) {
		VelCalcAverageError /= myGravBodies.Num();

	}


}

// Called every frame
void UNBodyHandler::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	



	//gradually spawn bodies to avoid a large lag spike at the start
	if(spawningBodies)
	{
		graduallySpawnBodies(SpawnsPerFrame_);
	}

	

}

void UNBodyHandler::moveBodies(bool alsoMoveActor, double updated_dt) {

	for (int i = 0; i < myGravBodies.Num(); i++) {
		myGravBodies[i]->position += updated_dt * myGravBodies[i]->velocity;

		if (alsoMoveActor) {
			myGravBodies[i]->SetActorLocation(myGravBodies[i]->position * 1000.0f);
		}
	}

}

// setup function for spawning bodies - creates a new body with specified parameters
void UNBodyHandler::spawnBodyAt(FVector position_, FVector velocity_, double mass_, std::string name_, float radius_, FVector4 colour_)
{

	FActorSpawnParameters SpawnInfo;
	FRotator myRot(0, 0, 0);

	//assign body's variables
	AGravBody * newBody = GetWorld()->SpawnActor<AGravBody>(position_*1000.0f, myRot, SpawnInfo);
	//newBody->SetActorEnableCollision(true);
	newBody->velocity = velocity_;
	newBody->mass = mass_;
	newBody->position = position_;
	newBody->radius = radius_;
	newBody->toBeDestroyed = false;
	newBody->SetActorLabel(name_.c_str());

	if (radius_ == 0.0f) {
		radius_ = cbrt(mass_);
	}
	//currently more for display purposes
	newBody->SetActorScale3D(FVector(radius_, radius_, radius_));

	//option to set colour too
	if (colour_ != FVector4(0.0f, 0.0f, 0.0f, 0.0f)) {
		newBody->myMat->SetVectorParameterValue(TEXT("Colour"), colour_);
		newBody->myCol = colour_;
	}
	myGravBodies.Add(newBody);



	//newBody->SetActorEnableCollision(true);


}

void UNBodyHandler::spawnSolarSystem(int SolarPlanetToSpawn) {


	//define and spawn the sun
	FVector bodyONE_pos = FVector(-9.0841f * pow(10, -3), 4.9241f * pow(10, -4), 2.0754f * pow(10, -4));
	float massOne = 1.0f;
	spawnBodyAt(bodyONE_pos + SpawnCentre /2.0f, FVector(0.0f, 0.0f, 0.0f), 1.0f, "Sun", 5.0f,FVector4(1.0f,1.0f,0.0f,1.0f));


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
		spawnBodyAt(bodyTWO_pos + SpawnCentre /2, RightVel, massTwo, bodyTwoName_, bodyTwoScale,planetColor);
	}

}

void UNBodyHandler::spawnTestPlanets()
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
void UNBodyHandler::graduallySpawnBodies(int spawnsPerFrame) {


	for (int s = 0; s < spawnsPerFrame; s++) { // spawn all the bodies for this frame

		//Exi the spawning loop
		if (gradualSpawnerIndex >= bodiesToSpawn) {
			if (handlerID == 1) {
				return;
			}
			spawningBodies = false;
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, "Spawned all desired bodies");
			if (bSpawnSolarSystem) {
				spawnSolarSystem(solarPlanetToSpawn);
			}
			if (bSpawnTestPlanets) {
				spawnTestPlanets();
			}

			return;
		}

		//random location
		FVector myLoc(-spawnExtent / 2, -spawnExtent / 2, -spawnExtent / 2);
		myLoc.X += FMath::FRandRange(0, spawnExtent);
		myLoc.Y += FMath::FRandRange(0, spawnExtent);
		myLoc.Z += FMath::FRandRange(0, spawnExtent);
		myLoc += SpawnCentre; //translate it to desired spawn centre

		//random speed
		FVector speed_ = FVector(-SpawnMaxSpeed / 2, -SpawnMaxSpeed / 2, -SpawnMaxSpeed / 2);
		speed_.X += FMath::FRandRange(0, SpawnMaxSpeed);
		speed_.Y += FMath::FRandRange(0, SpawnMaxSpeed);
		speed_.Z += FMath::FRandRange(0, SpawnMaxSpeed);

		//random mass
		float mass_ = 0.001f;
		mass_ += FMath::FRandRange(0.0f, SpawnMaxMass);

		std::string bodName = "Body ";
		bodName += std::to_string(gradualSpawnerIndex);
		spawnBodyAt(myLoc, speed_, mass_, bodName);
		gradualSpawnerIndex++;

	}

	return;
}


void UNBodyHandler::doubleAllScales() {
	for (int i = 0; i < myGravBodies.Num(); i++)
	{
		float scale_ = myGravBodies[i]->GetActorScale().X;
		myGravBodies[i]->SetActorScale3D(FVector(scale_*2, scale_*2, scale_*2));

	}
}



//Work in progress
void UNBodyHandler::RecentreSimulation() {


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

