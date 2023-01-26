// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GravBody.h"
#include "TreeHandler.generated.h"

struct TreeNode {
public:
	TreeNode * root_node = nullptr;
	TArray<TreeNode *> branch_nodes;
	TArray<AGravBody*> bodies;
	bool isLeaf = true;

	FVector position;
	float extent;

	FColor color = FColor::Green;

	//helper function to check whether body us in the extent of a given tree node
	bool isInExtent(FVector bodyPos) {
		if (bodyPos.X <= position.X + extent && bodyPos.X > position.X - extent && bodyPos.Y <= position.Y + extent && bodyPos.Y > position.Y - extent && bodyPos.Z <= position.Z + extent && bodyPos.Z > position.Z - extent) {
			return true;
		}
		return false;
	}

	void displayBoxes(UWorld * worldRef) {

		if (isLeaf) {
			if (bodies.Num() == 1) {
				DrawDebugBox(worldRef, position * 1000.0f, FVector(extent, extent, extent) * 1000.0f, bodies[0]->myCol, false, 0.0f, 0, 7.0f);
			}
			else {
				DrawDebugBox(worldRef, position * 1000.0f, FVector(extent, extent, extent) * 1000.0f, FColor::Red, false, -1.0f, 0, 2.0f);
			}
		}
		else {
			for (int j = 0; j < 8; j++) {

				branch_nodes[j]->displayBoxes(worldRef);
			}
			//DrawDebugBox(worldRef, position * 1000.0f, FVector(extent, extent, extent) * 1000.0f, FColor::White, false, -1.0f, 0, 4.0f);
		}
	}

	void poiterCleanup() {
		
		if (isLeaf) {
			return;
		}

		for (int j = 0; j < 8; j++) {
			if (!branch_nodes[j]->isLeaf) {
				branch_nodes[j]->poiterCleanup();
			}
			delete branch_nodes[j];
		}
	}

	void partition() {
		//exit if there is 1 or 0 children
		if (bodies.Num() < 2) {
			return;
		}

		isLeaf = false;

		//setup 8 children nodes
		for (int i = 0; i < 8; i++) {
			branch_nodes.Add(new TreeNode);
			branch_nodes.Last()->extent = extent / 2.0f;
			branch_nodes.Last()->root_node = this;
		}

		float childOffs = extent / 2.0f;

		branch_nodes[0]->position = position + FVector(childOffs, childOffs, childOffs);
		branch_nodes[1]->position = position + FVector(childOffs, childOffs, -childOffs);
		branch_nodes[2]->position = position + FVector(-childOffs, childOffs, childOffs);
		branch_nodes[3]->position = position + FVector(-childOffs, childOffs, -childOffs);
		branch_nodes[4]->position = position + FVector(childOffs, -childOffs, childOffs);
		branch_nodes[5]->position = position + FVector(childOffs, -childOffs, -childOffs);
		branch_nodes[6]->position = position + FVector(-childOffs, -childOffs, childOffs);
		branch_nodes[7]->position = position + FVector(-childOffs, -childOffs, -childOffs);

		//iterate through all bodies in this node
		for (int k = 0; k < bodies.Num(); k++) {

			//decide which child node to attach it to
			for (int j = 0; j < 8; j++) {
				if (branch_nodes[j]->isInExtent(bodies[k]->position)) {
					branch_nodes[j]->bodies.Add(bodies[k]);
					break;
				}
			}
		}

		//RECURSIVE CALL
		//decide which child node to attach it to
		for (int j = 0; j < 8; j++) {
			branch_nodes[j]->partition();
		}

	}
};


UCLASS()
class HONSPROJECT_API ATreeHandler : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATreeHandler();

	//pointer to the array from nbodyHandler
	TArray<AGravBody*> * bodyHandlerBodies;
	TreeNode * treeNodeRoot;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void RecalculatePartitioning();
	void DisplaySectors();

	UPROPERTY(Category = "VisualisationRelevant", EditAnywhere, BlueprintReadWrite)
		bool showTreeBoxes = false;
};
