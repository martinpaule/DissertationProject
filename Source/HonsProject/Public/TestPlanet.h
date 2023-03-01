// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GravBody.h"
#include "TestPlanet.generated.h"

/**
 * 
 */
UCLASS()
class HONSPROJECT_API ATestPlanet : public AGravBody
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	ATestPlanet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//engine runtime functions - important to avoid pointer erros
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent);
	virtual void PostInitializeComponents();


	//collision function
	UFUNCTION(BlueprintCallable, Category = "collision")
	void combineCollisionBody(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);



	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USphereComponent* SphereCollider;


	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		int handlerID = 0;


	bool universalDensity = true;

	ATestPlanet* ghostRef = NULL;
};
