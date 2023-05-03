// Fill out your copyright notice in the Description page of Project Settings.


#include "TestPlanet.h"
#include <string>


// Sets default values
ATestPlanet::ATestPlanet()
{
	PrimaryActorTick.bCanEverTick = false;

	ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/MyContent/Graphics/PlanetRelated/PlanetMesh.PlanetMesh'"));
	UStaticMesh* Asset = MeshAsset.Object;


	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshCompopnent"));
	StaticMeshComponent->SetupAttachment(SceneComponent);

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




	
}




// Called when the game starts or when spawned
void ATestPlanet::BeginPlay()
{
	Super::BeginPlay();


	

}

// Called every frame
void ATestPlanet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



//bind overlap function
void ATestPlanet::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &ATestPlanet::combineCollisionBody);

}


//real-time in engine resize scale based on mass
void ATestPlanet::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) {

	// !! Scale 1 means diameter is 100 km  !!
	//means scale = radius*2/100
	Super::PostEditChangeProperty(PropertyChangedEvent);

	//if (universalDensity) {
	//	float scale_ = cbrt(GravComp->mass);
	//	this->SetActorScale3D(FVector(scale_, scale_, scale_));
	//}
	//
	//if (PropertyChangedEvent.Property->GetName() == "radius") {
	//	float scale_ = GravComp->radius * 2.0f / 100.0f;
	//	this->SetActorScale3D(FVector(scale_, scale_, scale_));
	//	universalDensity = false;
	//}

}



//perfectly inelastic
// p = momentum. p = m*v.
// p_ = resulting momentum, v_ = resulting velocity
//p1+p2=p_1+p_2			        
//m1*v1 + m2*v2 =m1*v_1 + m2*v_2
//v_ = (m1*v1 + m2*v2)/(m1 + m2)
void ATestPlanet::combineCollisionBody(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	
	


	//perfectly ine	lastic collision
	ATestPlanet* otherBody = Cast<ATestPlanet>(OtherActor);
	
	//if other is not test planet
	if (!otherBody) {
		return;
	}


	//error failsafe
	if (!GravComp || !otherBody->GravComp) {
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, "Rare error - one gravcomp was missing!");
		////add into above name like  + std::string(TCHAR_TO_UTF8(*this->GetActorLabel()))
		//this->Destroy();
		return;
	}

	if (handlerID != otherBody->handlerID) {
		return;
	}

	float massA = GravComp->mass;
	float massB = otherBody->GravComp->mass;
	FVector velocityA = GravComp->velocity;
	FVector velocityB = otherBody->GravComp->velocity;
	FVector finalVelocity = (massA * velocityA + massB * velocityB) / (massA + massB);

	//add the smaller body's mass and speed to the larger one
	if (otherBody->GravComp->mass >= GravComp->mass) {
		otherBody->GravComp->velocity = finalVelocity;
		otherBody->GravComp->mass += GravComp->mass;
		float scale_ = otherBody->GetActorScale3D().X;
		scale_ += this->GetActorScale3D().X / 15.0f;
		otherBody->SetActorScale3D(FVector(scale_, scale_, scale_));
		GravComp->toBeDestroyed = true;

		//print message with timestamp, development feature, remove at the end
		if (true) {
			FDateTime nowTime = FDateTime::Now();
			std::string printStr = "(";
			FString myName = this->GetActorLabel();
			FString otherName = otherBody->GetActorLabel();
			std::string names = std::string(TCHAR_TO_UTF8(*myName));
			names += " and ";
			names += std::string(TCHAR_TO_UTF8(*otherName));
			printStr += std::to_string(nowTime.GetHour()) + ":" + std::to_string(nowTime.GetMinute()) + ":" + std::to_string(nowTime.GetSecond()) + "." + std::to_string(nowTime.GetMillisecond()) + "Merged Bodies " + names;
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, printStr.c_str());
		}

	}
		


	
}