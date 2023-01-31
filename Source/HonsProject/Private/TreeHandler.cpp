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

	RecalculatePartitioning();
	if (showTreeBoxes) {
		DisplaySectors(treeNodeRoot);
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
	treeNodeRoot->level = 1;
	treeNodeRoot->extent = extent;
	treeNodeRoot->bodies = *bodyHandlerBodies;
	
	float combinedMass = 0.0f;
	FVector centreOfMass = FVector(0.0f, 0.0f, 0.0f);
	for (int k = 0; k < treeNodeRoot->bodies.Num(); k++) {

			combinedMass += treeNodeRoot->bodies[k]->mass;
			centreOfMass += treeNodeRoot->bodies[k]->position * treeNodeRoot->bodies[k]->mass;
	}

	centreOfMass /= combinedMass;
	treeNodeRoot->Node_CentreOMass = centreOfMass;
	treeNodeRoot->Node_CombinedMass = combinedMass;

	partitionTree(treeNodeRoot);



}

void ATreeHandler::DisplaySectors(TreeNode* rootNode) {

	if (rootNode->isLeaf) {
		if (rootNode->bodies.Num() == 1) {
			DrawDebugBox(GetWorld(), rootNode->position * 1000.0f, FVector(rootNode->extent, rootNode->extent, rootNode->extent) * 1000.0f, rootNode->bodies[0]->myCol, false, 0.0f, 0, 7.0f);
		}
		else {
			//DrawDebugBox(worldRef, position * 1000.0f, FVector(extent, extent, extent) * 1000.0f, FColor::Red, false, -1.0f, 0, 2.0f);
		}
	}
	else {

		if (rootNode->level == 3) {
			//DrawDebugBox(GetWorld(), rootNode->position * 1000.0f, FVector(rootNode->extent, rootNode->extent, rootNode->extent) * 1000.0f, FColor::White, false, 0.0f, 0, 7.0f);

			//DrawDebugString(GetWorld(), rootNode->position * 1000.0f, FString("Average position: ").Append(rootNode->Node_CentreOMass.ToString()), this, FColor::White, 0.0f, false, 0.7f);
			//DrawDebugString(GetWorld(), rootNode->position * 1000.0f - FVector(0,0,500.0f), FString("Combined Mass: ").Append(FString::SanitizeFloat(rootNode->Node_CombinedMass)), this, FColor::White, 0.0f, false, 0.7f);

		}


		for (int j = 0; j < 8; j++) {



			DisplaySectors(rootNode->branch_nodes[j]);
		}
	}
}

void ATreeHandler::partitionTree(TreeNode* rootNode)
{
	//exit if there is 1 or 0 children
	if (rootNode->bodies.Num() < 2) {
		return;
	}

	rootNode->isLeaf = false;

	//setup 8 children nodes
	for (int i = 0; i < 8; i++) {
		rootNode->branch_nodes.Add(new TreeNode);
		rootNode->branch_nodes.Last()->extent = rootNode->extent / 2.0f;
		rootNode->branch_nodes.Last()->root_node = rootNode;
	}

	float childOffs = rootNode->extent / 2.0f;

	//create oct-tree children with their offsets
	rootNode->branch_nodes[0]->position = rootNode->position + FVector(childOffs, childOffs, childOffs);
	rootNode->branch_nodes[1]->position = rootNode->position + FVector(childOffs, childOffs, -childOffs);
	rootNode->branch_nodes[2]->position = rootNode->position + FVector(-childOffs, childOffs, childOffs);
	rootNode->branch_nodes[3]->position = rootNode->position + FVector(-childOffs, childOffs, -childOffs);
	rootNode->branch_nodes[4]->position = rootNode->position + FVector(childOffs, -childOffs, childOffs);
	rootNode->branch_nodes[5]->position = rootNode->position + FVector(childOffs, -childOffs, -childOffs);
	rootNode->branch_nodes[6]->position = rootNode->position + FVector(-childOffs, -childOffs, childOffs);
	rootNode->branch_nodes[7]->position = rootNode->position + FVector(-childOffs, -childOffs, -childOffs);

	//decide which child node to attach it to, also calclate centre of- and combined mass of the node
	for (int j = 0; j < 8; j++) {

		float combinedMass = 0.0f;
		FVector centreOfMass = FVector(0.0f, 0.0f, 0.0f);

		for (int k = 0; k < rootNode->bodies.Num(); k++) {
			if (rootNode->branch_nodes[j]->isInExtent(rootNode->bodies[k]->position)) {
				rootNode->branch_nodes[j]->bodies.Add(rootNode->bodies[k]);

				combinedMass += rootNode->bodies[k]->mass;
				centreOfMass += rootNode->bodies[k]->position * rootNode->bodies[k]->mass;
			}
		}

		centreOfMass /= combinedMass;
		rootNode->branch_nodes[j]->Node_CentreOMass = centreOfMass;
		rootNode->branch_nodes[j]->Node_CombinedMass = combinedMass;
		rootNode->branch_nodes[j]->level = rootNode->level + 1;
	}

	//RECURSIVE CALL
	//decide which child node to attach it to
	for (int j = 0; j < 8; j++) {
		partitionTree(rootNode->branch_nodes[j]);
	}
}

FVector ATreeHandler::getApproxForce(AGravBody* body, TreeNode * rootNode)
{
	float distance_body_to_centreOfMass = (body->position - rootNode->Node_CentreOMass).Length();
	float accuracy_Param = 1.0f;


	FVector combinedForces = FVector(0.0f, 0.0f, 0.0f);

	if (rootNode->bodies.Num() == 0) {
		return combinedForces;
	}


	if ((rootNode->extent*2.0f) / distance_body_to_centreOfMass < accuracy_Param) {
	
		double bigG = 39.4784f; //when using SolarMass, AU and Years
	
		FVector direction = rootNode->Node_CentreOMass - body->position;
		float distanceCubed = distance_body_to_centreOfMass * distance_body_to_centreOfMass * distance_body_to_centreOfMass;
		FVector returnForce = direction * bigG * rootNode->Node_CombinedMass;
		returnForce /= distanceCubed;
	
		gravCalcs++;
	
		return returnForce;
	}
	else {
		if (!rootNode->isLeaf) {
			for (int j = 0; j < 8; j++) {
				combinedForces += getApproxForce(body, rootNode->branch_nodes[j]);
			}
		}
	}

	return combinedForces;
}
