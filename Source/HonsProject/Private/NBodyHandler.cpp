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
void UNBodyHandler::calculateWithTree(double dt, bool calculateError, bool newTrees) {


	if (!newTrees) {
		treeHandlerRef->RecalculatePartitioning();
	}

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

	



	

}

void UNBodyHandler::moveBodies(bool alsoMoveActor, double updated_dt) {

	for (int i = 0; i < myGravBodies.Num(); i++) {
		//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, "moving");

		myGravBodies[i]->position += updated_dt * myGravBodies[i]->velocity;

		if (alsoMoveActor) {
			myGravBodies[i]->GetOwner()->SetActorLocation(myGravBodies[i]->position * 1000.0f);
				//SetActorLocation(myGravBodies[i]->position * 1000.0f);
		}
	}

}
