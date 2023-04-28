// Fill out your copyright notice in the Description page of Project Settings.

#include "TreeHandler.h"
#include <string>

// Sets default values
UTreeHandler::UTreeHandler()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryComponentTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void UTreeHandler::BeginPlay()
{
	Super::BeginPlay();
	

}

// Called every frame
void UTreeHandler::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


}





void UTreeHandler::RecalculatePartitioning() {
	
	//reset root
	delete treeNodeRoot;

	//also assign its combined mass and centre of mass
	float combinedMass = 0.0f;
	FVector centreOfMass = FVector(0.0f, 0.0f, 0.0f);

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
	
		combinedMass += (*bodyHandlerBodies)[i]->mass;
		centreOfMass += (*bodyHandlerBodies)[i]->position * (*bodyHandlerBodies)[i]->mass;

	}

	centreOfMass /= combinedMass;


	//find a CUBE that is JUST enveloping the planets
	float extent = XYZ_max.X - XYZ_min.X;
	if (XYZ_max.Y - XYZ_min.Y > extent) {
		extent = XYZ_max.Y - XYZ_min.Y;
	}
	if (XYZ_max.Z - XYZ_min.Z > extent) {
		extent = XYZ_max.Z - XYZ_min.Z;
	}
	extent /= 2.0f;


	//create new root and set its variables
	treeNodeRoot = new TreeNode;
	treeNodeRoot->position = (XYZ_min + XYZ_max) / 2.0f;
	treeNodeRoot->level = 1;
	treeNodeRoot->extent = extent;
	treeNodeRoot->bodies = *bodyHandlerBodies;
	

	treeNodeRoot->Node_CentreOMass = centreOfMass;
	treeNodeRoot->Node_CombinedMass = combinedMass;

	//recursively partition the tree
	partitionTree(treeNodeRoot);
}

void UTreeHandler::DisplaySectors(TreeNode* rootNode) {

	if (rootNode->isLeaf) {
		if (rootNode->bodies.Num() == 1) {
			DrawDebugBox(GetWorld(), rootNode->position * 1000.0f, FVector(rootNode->extent, rootNode->extent, rootNode->extent) * 1000.0f, FColor(rootNode->bodies[0]->myCol.X *255, rootNode->bodies[0]->myCol.Y * 255, rootNode->bodies[0]->myCol.Z * 255), false, 0.0f, 0, 15.0f);
		}
		else {
			//DrawDebugBox(worldRef, position * 1000.0f, FVector(extent, extent, extent) * 1000.0f, FColor::Red, false, -1.0f, 0, 2.0f);
		}
	}
	else {

		for (int j = 0; j < 8; j++) {

			DisplaySectors(rootNode->branch_nodes[j]);
		}
	}
}

void UTreeHandler::partitionTree(TreeNode* rootNode)
{
	//exitx if there is 1 or 0 children
	if (rootNode->bodies.Num() == 1) {
		rootNode->bodies[0]->leaf_ref = rootNode;
		return;
	}
	if (rootNode->bodies.Num() == 0) {
		return;
	}


	rootNode->isLeaf = false;

	//clear previous partitions
	rootNode->branch_nodes.Empty();

	//setup 8 children nodes
	for (int i = 0; i < 8; i++) {
		rootNode->branch_nodes.Add(new TreeNode);
		rootNode->branch_nodes.Last()->extent = rootNode->extent / 2.0f;
		rootNode->branch_nodes.Last()->root_node = rootNode;
		rootNode->branch_nodes.Last()->level = rootNode->level + 1;

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


	
	//assign all relevant bodies into this child node, handling their mass and position to calculate their avg pos and comb mass
	for (int k = 0; k < rootNode->bodies.Num(); k++) {

		//loop through the children nodes
		for (int j = 0; j < 8; j++) {


			if (rootNode->branch_nodes[j]->isInExtent(rootNode->bodies[k]->position)) {
				rootNode->branch_nodes[j]->bodies.Add(rootNode->bodies[k]);

				rootNode->branch_nodes[j]->Node_CombinedMass += rootNode->bodies[k]->mass;
				rootNode->branch_nodes[j]->Node_CentreOMass += rootNode->bodies[k]->position * rootNode->bodies[k]->mass;
				break;
			}

		}
	}

	//RECURSIVE CALL to partition branch nodes (also calculate centre of mass
	for (int j = 0; j < 8; j++) {
		if (rootNode->branch_nodes[j]->bodies.Num() > 0) {
			rootNode->branch_nodes[j]->Node_CentreOMass /= rootNode->branch_nodes[j]->Node_CombinedMass;
		}
		partitionTree(rootNode->branch_nodes[j]);
	}
}

//recursive function calculating force on a body
FVector UTreeHandler::getApproxForce(UGravBodyComponent* body, TreeNode * rootNode)
{

	if (rootNode->bodies.Num() == 1 && body != rootNode->bodies[0]) {

		FVector direction = rootNode->bodies[0]->position - body->position;
		float distanceCubed = direction.Length() * direction.Length() * direction.Length();
		FVector returnForce = direction * bigG * rootNode->bodies[0]->mass;
		returnForce /= distanceCubed;

		gravCalcs++;

		return returnForce;
	}
	if (rootNode->bodies.Num() == 0) {
		return FVector(0, 0, 0);
	}
	if (rootNode->bodies.Num() == 1 && body == rootNode->bodies[0]) {

		return FVector(0, 0, 0);
	}

	//necessary variables
	float distance_body_to_centreOfMass = (body->position - rootNode->Node_CentreOMass).Length();
	float accuracy_Param = 1.0f;

	//return of the function
	FVector combinedForces = FVector(0.0f, 0.0f, 0.0f);

	//if the length of the cell is smaller than the distabce of the planet to the cell's centre of mass
	if ((rootNode->extent*2.0f) / distance_body_to_centreOfMass < accuracy_Param) {
	
	
		FVector direction = rootNode->Node_CentreOMass - body->position;
		float distanceCubed = distance_body_to_centreOfMass * distance_body_to_centreOfMass * distance_body_to_centreOfMass;
		FVector returnForce = direction * bigG * rootNode->Node_CombinedMass;
		returnForce /= distanceCubed;
	
		gravCalcs++;
	
		return returnForce;
	}
	else if(!rootNode->isLeaf) {
		for (int j = 0; j < 8; j++) {
			combinedForces += getApproxForce(body, rootNode->branch_nodes[j]);
		}
		
	}

	return combinedForces;
}

TreeNode * UTreeHandler::getLowestSectorOfPos(FVector position) {

	return  searchLowestSectorRecursive(position, treeNodeRoot);
	
}

TreeNode* UTreeHandler::searchLowestSectorRecursive(FVector position, TreeNode* nowTInree) {

	if (nowTInree->isLeaf && nowTInree->isInExtent(position)) {
		return nowTInree;
	}
	for (int j = 0; j < 8; j++) {
		if (nowTInree->branch_nodes[j]->isInExtent(position)) {
			return searchLowestSectorRecursive(position, nowTInree->branch_nodes[j]);
		}
	}
	return NULL;
}

void UTreeHandler::mergeEmptiesAboveMe(TreeNode* rootNode) {

	for (int i = 0; i < 9; i++) {
		if (i == 8) {
			rootNode->root_node->isLeaf = true;
			rootNode->root_node->branch_nodes.Empty();
			mergeEmptiesAboveMe(rootNode->root_node);
		}
		else if (rootNode->root_node->branch_nodes[i]->bodies.Num() > 0) {
			break;
		}
	}
	return;
}