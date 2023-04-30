// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuNBODYmanager.h"

// Sets default values
AMainMenuNBODYmanager::AMainMenuNBODYmanager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMainMenuNBODYmanager::BeginPlay()
{
	Super::BeginPlay();

	FTransform tr;
	tr.SetIdentity();

	//create Nbody handler
	BodyHandler_ref = Cast<UNBodyHandler>(this->AddComponentByClass(UNBodyHandler::StaticClass(), false, tr, true));
	BodyHandler_ref->RegisterComponent();

	//setup Nbody handler
	BodyHandler_ref->useTreeCodes_ = useTreeCodes;
	//create tree code handler
	TreeHandler_ref = Cast<UTreeHandler>(this->AddComponentByClass(UTreeHandler::StaticClass(), false, tr, true));
	TreeHandler_ref->RegisterComponent();
	TreeHandler_ref->bodyHandlerBodies = &BodyHandler_ref->myGravBodies;

	//assign tree code handler
	BodyHandler_ref->treeHandlerRef = TreeHandler_ref;
	

	for (int i = 0; i < 20; i++) {

		//random location
		FVector myLoc(-SpawnLocationBounds / 2, -SpawnLocationBounds / 2, -SpawnLocationBounds / 2);
		//myLoc.X += FMath::FRandRange(0, SpawnLocationBounds);
		//myLoc.Y += FMath::FRandRange(0, SpawnLocationBounds);
		//myLoc.Z += FMath::FRandRange(0, SpawnLocationBounds);

		//random speed
		FVector speed_ = FVector(-SpawnInitialMaxSpeed / 2, -SpawnInitialMaxSpeed / 2, -SpawnInitialMaxSpeed / 2);
		speed_.X += FMath::FRandRange(0, SpawnInitialMaxSpeed);
		speed_.Y += FMath::FRandRange(0, SpawnInitialMaxSpeed);
		speed_.Z += FMath::FRandRange(0, SpawnInitialMaxSpeed);

		//random mass
		float mass_ = 0.001f;
		mass_ += FMath::FRandRange(0.0f, SpawnInitialMaxMass);

		FString bodName = "Body ";
		bodName.Append(std::to_string(overallPlanets).c_str());
		spawnPlanetAt(myLoc, speed_, mass_, FVector4(1.0f, 0.0f, 1.0f, 1.0f), bodName, 0, BodyHandler_ref);
	}
}


// setup function for spawning bodies - creates a new body with specified parameters
void AMainMenuNBODYmanager::spawnPlanetAt(FVector position_, FVector velocity_, double mass_, FVector4 colour_, FString name_, float radius_, UNBodyHandler* handlerToAddInto)
{

	FActorSpawnParameters SpawnInfo;
	FRotator myRot(0, 0, 0);

	//assign body's variables
	ATestPlanet* newBody = GetWorld()->SpawnActor<ATestPlanet>(position_ * 1000.0f, myRot, SpawnInfo);
	//newBody->SetActorEnableCollision(true);


	FTransform tr;
	tr.SetIdentity();

	//create Nbody handler
	newBody->GravComp = Cast<UGravBodyComponent>(newBody->AddComponentByClass(UGravBodyComponent::StaticClass(), true, tr, true));
	newBody->GravComp->RegisterComponent();


	newBody->GravComp->toBeDestroyed = false;
	newBody->GravComp->position = position_;
	newBody->GravComp->velocity = velocity_;
	newBody->GravComp->radius = radius_;
	newBody->GravComp->mass = mass_;
	newBody->SetActorLabel(name_);


	if (radius_ == 0.0f) {
		radius_ = cbrt(mass_);
	}

	//currently more for display purposes
	newBody->SetActorScale3D(FVector(radius_, radius_, radius_));

	//option to set colour too
	if (colour_ != FVector4(1.0f, 0.0f, 1.0f, 1.0f)) {
		newBody->myMat->SetVectorParameterValue(TEXT("Colour"), colour_);
		newBody->GravComp->myCol = colour_;
	}
	handlerToAddInto->myGravBodies.Add(newBody->GravComp);
	overallPlanets++;

}

void AMainMenuNBODYmanager::deleteDestroyedBodies() {

	for (int i = 0; i < BodyHandler_ref->myGravBodies.Num(); i++)
	{

		UGravBodyComponent* CompIT = BodyHandler_ref->myGravBodies[i];



		if (CompIT->toBeDestroyed) {

			ATestPlanet* asTP = Cast<ATestPlanet>(CompIT->GetOwner());
			CompIT->GetOwner()->Destroy();
			CompIT->DestroyComponent();
						BodyHandler_ref->myGravBodies.RemoveAt(i);
			i--;
		}


	}

}


// Called every frame
void AMainMenuNBODYmanager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//step 0: destroy overlapping bodies from previous step - must be done before force calculation otherwise the current step will be inaccurate
	deleteDestroyedBodies();

	//step 1: Gravitational calculations using fixed time updates

	//dt influenced by simulation time scale 
	double updatedDT = DeltaTime * timeMultiplier * 0.027f; //0.027 makes the time as 10 days/s		


	if (useTreeCodes) {
		BodyHandler_ref->calculateWithTree(updatedDT, false, false);

	}
	else {

	}
	//step 2: move bodies using their updated velocity, also destroy ones that 
	BodyHandler_ref->moveBodies(true, updatedDT);

}

