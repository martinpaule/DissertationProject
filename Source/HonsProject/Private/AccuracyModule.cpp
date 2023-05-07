// Fill out your copyright notice in the Description page of Project Settings.


#include "AccuracyModule.h"

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

	//accuracy calculation variables
	int positive_Comparisons = 0;
	int negative_Comparisons = 0;
	bool avgDiffRelevance = true;
	planet AvgDifference;
	int avgDiffIdx = 0;
	AvgDifference.pos = FVector(0, 0, 0);
	AvgDifference.vel = FVector(0, 0, 0);
	AvgDifference.mass = 0.0f;


	//accuracy Testing
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
							avgDiffRelevance = false;
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
	}

	//write into the file
	std::ofstream myfile_w("D:\\LocalWorkDir\\1903300\\DissertationProject\\testOutputs.txt");
	//std::ofstream myfile_w("D:\\Users\\User\\Documents\\GitHub\\DissertationProject\\testOutputs.txt");

	if (myfile_w.is_open())
	{

		myfile_w << "Overall Accuracy: " << float(positive_Comparisons) / float(positive_Comparisons + negative_Comparisons) * 100.0f << "%. (<- correct comparisons in AllwAll comps)" << "\n";
		myfile_w << "Individual simulations: " << planets.Num() << "\n";
		if (avgDiffRelevance) {
			if (negative_Comparisons > 0) {
				myfile_w << "All simulations ended with same amount of bodies. Average of error in; POS: (" << AvgDifference.pos.X << " " << AvgDifference.pos.Y << " " << AvgDifference.pos.Z << "),  VEL: (" << AvgDifference.vel.X << " " << AvgDifference.vel.Y << " " << AvgDifference.vel.Z << "),  MASS: " << AvgDifference.mass << "\n";
			}
		}
		else {
			myfile_w << "Simulations finished with an inconsistent number of planets" << "\n";
		}

		myfile_w << " " << "\n";
		myfile_w << "NAME POSITION DIRECTION MASS " << "\n";

		for (int i = 0; i < planets.Num(); i++)
		{
			for (int j = 0; j < planets[i].Num(); j++) {
				myfile_w << planets[i][j].name << " (" << planets[i][j].pos.X << " " << planets[i][j].pos.Y << " " << planets[i][j].pos.Z << ") " << " (" << planets[i][j].vel.X << " " << planets[i][j].vel.Y << " " << planets[i][j].vel.Z << ") " << planets[i][j].mass << "\n";
			}
			myfile_w << " " << "\n";
		}

		myfile_w.close();
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "couldnt openfile");
	}
}

bool UAccuracyModule::planetsEqual(planet a, planet b) {
	if (a.mass == b.mass && a.name == b.name && a.pos == b.pos && a.vel == b.vel) {
		return true;
	}
	return false;
}