// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceWireCloud.h"

// Sets default values
ASpaceWireCloud::ASpaceWireCloud()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);





}

// Called when the game starts or when spawned
void ASpaceWireCloud::BeginPlay()
{
	Super::BeginPlay();

	for (int a = 0; a <= 17; a++) {

		pointHandler tempH;
		tempH.animSpeed_ = animSpeed;
		tempH.spawnBounds_ = spawnBounds;

		FColor triCol;

		triCol.R = FMath::Rand() % 256;
		triCol.G = FMath::Rand() % 256;
		triCol.B = FMath::Rand() % 256;

		for (int i = 0; i < 3; i++) {
			CloudPoint cp_temp;
			cp_temp.position = FMath::VRand() * FMath::FRand() * spawnBounds;
			cp_temp.endPosition = FMath::VRand() * FMath::FRand() * spawnBounds;
			cp_temp.color = triCol;
			


			tempH.myPoints.Add(cp_temp);
		}

		myHandlers.Add(tempH);
	}
}


void ASpaceWireCloud::drawClouds() {

	for (int a = 0; a < myHandlers.Num(); a++) {
		for (int i = 0; i < myHandlers[a].myPoints.Num(); i++)
		{
			for (int j = i + 1; j < myHandlers[a].myPoints.Num(); j++)
			{
				DrawDebugLine(GetWorld(), GetActorLocation() + myHandlers[a].myPoints[i].position, GetActorLocation() + myHandlers[a].myPoints[j].position, myHandlers[a].myPoints[i].color, false, 0, 0, 3);
			}
		}
	}
	

}




// Called every frame
void ASpaceWireCloud::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorWorldOffset(velocity * DeltaTime);


	for (int i = 0; i < myHandlers.Num(); i++) {
		myHandlers[i].updatePointValues(DeltaTime);
	}

	//draw
	drawClouds();
}

