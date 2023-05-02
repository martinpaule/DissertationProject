// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GravBodyComponent.generated.h"

struct TreeNode;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HONSPROJECT_API UGravBodyComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGravBodyComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//simulation variables
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		FVector velocity = FVector(0.0f, 0.0f, 0.0f); //km/s
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		FVector position = FVector(0.0f, 0.0f, 0.0f); 
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		double mass = 1; //kg
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		double radius = 1; //km
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		bool toBeDestroyed = false;

	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		FVector4 myCol;

	TreeNode * leaf_ref = NULL;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		float myLocalTimeEditor = 1.0f;

};
