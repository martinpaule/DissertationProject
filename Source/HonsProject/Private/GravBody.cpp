// Fill out your copyright notice in the Description page of Project Settings.


#include "GravBody.h"
// Sets default values
AGravBody::AGravBody()
{
	//create root component and static mesh
	PrimaryActorTick.bCanEverTick = true;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshCompopnent"));
	StaticMeshComponent->SetupAttachment(SceneComponent);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/MyHonsContent/PlanetMesh.PlanetMesh'"));
	UStaticMesh* Asset = MeshAsset.Object;

	StaticMeshComponent->SetStaticMesh(Asset);

	
	//myMat = CreateDefaultSubobject<UMaterialInstanceDynamic>(TEXT("MaterialInstanceDynamic"));



	auto Mat_o = StaticMeshComponent->GetMaterial(0);

	myMat = UMaterialInstanceDynamic::Create(Mat_o, NULL);
	StaticMeshComponent->SetMaterial(0, myMat);

	FVector4 randCol = FVector4(0.0f, 0.0f, 1.0f, 1.0f);
	randCol.X = FMath::FRandRange(0.0f, 1.0f);
	randCol.Y = FMath::FRandRange(0.0f, 1.0f);
	randCol.Z = FMath::FRandRange(0.0f, 1.0f);

	myMat->SetVectorParameterValue(TEXT("Colour"), randCol);

	
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

void AGravBody::MoveBody(float dt, float timeMultiplier)
{
	SceneComponent->AddWorldOffset(speed*dt*timeMultiplier);
}



void AGravBody::spawnSetup(FVector initialSpeed, float bodyMass) {

	

	mass = bodyMass;
	speed = initialSpeed;

	float scale_ = cbrt(bodyMass);

	this->SetActorScale3D(FVector(scale_, scale_, scale_));
}