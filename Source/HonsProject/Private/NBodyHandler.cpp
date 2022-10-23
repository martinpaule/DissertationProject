// Fill out your copyright notice in the Description page of Project Settings.


#include "NBodyHandler.h"
#include <Kismet/GameplayStatics.h>
#include "GravBody.h"
#include <string>

#include "GameFramework/DefaultPawn.h"
// Sets default values
ANBodyHandler::ANBodyHandler()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	srand(NULL);

	// Set this pawn to be controlled by the lowest-numbered player
	//AutoPossessPlayer = EAutoReceiveInput::Player0;
	

	//InputComponent = NewObject<UInputComponent>(this);
	//InputComponent->RegisterComponent();

	
}

// Called when the game starts or when spawned
void ANBodyHandler::BeginPlay()
{
	Super::BeginPlay();
	ADefaultPawn * pawn_ = Cast<ADefaultPawn>(UGameplayStatics::GetActorOfClass(GetWorld(), ADefaultPawn::StaticClass()));
	UInputComponent * inp_ = pawn_->InputComponent;
	if (inp_)
	{
		// Bind an action to it
		inp_->BindAction
		(
			"SimSpeedUp", // The input identifier (specified in DefaultInput.ini)
			IE_Pressed, // React when button pressed (or on release, etc., if desired)
			this, // The object instance that is going to react to the input
			&ANBodyHandler::raiseSimulationSpeed // The function that will fire when input is received
		);
	
		// Bind an action to it
		inp_->BindAction
		(
			"SimSpeedDown", // The input identifier (specified in DefaultInput.ini)
			IE_Pressed, // React when button pressed (or on release, etc., if desired)
			this, // The object instance that is going to react to the input
			&ANBodyHandler::lowerSimulationSpeed // The function that will fire when input is received
		);
		//EnableInput(GetWorld()->GetFirstPlayerController());
	}

	for(int i = 0; i < 80; i++)
	{
		FActorSpawnParameters SpawnInfo;
		FRotator myRot(0, 0, 0);
		FVector myLoc(-50000.0f,-5000.0f,-5000.0f);
		myLoc.X += rand()%10000;
		myLoc.Y += rand()%10000;
		myLoc.Z += rand()%10000;
		GetWorld()->SpawnActor<AGravBody>(myLoc, myRot, SpawnInfo);
	}
}
bool ANBodyHandler::mergeGravBodies()
{
	TArray<AActor*> FoundActors_m;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGravBody::StaticClass(), FoundActors_m);
		
	for(int i = 0; i < FoundActors_m.Num();i++)
	{
		for(int j = 0; j < FoundActors_m.Num();j++)
		{
			if(i != j)
			{
				FVector distance = FoundActors_m[j]->GetActorLocation() - FoundActors_m[i]->GetActorLocation();
				float length_ = distance.Length();
				float combineThreshHold = (FoundActors_m[i]->GetActorScale3D().X + FoundActors_m[j]->GetActorScale3D().X)*45;
				if(length_ < combineThreshHold)
				{
					Cast<AGravBody>(FoundActors_m[i])->mass += Cast<AGravBody>(FoundActors_m[j])->mass;
					float scale_ = cbrt(Cast<AGravBody>(FoundActors_m[i])->mass);
					FoundActors_m[i]->SetActorScale3D(FVector(scale_,scale_,scale_));

					FoundActors_m[j]->Destroy();
					return false;
				}
			}
		}
	}

	return true;
}
// Called every frame
void ANBodyHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGravBody::StaticClass(), FoundActors);
	
	for(int i = 0; i < FoundActors.Num();i++)
	{
		FVector sumOfForces = FVector(0.0f,0.0f,0.0f);
		for(int j = 0; j < FoundActors.Num();j++)
		{
			if(i != j)
			{
				FVector distance = FoundActors[j]->GetActorLocation() - FoundActors[i]->GetActorLocation();
				float length_ = distance.Length();
				sumOfForces += Cast<AGravBody>(FoundActors[j])->mass*distance/length_*length_*length_;
			}
		}
		Cast<AGravBody>(FoundActors[i])->speed += bigG*(sumOfForces)/Cast<AGravBody>(FoundActors[i])->mass;
	}

	
	for(int i = 0; i < FoundActors.Num();i++)
	{
		Cast<AGravBody>(FoundActors[i])->MoveBody(DeltaTime,timeMultiplier);
	}

	bool FinishedMerging = false;

	while(!FinishedMerging)
	{
		FinishedMerging = mergeGravBodies();
	}
}


void ANBodyHandler::raiseSimulationSpeed()
{
	if(timeMultiplier > 100)
	{
		timeMultiplier+= 20;
	}
	else if(timeMultiplier > 50)
	{
		timeMultiplier+= 10;
	}
	else if(timeMultiplier > 20)
	{
		timeMultiplier+= 5;
	}
	else if(timeMultiplier > 5)
	{
		timeMultiplier+= 3;
	}
	else{timeMultiplier++;}

	std::string debug_ = "Simulation speed RAISED to ";
	debug_ += std::to_string(timeMultiplier);
	GEngine->AddOnScreenDebugMessage(-1,  5.0f, FColor::Yellow, debug_.c_str());	

}
void ANBodyHandler::lowerSimulationSpeed()
{
	if(timeMultiplier > 100)
	{
		timeMultiplier-= 20;
	}
	else if(timeMultiplier > 50)
	{
		timeMultiplier-= 10;
	}
	else if(timeMultiplier > 20)
	{
		timeMultiplier-= 5;
	}
	else if(timeMultiplier > 5)
	{
		timeMultiplier-= 3;
	}
	else if(timeMultiplier>0){timeMultiplier--;}

	std::string debug_ = "Simulation speed LOWERED to ";
	debug_ += std::to_string(timeMultiplier);
	GEngine->AddOnScreenDebugMessage(-1,  5.0f, FColor::Yellow, debug_.c_str());	
}
/*
// Called to bind functionality to input
void ANBodyHandler::SetupPlayerInputComponent(class UInputComponent* InputComponent_)
{
	Super::SetupPlayerInputComponent(InputComponent_);
	if (InputComponent_)
	{
		
		// Bind an action to it
		InputComponent_->BindAction
		(
			"SimSpeedUp", // The input identifier (specified in DefaultInput.ini)
			IE_Pressed, // React when button pressed (or on release, etc., if desired)
			this, // The object instance that is going to react to the input
			&ANBodyHandler::raiseSimulationSpeed // The function that will fire when input is received
		);
	
		// Bind an action to it
		InputComponent_->BindAction
		(
			"SimSpeedDown", // The input identifier (specified in DefaultInput.ini)
			IE_Pressed, // React when button pressed (or on release, etc., if desired)
			this, // The object instance that is going to react to the input
			&ANBodyHandler::lowerSimulationSpeed // The function that will fire when input is received
		);
	}
}
*/