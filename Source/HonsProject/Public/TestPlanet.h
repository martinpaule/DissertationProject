// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GravBody_Component.h"

#include "TestPlanet.generated.h"

/**
 * 
 */
UCLASS()
class HONSPROJECT_API ATestPlanet : public AActor
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	ATestPlanet();



	//Gravitation body object necessary component
	UPROPERTY(Category = "BasicComponents", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* SceneComponent;
	UPROPERTY(Category = "BasicComponents", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* StaticMeshComponent;
	UPROPERTY(Category = "BasicComponents", VisibleAnywhere, BlueprintReadWrite)
		UMaterialInstanceDynamic* myMat;
	UPROPERTY(Category = "BasicComponents", VisibleAnywhere, BlueprintReadWrite)
		UGravBodyComponent* GravComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USphereComponent* SphereCollider;

	//further variables
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		int handlerID = 0;
	bool universalDensity = true;
	ATestPlanet* ghostRef = NULL;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//sets up collision function 
	virtual void PostInitializeComponents();


	//collision function
	UFUNCTION(BlueprintCallable, Category = "collision")
	void combineCollisionBody(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);



};
