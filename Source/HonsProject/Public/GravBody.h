// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include <string>
#include "GravBody.generated.h"


class UMaterialInstanceDynamic;

UCLASS()
class HONSPROJECT_API AGravBody : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGravBody();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



	//Gravitation body object necessary component
	UPROPERTY(Category = "BasicComponents", VisibleAnywhere, BlueprintReadWrite)
	USceneComponent * SceneComponent;
	UPROPERTY(Category = "BasicComponents", VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent * StaticMeshComponent;
	UPROPERTY(Category = "BasicComponents", VisibleAnywhere, BlueprintReadWrite)
	UMaterialInstanceDynamic* myMat;


	//simulation variables
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
	FVector velocity = FVector(0.0f, 0.0f, 0.0f); //km/s
	FVector position = FVector(0.0f, 0.0f, 0.0f); //km/s
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
	double mass = 1; //kg
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
	double radius = 1; //km
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
	bool toBeDestroyed = false;


	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		FVector4 myCol;

};
