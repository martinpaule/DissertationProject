// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"


#include "CameraHandlingComponent.h"
#include "PlayerMovement.h"

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

	UPROPERTY(Category = "BasicComponents", VisibleAnywhere, BlueprintReadWrite)
		UCameraHandlingComponent* camHandleComp;

	UPROPERTY(Category = "BasicComponents", VisibleAnywhere, BlueprintReadWrite)
		UPlayerMovement *playerMovementComponent;


	UNiagaraSystem* shipTrail1;
	UNiagaraSystem* WarpDrive;
	UNiagaraComponent* WarpDriveComp;

	//necessary components
	UInputComponent* inputCompRef;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;







public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	//void rotate
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveShipForward(float AxisValue);
	void MoveShipRight(float AxisValue);

	void RotateShipUp(float AxisValue);
	void RotateShipRight(float AxisValue);

	void ChargeWarpSpeed(float AxisValue);

	 
};
