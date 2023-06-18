// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NBody_Handler.h"
#include "TestPlanet.h"
#include "MainMenu_Manager.generated.h"

UCLASS()
class HONSPROJECT_API AMainMenuNBODYmanager : public AActor
{
	GENERATED_BODY()
	
public:	
	// ----- Sets default values for this actor's properties
	AMainMenuNBODYmanager();

	// ----- necessary gravitational calc logic
	bool clampVelocity = true;
	UPROPERTY(Category = "UI_references", BlueprintReadWrite)
		UNBodyHandler* BodyHandler_ref;
	UPROPERTY(Category = "UI_references", BlueprintReadWrite)
		float maxMMPlanetSpeed = 80.0f;
	UPROPERTY(Category = "UI_references", BlueprintReadWrite)
		UTreeHandler* TreeHandler_ref;

	// ----- spawning relevant variables
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		float SpawnInitialMaxSpeed = 70.0f;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		float SpawnInitialMaxMass = 6.0f;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		float despawnRadiusRW = 5000.0f;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		FVector simCentre = FVector(4000, 0, 0);

	// ----- Simulation relevant variables
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		int planetsToSimulate = 40;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		float timeMultiplier = 0.3f;
	double bigG = 39.4784f; //when using SolarMass, AU and Years
	int overallPlanets = 0;

	// ----- Cursor relevant variables
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		float CursorPlanetMass = 10.0f;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		float CursorMaxMass = 150.0f;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// ----- necessary functions
	void deleteDestroyedBodies();
	UFUNCTION(BlueprintCallable, Category = "PlanetSpawn")
		ATestPlanet* spawnPlanetAt(FVector position_, FVector velocity_, double mass_, FVector4 colour_, FString name_, float radius_);
	UFUNCTION(BlueprintCallable, Category = "PlanetSpawn")
		ATestPlanet* spawnEdgePlanet();
	UFUNCTION(BlueprintCallable, Category = "CursorSpawn")
		ATestPlanet* spawnCursor();
};
