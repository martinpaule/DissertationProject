// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraHandlingComponent.generated.h"


class APlayerShip;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HONSPROJECT_API UCameraHandlingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCameraHandlingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	APlayerShip * shipRef;

	void handleCameraLerp(float DeltaTime);

	bool oneHoldFramePassed = false;


	UPROPERTY(Category = "CameraTings", EditAnywhere, BlueprintReadWrite)
	float camDistanceFromRoot = 1000.0f;
	//float camDistanceFromRoot = 1000.0f;

	UPROPERTY(Category = "TESTING", EditAnywhere, BlueprintReadWrite)
		FVector defaultCamOffset;
	UPROPERTY(Category = "CameraTings", EditAnywhere, BlueprintReadWrite)
		FVector nowCamOffset;


	bool movingBacktoDefaultCamPos = false;
	FVector lerpPos_;
	FRotator lerpRot;
	float lerpVal = 0.0f;


	
	UPROPERTY(Category = "TESTING", EditAnywhere, BlueprintReadWrite)
	float camMoveSpeed = 250.0f;
	UPROPERTY(Category = "TESTING", EditAnywhere, BlueprintReadWrite)
	float CamOffsetFromMiddle = 400.0f;


	void MoveShipForward_CamHandle(float AxisValue);
	void MoveShipRight_CamHandle(float AxisValue);

	void RotateShipUp_CamHandle(float AxisValue);

	FVector speedCamOffset;

};
