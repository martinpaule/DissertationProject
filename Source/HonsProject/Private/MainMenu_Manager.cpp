// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu_Manager.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
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

	//setup Nbody handler & create tree code handler
	BodyHandler_ref->constructTreeHandler();

	
}

//spawn cursor logic
ATestPlanet* AMainMenuNBODYmanager::spawnCursor() {
	//spawn cursor planet
	FVector CursorPlanetPos = FVector(0, 0, 0);
	FVector CursorPlanetSpeed = FVector(0, 0, 0);
	FString CursorPlanetbodName = "CursorPlanet";


	ATestPlanet* AsTP = spawnPlanetAt(CursorPlanetPos, CursorPlanetSpeed, CursorPlanetMass, FVector4(1.0f, 0.0f, 1.0f, 1.0f), CursorPlanetbodName, 0);

	return AsTP;

}

//spawns a planet at the edge of the screen, just out the range of the defined range (camera)
ATestPlanet * AMainMenuNBODYmanager::spawnEdgePlanet() {

	//random speed
	FVector speed_ = FVector(0.0f, 0.0f,0.0f);
	speed_.Y += FMath::FRandRange(-1.0f, 1.0f);
	speed_.Z += FMath::FRandRange(-1.0f, 1.0f);
	speed_.Normalize();

	//location is opposite of the planet's initial speed
	FVector myLoc = simCentre;
	myLoc += -speed_ * despawnRadiusRW * 0.9f;
	speed_ *= FMath::FRandRange(0.0f, SpawnInitialMaxSpeed);

	//random direction and rotation
	float sideRot = FMath::FRandRange(20.0f, 40.0f);
	if (FMath::FRandRange(0.0f, 1.0f) > 0.5f) {
		sideRot *= -1.0f;
	}
	speed_ = speed_.RotateAngleAxis(sideRot, FVector(1, 0, 0));

	//random mass
	float mass_ = 0.001f;
	mass_ += FMath::FRandRange(0.0f, SpawnInitialMaxMass);

	//define name
	FString bodName = "Body_";
	bodName.Append(std::to_string(overallPlanets).c_str());

	//random colour
	FVector4 randColor = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
	randColor.X -= FMath::FRandRange(0, 1.0f);
	randColor.Y -= FMath::FRandRange(0, 1.0f);
	randColor.Z -= FMath::FRandRange(0, 1.0f);

	return spawnPlanetAt(myLoc / 1000.0f, speed_, mass_, randColor, bodName, 0);

}

// setup function for spawning bodies - creates a new body with specified parameters
ATestPlanet * AMainMenuNBODYmanager::spawnPlanetAt(FVector position_, FVector velocity_, double mass_, FVector4 colour_, FString name_, float radius_)
{

	FActorSpawnParameters SpawnInfo;
	FRotator myRot(0, 0, 0);

	//assign body's variables
	ATestPlanet* newBody = GetWorld()->SpawnActor<ATestPlanet>(position_ * 1000.0f, myRot, SpawnInfo);
	newBody->GravComp = BodyHandler_ref->addGravCompAt(position_, velocity_, mass_, newBody);
	newBody->SetActorLabel(name_);


	if (radius_ == 0.0f) {
		radius_ = cbrt(mass_);
	}

	//currently more for display purposes
	newBody->SetActorScale3D(FVector(radius_, radius_, radius_));
	//option to set colour too
	newBody->myMat->SetVectorParameterValue(TEXT("Colour"), colour_);
	newBody->GravComp->myCol = colour_;

	//handlerToAddInto->myGravBodies.Add(newBody->GravComp);
	overallPlanets++;

	return newBody;
}

//cleanup function
void AMainMenuNBODYmanager::deleteDestroyedBodies() {

	for (int i = 1; i < BodyHandler_ref->myGravBodies.Num(); i++)
	{

		UGravBodyComponent* CompIT = BodyHandler_ref->myGravBodies[i];


		if ((CompIT->position*1000.0f - simCentre).Length() > despawnRadiusRW || CompIT->toBeDestroyed) {

			ATestPlanet* asTP = Cast<ATestPlanet>(CompIT->GetOwner());
			CompIT->GetOwner()->Destroy();
			CompIT->DestroyComponent();
			BodyHandler_ref->myGravBodies.RemoveAt(i);
			//spawnEdgePlanet();
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

	BodyHandler_ref->treeHandler->RecalculatePartitioning(false);

	//step 1: Gravitational calculations using fixed time updates
	//dt influenced by simulation time scale 
	double updatedDT = DeltaTime * timeMultiplier * 0.027f; //0.027 makes the time as 10 days/s		


	BodyHandler_ref->calculateWithTree(updatedDT);

	

	float lerpVal = BodyHandler_ref->myGravBodies[0]->mass / CursorMaxMass;

	//reset velocity of cursor, clamp its size and change scale
	BodyHandler_ref->myGravBodies[0]->velocity = FVector(0, 0, 0);
	BodyHandler_ref->myGravBodies[0]->mass = FMath::Clamp(BodyHandler_ref->myGravBodies[0]->mass, 0.0f, CursorMaxMass + 5.0f);
	BodyHandler_ref->myGravBodies[0]->GetOwner()->SetActorScale3D(FVector(1.0f, 1.0f, 1.0f)*FMath::Lerp(2.0f,4.0f, lerpVal));

	//cap velocity
	if(clampVelocity){

		for (int i = 1; i < BodyHandler_ref->myGravBodies.Num(); i++)
		{
			UGravBodyComponent* CompIT = BodyHandler_ref->myGravBodies[i];

			CompIT->velocity = CompIT->velocity.GetClampedToMaxSize(maxMMPlanetSpeed);


			//float distanceBasedMaxVel = (CompIT->GetOwner()->GetActorLocation() - simCentre).Length();
			//if (CompIT->velocity.Length() > distanceBasedMaxVel) {
			//	CompIT->velocity = CompIT->velocity.GetSafeNormal() * distanceBasedMaxVel;
			//}


		}
	}
	

	//step 2: move bodies using their updated velocity, also destroy ones that 
	BodyHandler_ref->moveBodies(true, updatedDT);



}

