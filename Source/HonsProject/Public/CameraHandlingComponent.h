// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
//#include <GameFramework/>

#include "Camera/CameraComponent.h"

#include "CameraHandlingComponent.generated.h"


class APlayerShip;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HONSPROJECT_API UCameraHandlingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCameraHandlingComponent();

	UPROPERTY(Category = "BasicComponents", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* camChaserComp;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		UCameraComponent* OurCamera;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
};
