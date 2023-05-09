// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UObject/ConstructorHelpers.h"
#include <string>
#include "Tree_Handler.h"
#include "GravBody_Component.h"
#include "NBody_Handler.generated.h"


UCLASS()
class HONSPROJECT_API UNBodyHandler : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UNBodyHandler();
	UFUNCTION(BlueprintCallable, Category = "Debugging")
		TArray<FVector> getThrowLine(FVector startingPos, FVector velocity, int segments = 50, float timeStep = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Debugging")
		void constructTreeHandler();
	UFUNCTION(BlueprintCallable, Category = "AddBody")
		UGravBodyComponent * addGravCompAt(FVector position, FVector velocity, double mass, AActor * gravCompOwner);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	//array holding a reference to all bodies
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
	TArray<UGravBodyComponent*> myGravBodies;


	//tree code handler reference
	UPROPERTY(Category = "SimulationRelevant", BlueprintReadWrite)
	UTreeHandler* treeHandler;


	// ----- Cursor relevant variables
	void moveBodies(bool alsoMoveActor, double updated_dt);

	//direct integration of gravitational calculations
	void calculateAllVelocityChanges(double dt);
	//tree codes calculaton
	void calculateWithTree(double dt);

	//double bigG = 0.000000000066743f; //when using kg,m and s
	double bigG = 39.4784f; //when using SolarMass, AU and Years

	UPROPERTY(Category = "SimulationRelevant", BlueprintReadWrite)
		bool useTreeCodes_ = false;

	////calculate average error of change in velocity
	//UPROPERTY(Category = "SimulationRelevant", BlueprintReadWrite)
	//	float VelCalcAverageError = 0.0f;
	
	int handlerID = 0;

	//fixed frame time logic
	//UPROPERTY(Category = "SimulationRelevant", BlueprintReadWrite, EditAnywhere)
	//	float fixedFrameTime = 0.1f;
	//float elapsedFrameTime = 0.0f;

	//simulation data incrementors
	//float timeTakenTotal = 0.0f;
	//float totalFramesPassed = 0.0f;
	UPROPERTY(Category = "forUI", BlueprintReadWrite)
		int gravCalculations = 0;
	bool drawDebugs = true;
};
