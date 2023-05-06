// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "NBodyHandler.h"
#include "GameManager.generated.h"

UCLASS()
class HONSPROJECT_API AGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameManager();

	UPROPERTY(Category = "references", EditAnywhere, BlueprintReadWrite)
		UNBodyHandler* BodyHandler_ref;
	UPROPERTY(Category = "references", EditAnywhere, BlueprintReadWrite)
		UTreeHandler* TreeHandler_ref;

	UPROPERTY(Category = "AsteroidFieldSettings", EditAnywhere, BlueprintReadWrite)
		float simulationRadius = 5;
	UPROPERTY(Category = "AsteroidFieldSettings", EditAnywhere, BlueprintReadWrite)
		float SpawnMaxSpeed = 40;
	UPROPERTY(Category = "AsteroidFieldSettings", EditAnywhere, BlueprintReadWrite)
		float SpawnMaxMass = 60;
	UPROPERTY(Category = "AsteroidFieldSettings", EditAnywhere, BlueprintReadWrite)
		int SimulationDesiredBodies = 400;
	
	int gradualSpawnerIndex = 0;
	int OverallSpawnerIndex = 0;
	bool spawningBodies = false;
	int bodiesToSpawn;
	AActor* playerRef;

	float timeMultiplier = 1.0f;
	bool useTreeCodes = true;

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "PlanetSpawn")
		void spawnAsteroidAt(FVector position_, FVector velocity_, double mass_, FVector4 colour_);

	void spawnAsteroidToGame();

	void graduallySpawnBodies(int spawnsPerFrame = 1);

	void deleteDestroyedBodies();
	void inGameAsteroidHandle();

	UFUNCTION(BlueprintCallable, Category = "PlanetSpawn")
	void startSpawning(int amount, float extent, float MaxVelocity_, double MaxMass_) {
		spawningBodies = true;
		bodiesToSpawn = amount;
		simulationRadius = extent;
		SpawnMaxSpeed = MaxVelocity_;
		SpawnMaxMass = MaxMass_;
		gradualSpawnerIndex = 0;
	}

	bool drawDebugs = true;
};
