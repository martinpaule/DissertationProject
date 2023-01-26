// Fill out your copyright notice in the Description page of Project Settings.

#include <string>
#include "TreeHandler.h"

// Sets default values
ATreeHandler::ATreeHandler()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATreeHandler::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATreeHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (showTreeBoxes) {
		RecalculatePartitioning();
		DisplaySectors();
	}
}

void ATreeHandler::RecalculatePartitioning() {
	
	//reset root
	delete treeNodeRoot;
	treeNodeRoot = new TreeNode;

	//find max and min on XYZ axis
	FVector XYZ_min;
	FVector XYZ_max;
	for (int i = 0; i < bodyHandlerBodies->Num(); i++) {
		if (i == 0) {
			XYZ_min = (*bodyHandlerBodies)[i]->position;
			XYZ_max = (*bodyHandlerBodies)[i]->position;
		}
		else {
			FVector bPos = (*bodyHandlerBodies)[i]->position;

			if (bPos.X < XYZ_min.X) {
				XYZ_min.X = bPos.X;
			}
			if (bPos.Y < XYZ_min.Y) {
				XYZ_min.Y = bPos.Y;
			}
			if (bPos.Z < XYZ_min.Z) {
				XYZ_min.Z = bPos.Z;
			}

			if (bPos.X > XYZ_max.X) {
				XYZ_max.X = bPos.X;
			}
			if (bPos.Y > XYZ_max.Y) {
				XYZ_max.Y = bPos.Y;
			}
			if (bPos.Z > XYZ_max.Z) {
				XYZ_max.Z = bPos.Z;
			}
		}
	}

	//use these max mins to setup first root node
	float extent = XYZ_max.X - XYZ_min.X;
	if (XYZ_max.Y - XYZ_min.Y > extent) {
		extent = XYZ_max.Y - XYZ_min.Y;
	}
	if (XYZ_max.Z - XYZ_min.Z > extent) {
		extent = XYZ_max.Z - XYZ_min.Z;
	}

	extent /= 2.0f;

	treeNodeRoot->position = (XYZ_min + XYZ_max) / 2.0f;
	treeNodeRoot->extent = extent;
	treeNodeRoot->bodies = *bodyHandlerBodies;
	treeNodeRoot->partition();


}

void ATreeHandler::DisplaySectors() {

	treeNodeRoot->displayBoxes(GetWorld());

}