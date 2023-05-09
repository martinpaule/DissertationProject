// Fill out your copyright notice in the Description page of Project Settings.


#include "Accuracy_Handler.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
// Sets default values
UAccuracyModule::UAccuracyModule()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryComponentTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void UAccuracyModule::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void UAccuracyModule::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


}


void UAccuracyModule::recordPositions() {

	//create a new array of planets in the 2D array of recordings
	TArray<planet> newRecording;
	planets.Add(newRecording);

	//note every single body into the TXT file
	for (int i = 0; i < bodyHandlerBodies->Num(); i++) {
		std::string name_ = std::string(TCHAR_TO_UTF8(*(*bodyHandlerBodies)[i]->GetOwner()->GetActorLabel()));
		notePlanet(name_, (*bodyHandlerBodies)[i]->position, (*bodyHandlerBodies)[i]->velocity, (*bodyHandlerBodies)[i]->mass);
	}

}

//add a new planet
void UAccuracyModule::notePlanet(std::string name_, FVector pos_, FVector vel_, float mass_) {
	planet a;

	a.name = name_;
	a.pos = pos_;
	a.vel = vel_;
	a.mass = mass_;

	planets.Last().Add(a);
}


void UAccuracyModule::printResultToTXT() {

	//using local path
	FString OutputPath = FPaths::ProjectDir().Append("Output_").Append(std::to_string(outputIndex).c_str()).Append(".txt");
	std::ofstream myfile_w(TCHAR_TO_UTF8(*OutputPath));

	if (myfile_w.is_open())
	{

		myfile_w << "NAME POSITION DIRECTION MASS " << "\n";

		for (int i = 0; i < planets.Num(); i++)
		{
			for (int j = 0; j < planets[i].Num(); j++) {
				myfile_w << planets[i][j].name << " (" << planets[i][j].pos.X << " " << planets[i][j].pos.Y << " " << planets[i][j].pos.Z << ") " << " (" << planets[i][j].vel.X << " " << planets[i][j].vel.Y << " " << planets[i][j].vel.Z << ") " << planets[i][j].mass << "\n";
			}
			myfile_w << " " << "\n";
		}

		myfile_w.close();
		outputIndex++;
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, "couldn't open file");
	}
}

bool UAccuracyModule::planetsEqual(planet a, planet b) {
	if (a.mass == b.mass && a.name == b.name && a.pos == b.pos && a.vel == b.vel) {
		return true;
	}
	return false;
}