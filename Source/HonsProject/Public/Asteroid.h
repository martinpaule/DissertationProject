// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GravBody.h"
#include "Asteroid.generated.h"

/**
 * 
 */
UCLASS()
class HONSPROJECT_API AAsteroid : public AGravBody
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


		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USphereComponent* SphereCollider;
	
};
