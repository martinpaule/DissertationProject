// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NBodyHandler.h"

#include "MainMenuNBODYmanager.generated.h"

UCLASS()
class HONSPROJECT_API AMainMenuNBODYmanager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMainMenuNBODYmanager();
	UFUNCTION(BlueprintCallable, Category = "PlanetSpawn")
		void spawnPlanetAt(FVector position_, FVector velocity_, double mass_, FVector4 colour_, FString name_, float radius_, UNBodyHandler* handlerToAddInto);
	void deleteDestroyedBodies();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	bool useTreeCodes = true;
	
	UPROPERTY(Category = "UI_references", EditAnywhere, BlueprintReadWrite)
		UNBodyHandler* BodyHandler_ref;
	UPROPERTY(Category = "UI_references", EditAnywhere, BlueprintReadWrite)
		UTreeHandler* TreeHandler_ref;

	// ----- spawning relevant variables
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		float SpawnInitialMaxSpeed = 10;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		float SpawnInitialMaxMass = 2;

	float timeMultiplier = 10.0f;
	double bigG = 39.4784f; //when using SolarMass, AU and Years

	int overallPlanets = 0;
};
