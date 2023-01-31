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

	int level;

	//half size of the cube
	float extent;
	FVector position;
	FColor color = FColor::Green;

	float Node_CombinedMass;
	FVector Node_CentreOMass;


	//helper function to check whether body us in the extent of a given tree node
	bool isInExtent(FVector bodyPos) {
		if (bodyPos.X <= position.X + extent && bodyPos.X > position.X - extent && bodyPos.Y <= position.Y + extent && bodyPos.Y > position.Y - extent && bodyPos.Z <= position.Z + extent && bodyPos.Z > position.Z - extent) {
			return true;
		}
		return false;
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
	void DisplaySectors(TreeNode* rootNode);
	void partitionTree(TreeNode* rootNode);

	//recursive function to get force acting on a given 1 body
	FVector getApproxForce(AGravBody* body, TreeNode* rootNode);

	//visualise oct trees
	UPROPERTY(Category = "VisualisationRelevant", EditAnywhere, BlueprintReadWrite)
		bool showTreeBoxes = false;

	//total gravitational calculations done
	UPROPERTY(Category = "SimulationRelevant", EditAnywhere, BlueprintReadWrite)
	int gravCalcs = 0;
};
