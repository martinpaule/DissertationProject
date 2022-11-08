// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
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

	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
	USceneComponent * SceneComponent;

	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent * StaticMeshComponent;

	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
	UMaterialInstanceDynamic* myMat;

	//UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
	//UBoxComponent * BoxCollider;
	
	void spawnSetup(FVector initialSpeed, float bodyMass);

	UFUNCTION(BlueprintCallable, Category="Move")
		void MoveBody(float dt,float timeMultiplier);
	
	UPROPERTY(Category = "myCategory", EditAnywhere, BlueprintReadWrite)
	float mass = 0;
	UPROPERTY(Category = "myCategory", EditAnywhere, BlueprintReadWrite)
	FVector speed = FVector(0,0,0);

	//UFUNCTION()void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
