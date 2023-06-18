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
	// ----- Sets default values for this actor's properties
	UNBodyHandler();

	// ----- array holding a reference to all bodies
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		TArray<UGravBodyComponent*> myGravBodies;
	
	// ----- tree code handler reference
	UPROPERTY(Category = "SimulationRelevant", BlueprintReadWrite)
		UTreeHandler* treeHandler;

	// ----- misc variables
	UPROPERTY(Category = "forUI", BlueprintReadWrite)
		int gravCalculations = 0;
	//double bigG = 0.000000000066743f; //when using kg,m and s
	double bigG = 39.4784f; //when using SolarMass, AU and Years
	int handlerID = 0;
	bool drawDebugs = true;

protected:
	// ----- Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// ----- Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// ----- init function
	UFUNCTION(BlueprintCallable, Category = "Debugging")
		void constructTreeHandler();
	

	// ----- direct integration of gravitational calculations
	void calculateAllVelocityChanges(double dt);
	// ----- tree codes calculaton
	void calculateWithTree(double dt);
	// ----- moving 
	void moveBodies(bool alsoMoveActor, double updated_dt);
	
	// ----- body spawning
	UFUNCTION(BlueprintCallable, Category = "AddBody")
		UGravBodyComponent* addGravCompAt(FVector position, FVector velocity, double mass, AActor* gravCompOwner);
	
};
