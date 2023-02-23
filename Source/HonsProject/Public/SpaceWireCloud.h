// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpaceWireCloud.generated.h"


struct CloudPoint {

	FVector position;
	FVector endPosition;

	FColor color;
};

struct pointHandler {
public:
	TArray<CloudPoint> myPoints;
		float spawnBounds_ = 500;
		float animSpeed_ = 100;

	void updatePointValues(float dt) {
		for (int i = 0; i < myPoints.Num(); i++) {

			FVector moveV = myPoints[i].endPosition - myPoints[i].position;


			if (moveV.Length() < 50) {
				myPoints[i].endPosition = FMath::VRand() * FMath::FRand() * spawnBounds_;
				return;
			}
			moveV.Normalize();
			myPoints[i].position += moveV * animSpeed_ * dt;
		}
	}
};


UCLASS()
class HONSPROJECT_API ASpaceWireCloud : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpaceWireCloud();


	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	USceneComponent* SceneComponent;


	TArray<pointHandler> myHandlers;

	void drawClouds();

	UPROPERTY(Category = "CustomSetters", EditAnywhere, BlueprintReadWrite)
		FVector velocity = FVector(10, 100, -50);

	UPROPERTY(Category = "CustomSetters", EditAnywhere, BlueprintReadWrite)
		float spawnBounds = 500;

	UPROPERTY(Category = "CustomSetters", EditAnywhere, BlueprintReadWrite)
		float animSpeed = 100;

	UPROPERTY(Category = "CustomSetters", EditAnywhere, BlueprintReadWrite)
		int pointCount = 20;
};
