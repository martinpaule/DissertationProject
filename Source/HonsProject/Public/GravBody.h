// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include <string>
#include "GravBody.generated.h"


class UMaterialInstanceDynamic;

UCLASS()
class HONSPROJECT_API AGravBody : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGravBody();

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

	//Gravitation body object necessary component
	UPROPERTY(Category = "BasicComponents", VisibleAnywhere, BlueprintReadWrite)
	USceneComponent * SceneComponent;
	UPROPERTY(Category = "BasicComponents", VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent * StaticMeshComponent;
	UPROPERTY(Category = "BasicComponents", VisibleAnywhere, BlueprintReadWrite)
	UMaterialInstanceDynamic* myMat;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* SphereCollider;
	
	//apply displacement to the body based on its velocity
	UFUNCTION(BlueprintCallable, Category="Move")
		void MoveBody(float editedDT);

	//simulation variables
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
	float mass = 1;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
	FVector velocity = FVector(0,0,0);

	bool toBeDestroyed = false;
};
