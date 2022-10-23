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
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	UStaticMesh* Asset = MeshAsset.Object;

	StaticMeshComponent->SetStaticMesh(Asset);

	UPrimitiveComponent * aa = this->FindComponentByClass<UPrimitiveComponent>();
	aa->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	
	//speed = FVector(float(-1000 + rand()%2000)/10,float(-1000 + rand()%2000)/10,float(-1000 + rand()%2000)/10);
	speed = FVector(-500,-500,-500);
	speed.X += rand()%1000;
	speed.Y += rand()%1000;
	speed.Z += rand()%1000;
	
	mass = rand()%300;

	float scale_ = cbrt(mass);

	this->SetActorScale3D(FVector(scale_, scale_, scale_));
	
	////FString TheFloatStr = FString::SanitizeFloat(bigG*1000000000000000);
	//std::ostringstream strs;
	//strs << bigG;
	//std::string str = strs.str();
	////std::string bigG_str = boos
	//GEngine->AddOnScreenDebugMessage(-1,  5.0f, FColor::Yellow, str.c_str());	
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

void AGravBody::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1,  5.0f, FColor::Yellow, "OVERLAPPED");	
}