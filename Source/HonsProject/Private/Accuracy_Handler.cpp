// Fill out your copyright notice in the Description page of Project Settings.


#include "Accuracy_Handler.h"

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


planet UAccuracyModule::getAverageDifference() {


	//accuracy calculation variables
	planet AvgDifference;
	bool avgDiffRelevance = true;
	int positive_Comparisons = 0;
	int negative_Comparisons = 0;
	int avgDiffIdx = 0;
	AvgDifference.pos = FVector(0, 0, 0);
	AvgDifference.vel = FVector(0, 0, 0);
	AvgDifference.mass = 0.0f;






	//accuracy Testing - WIP
	for (int i = 0; i < planets.Num(); i++) {
		for (int j = i + 1; j < planets.Num(); j++) {

			int k = 0;
			bool stillComparing = true;
			while (stillComparing) {

				int validIndexes = 0;

				if (k < planets[i].Num()) {
					validIndexes++;
				}
				if (k < planets[j].Num()) {
					validIndexes++;
				}

				switch (validIndexes) {
				case 0:
					stillComparing = false;
					break;
				case 1:
					negative_Comparisons++;
					avgDiffRelevance = false;
					break;
				case 2:
					if (!planetsEqual(planets[i][k], planets[j][k])) {
						negative_Comparisons++;

						if (planets[i][k].name == planets[j][k].name) {
							avgDiffIdx++;
							AvgDifference.pos += (planets[i][k].pos - planets[j][k].pos).GetAbs();
							AvgDifference.vel += (planets[i][k].vel - planets[j][k].vel).GetAbs();
							AvgDifference.mass += abs(planets[i][k].mass - planets[j][k].mass);
						}
						else {
						}
					}
					else {
						positive_Comparisons++;
					}
					break;
				default:
					break;
				}
				k++;
			}
		}
	}

	//calculate average error
	if (avgDiffRelevance) {
		AvgDifference.pos /= avgDiffIdx;
		AvgDifference.vel /= avgDiffIdx;
		AvgDifference.mass /= avgDiffIdx;
		AvgDifference.name = "Success";
	}
	else {
		AvgDifference.name = "IncoherentIndexes";
	}
	return AvgDifference;
}
void UAccuracyModule::printResultToTXT() {

	std::string filePath = std::string("D:\\Users\\User\\Documents\\GitHub\\DissertationProject\\Output_").append(std::to_string(outputIndex)).append(".txt");
	//write into the file
	//std::ofstream myfile_w("D:\\LocalWorkDir\\1903300\\DissertationProject\\testOutputs.txt");
	std::ofstream myfile_w(filePath);

	if (myfile_w.is_open())
	{

		//myfile_w << "Overall Accuracy: " << float(positive_Comparisons) / float(positive_Comparisons + negative_Comparisons) * 100.0f << "%. (<- correct comparisons in AllwAll comps)" << "\n";
		//myfile_w << "Individual simulations: " << planets.Num() << "\n";
		//if (avgDiffRelevance) {
		//	if (negative_Comparisons > 0) {
		//		myfile_w << "All simulations ended with same amount of bodies. Average of error in; POS: (" << AvgDifference.pos.X << " " << AvgDifference.pos.Y << " " << AvgDifference.pos.Z << "),  VEL: (" << AvgDifference.vel.X << " " << AvgDifference.vel.Y << " " << AvgDifference.vel.Z << "),  MASS: " << AvgDifference.mass << "\n";
		//	}
		//}
		//else {
		//	myfile_w << "Simulations finished with an inconsistent number of planets" << "\n";
		//}
		//
		//myfile_w << " " << "\n";
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