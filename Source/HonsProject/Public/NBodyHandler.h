// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
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
	void raiseSimulationSpeed();
	void lowerSimulationSpeed();
	void moveToSimulationCore(float keyDown);
	long double bigG = 0.000000000066743f;
	bool mergeGravBodies(); //returns true if no more bodies to merge
	float timeMultiplier = 1.0f;

	UPROPERTY(Category = "myCategorhhy", EditAnywhere, BlueprintReadWrite)
	int bodiesToSpawn = 20;
	// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent_) override;

};
