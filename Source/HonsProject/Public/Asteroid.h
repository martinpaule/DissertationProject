// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/PrimitiveComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GravBodyComponent.h"

#include "Asteroid.generated.h"

/**
 * 
 */
UCLASS()
class HONSPROJECT_API AAsteroid : public AActor
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	AAsteroid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	//Gravitation body object necessary component
	UPROPERTY(Category = "BasicComponents", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* SceneComponent;
	UPROPERTY(Category = "BasicComponents", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* StaticMeshComponent;
	UPROPERTY(Category = "BasicComponents", VisibleAnywhere, BlueprintReadWrite)
		UMaterialInstanceDynamic* myMat;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USphereComponent* SphereCollider;
	

	

	UPROPERTY(Category = "BasicComponents", VisibleAnywhere, BlueprintReadWrite)
	UGravBodyComponent* GravComp;


	float myRotateSpeed;
	FRotator myRot;
};
