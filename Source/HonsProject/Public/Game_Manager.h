// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"

#include "NBody_Handler.h"
#include "Game_Manager.generated.h"

UCLASS()
class HONSPROJECT_API AGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameManager();

	UPROPERTY(Category = "references", EditAnywhere, BlueprintReadWrite)
		UNBodyHandler* BodyHandler_ref;

	// ----- Game Settings
	UPROPERTY(Category = "AsteroidFieldSettings", EditAnywhere, BlueprintReadWrite)
		float simulationRadius = 5;
	UPROPERTY(Category = "AsteroidFieldSettings", EditAnywhere, BlueprintReadWrite)
		FVector2D SpawnSpeed = 40;
	UPROPERTY(Category = "AsteroidFieldSettings", EditAnywhere, BlueprintReadWrite)
		FVector SimulationCentre = FVector(0.0f, 0.0f, 0.0f);
	UPROPERTY(Category = "AsteroidFieldSettings", EditAnywhere, BlueprintReadWrite)
		FVector2D SpawnMass = 60;
	UPROPERTY(Category = "AsteroidFieldSettings", EditAnywhere, BlueprintReadWrite)
		int SimulationDesiredBodies = 100;
	UPROPERTY(Category = "Debugging", EditAnywhere, BlueprintReadWrite)
		bool drawDebugs = true;

	// ----- Game defaults
	int OverallSpawnerIndex = 0;
	AActor* playerRef;
	UPROPERTY(Category = "AsteroidFieldSettings", EditAnywhere, BlueprintReadWrite)
	bool useTreeCodes = true;
	UPROPERTY(Category = "Testing", EditAnywhere, BlueprintReadWrite)
	bool newTrees = false;
	float timeMultiplier = 0.7f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void spawnAsteroidToGame();

	void handleDestroyingAsteroids();
	void inGameAsteroidHandle();


	UFUNCTION(BlueprintCallable, Category = "AddBody")
	void addAsteroid(UGravBodyComponent* ref);
	UFUNCTION(BlueprintCallable, Category = "RemoveBody")
	void removeAsteroid(UGravBodyComponent* ref);


};
