// Fill out your copyright notice in the Description page of Project Settings.


#include "NBody_Handler.h"
#include <Kismet/GameplayStatics.h>
#include "Misc/DateTime.h"
#include "GameFramework/DefaultPawn.h"
#include <Kismet/KismetMathLibrary.h>
#include <fstream>
#include "DrawDebugHelpers.h"


DECLARE_CYCLE_STAT(TEXT("Direct Integration (Grav Calc)"), STAT_MovingWithDI, STATGROUP_NBodyHandler);
DECLARE_CYCLE_STAT(TEXT("Tree Codes (Grav Calc)"), STAT_MovingWithTree, STATGROUP_NBodyHandler);


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

//initialize function for TH to register as component
void UNBodyHandler::constructTreeHandler() {


	FTransform tr;
	tr.SetIdentity();
	//create tree code handler
	treeHandler = Cast<UTreeHandler>(GetOwner()->AddComponentByClass(UTreeHandler::StaticClass(), false, tr, true));
	treeHandler->RegisterComponent();
	treeHandler->bodyHandlerBodies = &myGravBodies;
	treeHandler->drawDebugs = drawDebugs;

}

//add a new body into the handler
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

	SCOPE_CYCLE_COUNTER(STAT_MovingWithDI);


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
void UNBodyHandler::calculateWithTree(double dt) {

	////time taken code
	//auto startDI = std::chrono::high_resolution_clock::now();
	//treeHandler->RecalculatePartitioning(newTrees);
	//auto stopDI = std::chrono::high_resolution_clock::now();
	//float msTakenCALCTC = std::chrono::duration_cast<std::chrono::microseconds>(stopDI - startDI).count();
	//
	//timeTakenTotal += msTakenCALCTC;
	//totalFramesPassed += 1.0f;
	//
	//std::string printStr = "Average Time taken to recalculate tree with NewTrees(" + std::to_string(newTrees) + ") = " + std::to_string(timeTakenTotal / totalFramesPassed);;
	//
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Orange, printStr.c_str());


	SCOPE_CYCLE_COUNTER(STAT_MovingWithTree);


	treeHandler->gravCalcs = 0;


	for (int i = 0; i < myGravBodies.Num(); i++)
	{
		
		myGravBodies[i]->velocity += myGravBodies[i]->myLocalTimeEditor * dt * treeHandler->getApproxForce(myGravBodies[i], treeHandler->treeNodeRoot);

	}

}

// Called every frame
void UNBodyHandler::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	



	

}

//apply new velocity to bodies
void UNBodyHandler::moveBodies(bool alsoMoveActor, double updated_dt) {

	for (int i = 0; i < myGravBodies.Num(); i++) {
		//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, "moving");

		myGravBodies[i]->position += myGravBodies[i]->myLocalTimeEditor * updated_dt * myGravBodies[i]->velocity;

		if (alsoMoveActor) {
			myGravBodies[i]->GetOwner()->SetActorLocation(myGravBodies[i]->position * 1000.0f);
		}
	}

}
