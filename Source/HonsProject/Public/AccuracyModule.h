// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <fstream>
#include <string>
#include "AccuracyModule.generated.h"


struct planet {
public:
	std::string name;
	FVector pos;
	FVector vel;
	float mass;
	
};

UCLASS()
class HONSPROJECT_API AAccuracyModule : public AActor
{
	GENERATED_BODY()

public:




	// Sets default values for this actor's properties
	AAccuracyModule();




protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool planetsEqual(planet a, planet b) {
		if (a.mass == b.mass && a.name == b.name && a.pos == b.pos && a.vel == b.vel) {
			return true;
		}
		return false;
	}

	TArray<TArray<planet>> planets;
	void notePlanet(std::string name_, FVector pos_, FVector vel_, float mass_);
	
	void printResultToTXT();


};
