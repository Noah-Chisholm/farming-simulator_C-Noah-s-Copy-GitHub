// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/BoxComponent.h>
#include "tileGenCollisions_C_bp.generated.h"

UCLASS()
class FARMINGSIMULATOR_API AtileGenCollisions_C_bp : public AActor
{
	GENERATED_BODY()
	
public:	 
	UFUNCTION()
		void printDebug(FString stringToPrint);
	// Sets default values for this actor's properties
	AtileGenCollisions_C_bp();
	//declare a UBoxComponent called playerDetector
	//this is for detecting when the player enters and exit the chunk to determine shouldGen value
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TObjectPtr<UBoxComponent> playerDetector;
	//declarte USceneComponent called root
	//this is for the root component
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TObjectPtr<USceneComponent> root;
	//declare a UBoxComponent called northExit
	//this is for detecting when the player exits the chunk going north
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TObjectPtr<UBoxComponent> northExit;
	//declare a UBoxComponent called southExit
	//this is for detecting when the player exits the chunk going south
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TObjectPtr<UBoxComponent> southExit;
	//declare a UBoxComponent called eastExit
	//this is for detecting when the player exits the chunk going east
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TObjectPtr<UBoxComponent> eastExit;
	//declare a UBoxComponent called westExit
	//this is for detecting when the player exits the chunk going west
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TObjectPtr<UBoxComponent> westExit;
	//declare an array of class references for the resource managers called worldManagers
	//this is for finding the blueprint actors in the level for adding and removing instances
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AActor> worldManagerClass;
	//player class
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AActor> playerClass;
	UPROPERTY()
		AActor* worldManager;
	//init the shouldGen var
	//this is for storing whether or not this actor can currently begin procedural generation
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool shouldGen = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName exitDir;
protected:
	UFUNCTION()
		virtual void BeginPlay() override;
	//event to make the blueprint spawn an enemy
	UFUNCTION(BlueprintImplementableEvent)
		void genMonster(FTransform transform);
	//events to change x and y
	UFUNCTION(BlueprintImplementableEvent)
		void changeX(bool isPositive);
	UFUNCTION(BlueprintImplementableEvent)
		void changeY(bool isPositive);
private:
	//init biomeID
	// this is for storing the id of the biome for the generated chunk
	//0 = woodlands - 
	//1 = stones
	//2 = water
	//3 = ruins
	//4 = infested
	//5 event
	//6 normal 
	int32 biomeID = 0;
	//init curTileX and Y
	//this is for storing the current tile location within the current chunk
	int32 curTileX = 0;
	int32 curTileY = 0;
	//init curChunkIndex
	//this is for storing the current generated chunk out of the chunksToGenTotal
	int32 curChunkIndex = 0;
	//init a Timer handle called tileGenTimer
	//this is for storing the timer until next tick if the tilesToGenPerTick limit is reached
	FTimerHandle tileGenTimer;
	//init curTileIndex
	//this is for storing the number of tiles generated in this tick to check against the tilesToGenPerTick limit
	int32 curTileIndex = 0;
	//init a few const values for processing
	// generation chances
	//the chance for a tree to spawn
	const float treeGenChanceForTile = 0.01; // 1% approx every 100 tiles should have a tree
	//the chance for a rock to spawn
	const float rockGenChanceForTile = 0.01; // 1% approx every 100 tiles should have a rock
	//the chance for a monster to spawn
	const float monsterGenChanceForTile = 0.001; // 0.1% chance every 1000 tiles should have a monster
	//the offset from the pivot of this actor to the corner of the 'chunk'
	const int32 cornerToCenterChunkXOffset = 1300;
	const int32 cornerToCenterChunkYOffset = 1300;
	//the size of the tiles
	const int32 tileSizeX = 100;
	const int32 tileSizeY = 100;
	//the count limits of generation
	//this is zero indexed as such all are one less than the number they are intended, ie: 4 is 5
	//the count of chunks to gen
	const int32 chunksToGen = 4; //generation zone is 5 tiles wide
	//size of the chunks in tiles
	const int32 xTilesPerChunk = 24; // each chunk is 25 by
	const int32 yTilesPerChunk = 24; //25
	//the number of times to loop through the deletion process
	const int32 endLoops = 10;
	//end of zero indexing
	//calculate the length and width in unreal units of a chunk
	//init vars to hold those values in both the x and y dimensions
	const double chunkLengthX = tileSizeX * (xTilesPerChunk + 1);
	const double chunkLengthY = tileSizeY * (yTilesPerChunk + 1);
	//this is depreciated it was for limiting processes when it was suboptimized but now it runs smoothly
	// I am keeping it and the functionality just incase though
	//init the number of tiles to gen per 
	const int32 tilesToGenPerTick = (xTilesPerChunk + 1) * (yTilesPerChunk + 1) * (chunksToGen + 1); //currently equal to the number of tiles in a single generation zone
	//init consts specific to the north generation process
	//init northPlacementOffsets
	//these are for finding the starting chunks starting corner in the north generation direction
	//shouldn't change unless generated map is made larger than 5 x 5 chunks
	const int32 northXPlacementOffset = 7600;
	const int32 northYPlacementOffset = -4900;
	//init northDestructionBoundsOffset
	//the offset required to reach the center of the northDestruction bounds from this 'chunk's corner
	const FVector northDestructionBoundsOffset = FVector(3850, 1250, 0);
	//init northDestructionExtent
	//the half extent of the north destruction bounds
	const FVector northDestructionExtent = FVector(1245, 6245, 3);
	//init consts specific to the south generation process
	//init southPlacementOffsets
	//these are for finding the starting chunks starting corner in the south generation direction
	//shouldn't change unless generated map is made larger than 5 x 5 chunks
	const int32 southXPlacementOffset = -7400;
	const int32 southYPlacementOffset = -4900;
	//init southDestructionBoundsOffset
	//the offset required to reach the center of the southDestruction bounds from this 'chunk's corner
	const FVector southDestructionBoundsOffset = FVector(-6250, 1250, 0);
	//init southDestructionExtent
	//the half extent of the south destruction bounds
	const FVector southDestructionExtent = FVector(1245, 6245, 3);
	//init consts specific to the east generation process
	//init eastPlacementOffsets
	//these are for finding the starting chunks starting corner in the south generation direction
	//shouldn't change unless generated map is made larger than 5 x 5 chunks
	const int32 eastXPlacementOffset = -4900;
	const int32 eastYPlacementOffset = 7500;
	//init southDestructionBoundsOffset
	//the offset required to reach the center of the southDestruction bounds from this 'chunk's corner
	const FVector eastDestructionBoundsOffset = FVector(-1250, 6250, 0);
	//init eastDestructionExtent
	//the half extent of the east destruction bounds
	const FVector eastDestructionExtent = FVector(6245, 1245, 3);
	//init consts specific to the west generation process
	//init westPlacementOffsets
	//these are for finding the starting chunks starting corner in the south generation direction
	//shouldn't change unless generated map is made larger than 5 x 5 chunks
	const int32 westXPlacementOffset = -4900;
	const int32 westYPlacementOffset = -7400;
	//init southDestructionBoundsOffset
	//the offset required to reach the center of the southDestruction bounds from this 'chunk's corner
	const FVector westDestructionBoundsOffset = FVector(-1250, -4000, 0);
	//init eastDestructionExtent
	//the half extent of the east destruction bounds
	const FVector westDestructionExtent = FVector(6245, 1245, 3);
	//declare a few private functions
	//function to determine the next biomeID
	UFUNCTION()
		int32 newBiomeID();
	//function to determine if this actor should begin generation and manage the state of variable shouldGen 
	UFUNCTION()
		bool checkShouldGen(AActor* actorOverlapped);
	//function for the process of spawning a single tile once the transform function has been ran
	UFUNCTION()
		bool spawnTile(FTransform tileTransform);
	//function to check if the curTileX and Y values are half of their limits
	UFUNCTION()
		bool checkIfTileCenter();
	//function to attempt to spawn a new actor of this class if checkIfTileCenter is true
	UFUNCTION()
		void spawnTileGenCollision(FTransform transform);
	//function to attempt to spawn a resource
	UFUNCTION()
		FTransform randomizeRotation(FTransform transformToChange);
	UFUNCTION()
		void tryResourceGen(FTransform transform);
	//function to attempt to spawn a tree at the current tile's location if it passes the chance to spawn a tree defined by variable treeGenChanceForTile
	UFUNCTION()
		bool tryTreeGen(FTransform transform);
	//function to attempt to spawn a rock at the current tile's location if it passes the chance to spawn a rock defined by variable rockGenChanceForTile
	UFUNCTION()
		bool tryRockGen(FTransform transform);
	//function to attempt to spawn a monster at the current tile's location if it passes the chance to spawn a rock defined by variable monsterSpawnChanceForTile
	UFUNCTION()
		bool tryMonsterGen(FTransform transform);
	//functions for player detector
	//player detector
	//a linked function to the OnComponentBeginOVerlap for the UBoxComponent 'playerDetector'
	UFUNCTION()
		void playerDetectorBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//a linked function to the OnComponentBeginOVerlap for the UBoxComponent 'playerDetector'
	UFUNCTION()
		void playerDetectorEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	//functions specifically for north generation
	//northFunctions
	//the process of deleting the southern section of map when exiting north
	UFUNCTION()
		void endNorth();
	//a linked function to the OnComponentBeginOverlap for the UBoxComponent 'northExit'
	//checks checkShouldGen and if true begins the north process
	UFUNCTION()
		void northOverlapped(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//a function to generate the current transform for a tile generating to the north
	UFUNCTION()
		FTransform northGetTileTransform();
	//the function that will loop through the current tile and chunk counts to call the spawnTile function with the northGetTileTransform result
	UFUNCTION()
	void northProcess(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//function to set a timer for the next tick and restart the north generation process upon timer completion
	UFUNCTION()
	void setTimerNorth(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//functions specifically for south generation
	//southFunctions
	//the process of deleting the southern section of map when exiting south
	UFUNCTION()
	void endSouth();
	//a linked function to the OnComponentBeginOverlap for the UBoxComponent 'southExit'
	//checks checkShouldGen and if true begins the south process
	UFUNCTION()
	void southOverLapped(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//a function to generate the current transform for a tile generating to the south
	UFUNCTION()
	FTransform southGetTileTransform();
	//the function that will loop through the current tile and chunk counts to call the spawnTile function with the southGetTileTransform result
	UFUNCTION()
	void southProcess(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//function to set a timer for the next tick and restart the south generation process upon timer completion
	UFUNCTION()
	void setTimerSouth(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);	//functions specifically for south generation
	//eastFunctions
	//the process of deleting the eastern section of map when exiting east
	UFUNCTION()
	void endEast();
	//a linked function to the OnComponentBeginOverlap for the UBoxComponent 'eastExit'
	//checks checkShouldGen and if true begins the east process
	UFUNCTION()
	void eastOverLapped(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//a function to generate the current transform for a tile generating to the east
	UFUNCTION()
	FTransform eastGetTileTransform();
	//the function that will loop through the current tile and chunk counts to call the spawnTile function with the eastGetTileTransform result
	UFUNCTION()
	void eastProcess(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//function to set a timer for the next tick and restart the east generation process upon timer completion
	UFUNCTION()
	void setTimerEast(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);	//eastFunctions
	//westFunctions
	//the process of deleting the eastern section of map when exiting east
	UFUNCTION()
	void endWest();
	//a linked function to the OnComponentBeginOverlap for the UBoxComponent 'eastExit'
	//checks checkShouldGen and if true begins the east process
	UFUNCTION()
	void westOverLapped(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//a function to generate the current transform for a tile generating to the east
	UFUNCTION()
	FTransform westGetTileTransform();
	//the function that will loop through the current tile and chunk counts to call the spawnTile function with the eastGetTileTransform result
	UFUNCTION()
	void westProcess(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//function to set a timer for the next tick and restart the east generation process upon timer completion
	UFUNCTION()
	void setTimerWest(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
