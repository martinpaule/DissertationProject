// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GravBody.h"

#include "NBodyHandler.generated.h"

UCLASS()
class HONSPROJECT_API ANBodyHandler : public APawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANBodyHandler();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "SimSpeed")
	void raiseSimulationSpeed();
	UFUNCTION(BlueprintCallable, Category = "SimSpeed")
	void lowerSimulationSpeed();
	UFUNCTION(BlueprintCallable, Category = "SimSpeed")
	void pauseSimulation();
	UFUNCTION(BlueprintCallable, Category = "SimCentre")
	void moveToSimulationCore();

	TArray<AGravBody*> myGravBodies;

	bool mergeGravBodies(); //returns true if no more bodies to merge
	void spawnBodyAt(FVector position, FVector velocity, float mass);
	void graduallySpawnBodies(int spawnsPerFrame = 1);

	long double bigG = 0.000000000066743f;
	bool notPaused = true;

	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
	float timeMultiplier = 1.0f;

	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
	float SimulationElapsedTime = 0.0f;

	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
	int bodiesToSpawn = 20;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
	int SpawnsPerFrame = 5;

	// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent_) override;
	int gradualSpawnerIndex = 0;
	bool spawningBodies = true;
};
