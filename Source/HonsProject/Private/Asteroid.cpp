// Fill out your copyright notice in the Description page of Project Settings.


#include "Asteroid.h"

// Sets default values
AAsteroid::AAsteroid()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	//choose a random mesh out of 4 for the ore
	auto MeshAssetA = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/GreenwoodFantasyVillage/Meshes/SM_RockA.SM_RockA'"));
	auto MeshAssetB = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/GreenwoodFantasyVillage/Meshes/SM_RockB.SM_RockB'"));
	auto MeshAssetC = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/GreenwoodFantasyVillage/Meshes/SM_RockC.SM_RockC'"));
	auto MeshAssetD = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/GreenwoodFantasyVillage/Meshes/SM_RockD.SM_RockD'"));
	UStaticMesh* tempM = nullptr;


	int choiceNR = FMath::RandRange(0, 3);



	switch (choiceNR) {
	case 1:
		if (MeshAssetB.Succeeded()) {
			tempM = MeshAssetB.Object;
			
		}
		break;
	case 2:
		if (MeshAssetC.Succeeded()) {
			tempM = MeshAssetC.Object;
		}
		break;
	case 3:
		if (MeshAssetD.Succeeded()) {
			tempM = MeshAssetD.Object;
		}
		break;
	default:
		if (MeshAssetA.Succeeded()) {
			tempM = MeshAssetA.Object;
		}
		break;
	}
	StaticMeshComponent->SetStaticMesh(tempM);


	myCol = FVector4(1.0f, 0.0f, 1.0f, 1.0f);


	//create random colour on material
	auto Mat_o = StaticMeshComponent->GetMaterial(0);

	myMat = UMaterialInstanceDynamic::Create(Mat_o, NULL);
	StaticMeshComponent->SetMaterial(0, myMat);
	myMat->SetVectorParameterValue(TEXT("OreColour"), myCol);


}




// Called when the game starts or when spawned
void AAsteroid::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AAsteroid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
