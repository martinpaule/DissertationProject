// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <string>
#include "PlayerMovement.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HONSPROJECT_API UPlayerMovement : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerMovement();



	//base movement variables
	float MoveSpeed = 800.0f;
	float RotateSpeed = 40.0f;

	//warp varables
	float warpStrength = 0.0f;
	bool warping = false;
	float warpTimeLeft = 3.0f;
	bool slowingWarp = false;

	AActor* myOwner;

	float partLife = 0;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);

	void RotateUp(float AxisValue);
	void RotateRight(float AxisValue);

	void ChargeWarp(float AxisValue);

	void handleMovement(float dt);
};
