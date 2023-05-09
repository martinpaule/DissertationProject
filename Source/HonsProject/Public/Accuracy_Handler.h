// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <fstream>
#include "TestPlanet.h"
#include <string>

#include "Accuracy_Handler.generated.h"

//simple struct holding all the relevant data of a gravitational body
struct planet {
public:
	std::string name;
	FVector pos;
	FVector vel;
	float mass;

};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class HONSPROJECT_API UAccuracyModule : public UActorComponent
{
	GENERATED_BODY()

public:

	// Sets default values for this actor's properties
	UAccuracyModule();

	// ----- default vars
	TArray<UGravBodyComponent*>* bodyHandlerBodies;
	TArray<TArray<planet>> planets;
	int outputIndex = 0;

	// ----- testing
	bool shouldResetTest = false;
	float resetTime = 1.0f; //in years

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	
	//Testing functions
	UFUNCTION(BlueprintCallable, Category = "Recording")
	void printResultToTXT();
	UFUNCTION(BlueprintCallable, Category = "Recording")
	void recordPositions();
	bool planetsEqual(planet a, planet b);
	void notePlanet(std::string name_, FVector pos_, FVector vel_, float mass_);

	
};
