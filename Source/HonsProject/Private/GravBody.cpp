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

	ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/MyHonsContent/PlanetRelated/PlanetMesh.PlanetMesh'"));
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
	
	myCol = randCol;

}

//bind overlap function
void AGravBody::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AGravBody::combineCollisionBody);

}


//real-time in engine resize scale based on mass
void AGravBody::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) {

	// !! Scale 1 means diameter is 100 km  !!
	//means scale = radius*2/100
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (universalDensity) {
		float scale_ = cbrt(mass);
		this->SetActorScale3D(FVector(scale_, scale_, scale_));
	}
	
	if (PropertyChangedEvent.Property->GetName() == "radius") {
		float scale_ = radius * 2.0f / 100.0f;
		this->SetActorScale3D(FVector(scale_, scale_, scale_));
		universalDensity = false;
	}

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

void AGravBody::MoveBody(double editedDT)
{
	position += velocity * editedDT;
	this->SetActorLocation(position* 1000.0f);

}


//perfectly inelastic
// p = momentum. p = m*v.
// p_ = resulting momentum, v_ = resulting velocity
//p1+p2=p_1+p_2			        
//m1*v1 + m2*v2 =m1*v_1 + m2*v_2
//v_ = (m1*v1 + m2*v2)/(m1 + m2)
void AGravBody::combineCollisionBody(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	//perfectly inelastic collision
	AGravBody* otherBody = Cast<AGravBody>(OtherActor);

	if (otherBody)
	{
		

		

		if (handlerID != otherBody->handlerID) {
			return;
		}

		float massA = mass;
		float massB = otherBody->mass;
		FVector velocityA = velocity;
		FVector velocityB = otherBody->velocity;
		FVector finalVelocity = (massA * velocityA + massB * velocityB) / (massA + massB);

		//add the smaller body's mass and speed to the larger one
		if (otherBody->mass >= mass) {
			otherBody->velocity = finalVelocity;
			otherBody->mass += mass;
			float scale_ = otherBody->GetActorScale3D().X;
			scale_ += this->GetActorScale3D().X / 15.0f;
			otherBody->SetActorScale3D(FVector(scale_, scale_, scale_));
			toBeDestroyed = true;

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
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, printStr.c_str());
			}
			
		}


	}
}