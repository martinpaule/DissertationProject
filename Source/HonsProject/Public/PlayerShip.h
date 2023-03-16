// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"


#include "CameraHandlingComponent.h"
#include "Camera/CameraComponent.h"
#include "PlayerShip.generated.h"


class UNiagaraSystem;
class UNiagaraComponent;


UCLASS()
class HONSPROJECT_API APlayerShip : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerShip();

	//Gravitation body object necessary component
	UPROPERTY(Category = "BasicComponents", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* SceneComponent;
	UPROPERTY(Category = "BasicComponents", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* StaticMeshComponent;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		UCameraComponent* OurCamera;
	UPROPERTY(Category = "BasicComponents", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* camChaserComp;

	UPROPERTY(Category = "BasicComponents", VisibleAnywhere, BlueprintReadWrite)
		UCameraHandlingComponent* camHandleComp;


	UNiagaraSystem* shipTrail1;
	UNiagaraSystem* WarpDrive;
	UNiagaraComponent* WarpDriveComp;
	UInputComponent* inputCompRef;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



	float shipMoveSpeed = 800.0f;
	float shipRotateSpeed = 40.0f;
	
	float warpStrength = 0.0f;
	bool warping = false;
	float warpTimeLeft = 3.0f;
	
	bool slowingWarp = false;
	float partLife = 0;





public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	//void rotate


	void MoveShipForward(float AxisValue);
	void MoveShipRight(float AxisValue);

	void RotateShipUp(float AxisValue);
	void RotateShipRight(float AxisValue);

	void ChargeWarpSpeed(float AxisValue);


	void RMBrot(float DeltaTime);

	 
};
