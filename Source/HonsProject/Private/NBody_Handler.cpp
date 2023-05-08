// Fill out your copyright notice in the Description page of Project Settings.


#include "NBody_Handler.h"
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

TArray<FVector> UNBodyHandler::getThrowLine(FVector startingPos, FVector velocity, int segments, float timeStep)
{
	TArray<FVector> returnArray;
	

	FVector Pos_now = startingPos;
	FVector Vel_now = velocity;

	returnArray.Add(Pos_now);


	//declarations
	double distance = 0.0f;
	double distanceCubed = 0.0f;
	FVector direction = FVector(0.0f, 0.0f, 0.0f);
	FVector iteratedBodyForce = FVector(0.0f, 0.0f, 0.0f);
	FVector sumOfForces = FVector(0.0f, 0.0f, 0.0f);
	FVector deltaVelocity = FVector(0.0f, 0.0f, 0.0f);
	FVector deltaDist = FVector(0.0f, 0.0f, 0.0f);


	for (int i = 0; i < segments; i++) {


		
		sumOfForces = FVector(0.0f, 0.0f, 0.0f);

		//calulate combined forces acting on body I
		for (int j = 0; j < myGravBodies.Num(); j++)
		{
			direction = myGravBodies[j]->position - Pos_now;
			distance = direction.Length();
			distanceCubed = distance * distance * distance;
			iteratedBodyForce = direction * bigG * myGravBodies[j]->mass;
			iteratedBodyForce /= distanceCubed;

			//add this to the sum of forces acting on body I
			sumOfForces += iteratedBodyForce;
			gravCalculations++;
		}

		//apply change in velocity to body I
		deltaVelocity = timeStep * sumOfForces;
		Vel_now += deltaVelocity;



		//apply imagined time step and add to array
		deltaDist = timeStep * Vel_now;
		Pos_now += deltaDist;
		returnArray.Add(Pos_now);

	}

	return returnArray;
}




// Called when the game starts or when spawned
void UNBodyHandler::BeginPlay()
{
	Super::BeginPlay();


	
	
	

	
}

void UNBodyHandler::constructTreeHandler() {


	FTransform tr;
	tr.SetIdentity();
	//create tree code handler
	treeHandler = Cast<UTreeHandler>(GetOwner()->AddComponentByClass(UTreeHandler::StaticClass(), false, tr, true));
	treeHandler->RegisterComponent();
	treeHandler->bodyHandlerBodies = &myGravBodies;
	treeHandler->drawDebugs = drawDebugs;

}

UGravBodyComponent * UNBodyHandler::addGravCompAt(FVector position, FVector velocity, double mass, AActor * gravCompOwner)
{

	FTransform tr;
	tr.SetIdentity();

	//create Nbody handler
	UGravBodyComponent* GravComp = Cast<UGravBodyComponent>(gravCompOwner->AddComponentByClass(UGravBodyComponent::StaticClass(), true, tr, true));
	GravComp->RegisterComponent();

	GravComp->toBeDestroyed = false;
	GravComp->position = position;
	GravComp->velocity = velocity;
	GravComp->mass = mass;

	myGravBodies.Add(GravComp);

	return GravComp;

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
		deltaVelocity = myGravBodies[i]->myLocalTimeEditor* dt* sumOfForces;
		myGravBodies[i]->velocity += deltaVelocity;

	}
}


//tree code calculations of gravitational dynamics
void UNBodyHandler::calculateWithTree(double dt, bool calculateError, bool newTrees) {

	////time taken code
	//auto startDI = std::chrono::high_resolution_clock::now();
	treeHandler->RecalculatePartitioning(newTrees);
	//auto stopDI = std::chrono::high_resolution_clock::now();
	//float msTakenCALCTC = std::chrono::duration_cast<std::chrono::microseconds>(stopDI - startDI).count();
	//
	//timeTakenTotal += msTakenCALCTC;
	//totalFramesPassed += 1.0f;
	//
	//std::string printStr = "Average Time taken to recalculate tree with NewTrees(" + std::to_string(newTrees) + ") = " + std::to_string(timeTakenTotal / totalFramesPassed);;
	//
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Orange, printStr.c_str());



	treeHandler->gravCalcs = 0;

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
		TreeSumOfForces = treeHandler->getApproxForce(myGravBodies[i], treeHandler->treeNodeRoot);
		myGravBodies[i]->velocity += myGravBodies[i]->myLocalTimeEditor * dt * TreeSumOfForces;

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

	



	

}

void UNBodyHandler::moveBodies(bool alsoMoveActor, double updated_dt) {

	for (int i = 0; i < myGravBodies.Num(); i++) {
		//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, "moving");

		myGravBodies[i]->position += myGravBodies[i]->myLocalTimeEditor * updated_dt * myGravBodies[i]->velocity;

		if (alsoMoveActor) {
			myGravBodies[i]->GetOwner()->SetActorLocation(myGravBodies[i]->position * 1000.0f);
				//SetActorLocation(myGravBodies[i]->position * 1000.0f);
		}
	}

}
