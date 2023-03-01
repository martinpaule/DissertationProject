// Fill out your copyright notice in the Description page of Project Settings.
#include "GravBody.h"
#include "Misc/DateTime.h"
#include "Math/UnrealMathVectorCommon.h"
#include "NBodyHandler.h"

// Sets default values
AGravBody::AGravBody()
{
	//create root component, collider and static mesh
	PrimaryActorTick.bCanEverTick = true;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshCompopnent"));
	StaticMeshComponent->SetupAttachment(SceneComponent);

	
	FVector4 randCol = FVector4(0.0f, 0.0f, 1.0f, 1.0f);
	randCol.X = FMath::FRandRange(0.0f, 1.0f);
	randCol.Y = FMath::FRandRange(0.0f, 1.0f);
	randCol.Z = FMath::FRandRange(0.0f, 1.0f);


	myCol = randCol;

}



// Called when the game starts or when spawned
void AGravBody::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGravBody::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

