// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuNBODYmanager.h"
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

	//setup Nbody handler
	BodyHandler_ref->useTreeCodes_ = useTreeCodes;
	//create tree code handler
	TreeHandler_ref = Cast<UTreeHandler>(this->AddComponentByClass(UTreeHandler::StaticClass(), false, tr, true));
	TreeHandler_ref->RegisterComponent();
	TreeHandler_ref->bodyHandlerBodies = &BodyHandler_ref->myGravBodies;

	//assign tree code handler
	BodyHandler_ref->treeHandlerRef = TreeHandler_ref;


	if(true){
		//spawn cursor planet
		FVector CursorPlanetPos = FVector(0, 0, 0);
		FVector CursorPlanetSpeed = FVector(0, 0, 0);
		FString CursorPlanetbodName = "CursorPlanet";
			

		 ATestPlanet* AsTP = spawnPlanetAt(CursorPlanetPos, CursorPlanetSpeed, CursorPlanetMass, FVector4(1.0f, 0.0f, 1.0f, 1.0f), CursorPlanetbodName, 0, BodyHandler_ref);

		 if (AsTP) {
			 AsTP->SphereCollider->SetGenerateOverlapEvents(false);
		 }

		for (int i = 0; i < planetsToSimulate; i++) {

			spawnEdgePlanet();
		}
	}
}

void AMainMenuNBODYmanager::spawnEdgePlanet() {

	//random speed
	FVector speed_ = FVector(0.0f, -SpawnInitialMaxSpeed / 2, -SpawnInitialMaxSpeed / 2);
	speed_.Y += FMath::FRandRange(0, SpawnInitialMaxSpeed);
	speed_.Z += FMath::FRandRange(0, SpawnInitialMaxSpeed);
	speed_.Normalize();

	FVector myLoc = simCentre;
	myLoc += -speed_ * despawnRadiusRW * 0.8f;

	//speed_.RotateAngleAxis(FMath::FRandRange(-30.0f, 30.0f),FVector(1,0,0));

	//random mass
	float mass_ = 0.001f;
	mass_ += FMath::FRandRange(0.0f, SpawnInitialMaxMass);

	FString bodName = "Body ";
	bodName.Append(std::to_string(overallPlanets).c_str());

	FVector4 randColor = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
	randColor.X -= FMath::FRandRange(0, 1.0f);
	randColor.Y -= FMath::FRandRange(0, 1.0f);
	randColor.Z -= FMath::FRandRange(0, 1.0f);

	spawnPlanetAt(myLoc / 1000.0f, speed_, mass_, randColor, bodName, 0, BodyHandler_ref);

	//// Create the Niagara component and attach it to the Actor
	//UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
	//	LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/MyContent/Graphics/PlanetRelated/p_Trail.p_Trail")), // Replace with your Niagara effect path
	//	BodyHandler_ref->myGravBodies.Last()->GetOwner()->GetRootComponent(),
	//	NAME_None,
	//	FVector::ZeroVector,
	//	FRotator::ZeroRotator,
	//	EAttachLocation::KeepRelativeOffset,
	//	true // Auto destroy
	//);

}

// setup function for spawning bodies - creates a new body with specified parameters
ATestPlanet * AMainMenuNBODYmanager::spawnPlanetAt(FVector position_, FVector velocity_, double mass_, FVector4 colour_, FString name_, float radius_, UNBodyHandler* handlerToAddInto)
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
	//newBody->SphereCollider->SetGenerateOverlapEvents(false);
	newBody->SetActorLabel(name_);


	if (radius_ == 0.0f) {
		radius_ = cbrt(mass_);
	}

	//currently more for display purposes
	newBody->SetActorScale3D(FVector(radius_, radius_, radius_));




	//option to set colour too
	newBody->myMat->SetVectorParameterValue(TEXT("Colour"), colour_);
	newBody->GravComp->myCol = colour_;

	handlerToAddInto->myGravBodies.Add(newBody->GravComp);
	overallPlanets++;

	return newBody;
}

void AMainMenuNBODYmanager::deleteDestroyedBodies() {

	for (int i = 1; i < BodyHandler_ref->myGravBodies.Num(); i++)
	{

		UGravBodyComponent* CompIT = BodyHandler_ref->myGravBodies[i];

		CompIT->velocity = CompIT->velocity.GetClampedToMaxSize(maxMMPlanetSpeed);

		if ((CompIT->position*1000.0f - simCentre).Length() > despawnRadiusRW) {

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


	//step 1: Gravitational calculations using fixed time updates
	//dt influenced by simulation time scale 
	double updatedDT = DeltaTime * timeMultiplier * 0.027f; //0.027 makes the time as 10 days/s		

	if (useTreeCodes) {
		BodyHandler_ref->calculateWithTree(updatedDT, false, false);

	}

	BodyHandler_ref->myGravBodies[0]->velocity = FVector(0, 0, 0);


	for (int i = 1; i < BodyHandler_ref->myGravBodies.Num(); i++)
	{
		UGravBodyComponent * CompIT = BodyHandler_ref->myGravBodies[i];


		

		float distanceBasedMaxVel =  (CompIT->GetOwner()->GetActorLocation() - simCentre).Length();

		if (CompIT->velocity.Length() > distanceBasedMaxVel) {
			CompIT->velocity = CompIT->velocity.GetSafeNormal() * distanceBasedMaxVel;
		}


	}

	//step 2: move bodies using their updated velocity, also destroy ones that 
	BodyHandler_ref->moveBodies(true, updatedDT);



}
