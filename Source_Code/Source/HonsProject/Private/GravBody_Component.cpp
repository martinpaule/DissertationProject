// Fill out your copyright notice in the Description page of Project Settings.


#include "GravBody_Component.h"

// Sets default values for this component's properties
UGravBodyComponent::UGravBodyComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	myCol = FVector4(1.0f, 0.0f, 1.0f, 1.0f);

}


// Called when the game starts
void UGravBodyComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UGravBodyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

