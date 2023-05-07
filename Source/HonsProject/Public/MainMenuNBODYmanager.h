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
	void deleteDestroyedBodies();
	UFUNCTION(BlueprintCallable, Category = "PlanetSpawn")
		ATestPlanet* spawnPlanetAt(FVector position_, FVector velocity_, double mass_, FVector4 colour_, FString name_, float radius_, UNBodyHandler* handlerToAddInto);
	UFUNCTION(BlueprintCallable, Category = "PlanetSpawn")
		ATestPlanet* spawnEdgePlanet();
	UFUNCTION(BlueprintCallable, Category = "CursorSpawn")
		ATestPlanet* spawnCursor();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	bool useTreeCodes = true;
	bool clampVelocity = true;
	UPROPERTY(Category = "UI_references", EditAnywhere, BlueprintReadWrite)
		UNBodyHandler* BodyHandler_ref;
	UPROPERTY(Category = "UI_references", EditAnywhere, BlueprintReadWrite)
		float maxMMPlanetSpeed = 50.0f;
	UPROPERTY(Category = "UI_references", EditAnywhere, BlueprintReadWrite)
		UTreeHandler* TreeHandler_ref;

	// ----- spawning relevant variables
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		float SpawnInitialMaxSpeed = 10;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		float SpawnInitialMaxMass = 2;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		float despawnRadiusRW = 1500;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		FVector simCentre = FVector(4000, 0, 0);
	
	// ----- Simulation relevant variables
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		int planetsToSimulate = 20;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		float timeMultiplier = 1.0f;
	double bigG = 39.4784f; //when using SolarMass, AU and Years
	int overallPlanets = 0;

	// ----- Cursor relevant variables
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		float CursorPlanetMass = 20.0f;
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
		float CursorMaxMass = 500.0f;
};
