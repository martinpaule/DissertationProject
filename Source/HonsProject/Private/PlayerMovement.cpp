// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMovement.h"

// Sets default values for this component's properties
UPlayerMovement::UPlayerMovement()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPlayerMovement::BeginPlay()
{
	Super::BeginPlay();

	myOwner = GetOwner();
	if (myOwner == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UMyCustomComponent: Owning actor is null!"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("UMyCustomComponent: Owning actor is %s"), *myOwner->GetName());
	}
	
}


// Called every frame
void UPlayerMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}




void UPlayerMovement::MoveForward(float AxisValue) {

	if (!warping) {


		if (AxisValue) {
			myOwner->AddActorWorldOffset(myOwner->GetActorForwardVector() * MoveSpeed * GetWorld()->DeltaTimeSeconds * AxisValue);
		}

		//camHandleComp->MoveForward_CamHandle(AxisValue);


		

	}

}

void UPlayerMovement::MoveRight(float AxisValue) {


	if (AxisValue) {
		myOwner->AddActorLocalRotation(FRotator(0, AxisValue * RotateSpeed * GetWorld()->DeltaTimeSeconds, 0));

	}
	//camHandleComp->MoveRight_CamHandle(AxisValue);

}


void UPlayerMovement::RotateUp(float AxisValue) {





	if (AxisValue) {
		myOwner->AddActorLocalRotation(FRotator(-AxisValue * RotateSpeed * GetWorld()->DeltaTimeSeconds, 0, 0));
	}
	


}



void UPlayerMovement::RotateRight(float AxisValue) {





	myOwner->AddActorLocalRotation(FRotator(0, 0, AxisValue * RotateSpeed * GetWorld()->DeltaTimeSeconds));



}

void UPlayerMovement::ChargeWarp(float AxisValue) {


	if (warping) {
		return;
	}

	if (AxisValue) {
		warpStrength += GetWorld()->DeltaTimeSeconds;
	}
	else {



		if (warpStrength > 0.0f) {
			warpStrength -= GetWorld()->DeltaTimeSeconds;

			//theoretically removable
			if (warpStrength < 0.0f) {
				warpStrength = 0.0f;
			}
		}
	}

}

void UPlayerMovement::handleMovement(float dt)
{

	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, std::to_string(warpStrength).c_str());

	if (warping) {
		warpTimeLeft -= dt;


		myOwner->AddActorWorldOffset(myOwner->GetActorForwardVector() * MoveSpeed * GetWorld()->DeltaTimeSeconds * int(warpStrength) * 3);



		if (warpTimeLeft < 0.0f) {
			warping = false;
			warpStrength = 0;


			partLife = 1.0f;
			slowingWarp = true;
		}
	}

	if (slowingWarp) {
		partLife -= dt;

		if (partLife < 0.0f) {
			slowingWarp = false;
			
		}

	}
}

