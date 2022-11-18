// Fill out your copyright notice in the Description page of Project Settings.
#include "GravBody.h"
#include "Misc/DateTime.h"
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

	ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/MyHonsContent/PlanetMesh.PlanetMesh'"));
	UStaticMesh* Asset = MeshAsset.Object;
	

	StaticMeshComponent->SetStaticMesh(Asset);
	StaticMeshComponent->SetGenerateOverlapEvents(false);


	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	SphereCollider->SetupAttachment(SceneComponent);
	SphereCollider->InitSphereRadius(45.0f);
	SphereCollider->SetCollisionResponseToAllChannels(ECR_Overlap);

	//create random colour on material
	auto Mat_o = StaticMeshComponent->GetMaterial(0);

	myMat = UMaterialInstanceDynamic::Create(Mat_o, NULL);
	StaticMeshComponent->SetMaterial(0, myMat);

	FVector4 randCol = FVector4(0.0f, 0.0f, 1.0f, 1.0f);
	randCol.X = FMath::FRandRange(0.0f, 1.0f);
	randCol.Y = FMath::FRandRange(0.0f, 1.0f);
	randCol.Z = FMath::FRandRange(0.0f, 1.0f);

	myMat->SetVectorParameterValue(TEXT("Colour"), randCol);
	
	

}

//bind overlap function
void AGravBody::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AGravBody::combineCollisionBody);

}


//perfectly inelastic
// p = momentum. p = m*v.
// p_ = resulting momentum, v_ = resulting velocity
//p1+p2=p_1+p_2			        
//m1*v1 + m2*v2 =m1*v_1 + m2*v_2
//v_ = (m1*v1 + m2*v2)/(m1 + m2)
void AGravBody::combineCollisionBody(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	
	if (Cast<AGravBody>(OtherActor))
	{

		//print message with timestamp, development feature, remove at the end
		FDateTime nowTime = FDateTime::Now();
		std::string printStr = "(";
		printStr += std::to_string(nowTime.GetHour()) + ":" + std::to_string(nowTime.GetMinute()) + ":" + std::to_string(nowTime.GetSecond()) + "." + std::to_string(nowTime.GetMillisecond()) + ") MERGED 2 BODIES";
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, printStr.c_str());

		//perfectly inelastic collision
		AGravBody * otherBody = Cast<AGravBody>(OtherActor);

		float massA = mass;
		float massB = otherBody->mass;
		FVector velocityA = velocity;
		FVector velocityB = otherBody->velocity;
		FVector finalVelocity = (massA * velocityA + massB * velocityB) / (massA + massB);
		
		//add the smaller body's mass and speed to the larger one
		if (otherBody->mass >= mass) {
			otherBody->velocity = finalVelocity;
			otherBody->mass += mass;
			float scale_ = cbrt(otherBody->mass);
			otherBody->SetActorScale3D(FVector(scale_, scale_, scale_));
		
			toBeDestroyed = true;
		}

		 
	}
}

//real-time in engine resize scale based on mass
void AGravBody::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) {

	Super::PostEditChangeProperty(PropertyChangedEvent);


	float scale_ = cbrt(mass);

	this->SetActorScale3D(FVector(scale_, scale_, scale_));
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

void AGravBody::MoveBody(float editedDT)
{
	SceneComponent->AddWorldOffset(velocity * editedDT);
}

