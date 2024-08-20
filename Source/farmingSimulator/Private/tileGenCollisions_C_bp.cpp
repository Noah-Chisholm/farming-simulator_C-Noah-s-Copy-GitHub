// Fill out your copyright notice in the Description page of Project Settings.

/*
want to do biomed chunks
ruins
compass
exit
compass points to exit
monsters
exit is relocated if it is deleted
events
*/
#include "tileGenCollisions_C_bp.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Async/Async.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Kismet/KismetStringLibrary.h"

void AtileGenCollisions_C_bp::printDebug(FString stringToPrint)
{
	//UKismetSystemLibrary::PrintString(GetWorld(), stringToPrint, true, true, FLinearColor(0.0F, 0.66F, 1.0F), 999);
}

// Sets default values
AtileGenCollisions_C_bp::AtileGenCollisions_C_bp()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	//rules for attaching components to roots
	FAttachmentTransformRules rules{
		EAttachmentRule::KeepRelative,
		false
	};
	//create root
	{
		root = CreateDefaultSubobject<USceneComponent>(TEXT("root"));
		SetRootComponent(root);
	}
	//init player detector
	{
		playerDetector = CreateDefaultSubobject<UBoxComponent>(TEXT("playerDetector"));
		playerDetector->AttachToComponent(root, rules);
		playerDetector->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		playerDetector->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		playerDetector->SetBoxExtent(FVector(100, 100, 100));
		playerDetector->SetRelativeScale3D(FVector(12.35, 12.35, 2.5));
		playerDetector->SetRelativeLocation(FVector(0, 0, 250));
		playerDetector->OnComponentBeginOverlap.AddDynamic(this, &AtileGenCollisions_C_bp::playerDetectorBeginOverlap);
		playerDetector->OnComponentEndOverlap.AddDynamic(this, &AtileGenCollisions_C_bp::playerDetectorEndOverlap);
	}
	//init north exit
	{
		northExit = CreateDefaultSubobject<UBoxComponent>(TEXT("northExit"));
		northExit->AttachToComponent(root, rules);
		northExit->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		northExit->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		northExit->SetBoxExtent(FVector(100, 100, 100));
		northExit->SetRelativeScale3D(FVector(0.05, 12.4, 2.5));
		northExit->SetRelativeLocation(FVector(1245, 0, 250));
		northExit->OnComponentBeginOverlap.AddDynamic(this, &AtileGenCollisions_C_bp::northOverlapped);
	}
	//init south exit
	{
		southExit = CreateDefaultSubobject<UBoxComponent>(TEXT("southExit"));
		southExit->AttachToComponent(root, rules);
		southExit->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		southExit->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		southExit->SetBoxExtent(FVector(100, 100, 100));
		southExit->SetRelativeScale3D(FVector(0.05, 12.4, 2.5));
		southExit->SetRelativeLocation(FVector(-1245, 0, 250));
		southExit->OnComponentBeginOverlap.AddDynamic(this, &AtileGenCollisions_C_bp::southOverLapped);
	}
	//init east exit
	{
		eastExit = CreateDefaultSubobject<UBoxComponent>(TEXT("eastExit"));
		eastExit->AttachToComponent(root, rules);
		eastExit->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		eastExit->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		eastExit->SetBoxExtent(FVector(100, 100, 100));
		eastExit->SetRelativeTransform(FTransform(FRotator(0, 90, 0), FVector(0, 1245, 250), FVector(0.05, 12.4, 2.5)));
		eastExit->OnComponentBeginOverlap.AddDynamic(this, &AtileGenCollisions_C_bp::eastOverLapped);
	}
	//init west exit
	{
		westExit = CreateDefaultSubobject<UBoxComponent>(TEXT("westExit"));
		westExit->AttachToComponent(root, rules);
		westExit->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		westExit->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		westExit->SetBoxExtent(FVector(100, 100, 100));
		westExit->SetRelativeTransform(FTransform(FRotator(0, 90, 0), FVector(0, -1245, 250), FVector(0.05, 12.4, 2.5)));
		westExit->OnComponentBeginOverlap.AddDynamic(this, &AtileGenCollisions_C_bp::westOverLapped);
	}
	//TArray<AActor*> tempWorldManagerArray;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), worldManagerClass, tempWorldManagerArray);
	//worldManager = tempWorldManagerArray[0];
}

void AtileGenCollisions_C_bp::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> tempWorldManagerArray; 
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), worldManagerClass, tempWorldManagerArray);
	worldManager = tempWorldManagerArray[0];
}

int32 AtileGenCollisions_C_bp::newBiomeID()
{
	//printDebug(UKismetStringLibrary::Conv_IntToString(biomeID));
	float percent = UKismetMathLibrary::RandomIntegerInRange(1, 100);
	if (percent <= 40) {
		biomeID = 6;
	}
	if (percent <= 57 && percent > 40) {
		biomeID = 0;
	}
	if (percent <= 74 && percent > 57) {
		biomeID = 1;
	}
	if (percent <= 86 && percent > 74) {
		biomeID = 2;
	}
	if (percent <= 97 && percent > 86) {
		biomeID = 3;
	}
	if (percent <= 99 && percent > 97) {
		biomeID = 4;
	}
	if (percent == 100) {
		biomeID = 5;
	}
	return biomeID;
}

//check if the requirments for procedural generation are met
bool AtileGenCollisions_C_bp::checkShouldGen(AActor* actorOverlapped)
{
	printDebug(playerClass.Get()->GetName());
	printDebug(actorOverlapped->GetClass()->GetName());
	printDebug(UKismetStringLibrary::Conv_BoolToString(actorOverlapped->GetClass() == playerClass));
	//check if shouldGen is true
	if (shouldGen && (actorOverlapped->GetClass() == playerClass)) {
		printDebug("Should Gen");
		//if true set to false
		shouldGen = false;
		//return true because it should gen
		return true;
	}
	//other wise return false because it should not gen
	return false;
}

//destroy the section of map behind the player and reset values
void AtileGenCollisions_C_bp::endNorth()
{
	//reset curChunkIndex for next generation
	curChunkIndex = 0;
	//start the destruction of the previous tiles
	AsyncTask(ENamedThreads::GameThread, [this]() {
		//array for storing the static meshes to be removed
		TArray<int> overlappedTerrain;
		//get references to the world meshes
		//UKismetSystemLibrary::PrintString(GetWorld(), UKismetStringLibrary::Conv_BoolToString(worldManager != nullptr));
		UHierarchicalInstancedStaticMeshComponent* grassTiles = Cast<UHierarchicalInstancedStaticMeshComponent>(worldManager->FindComponentByTag(UHierarchicalInstancedStaticMeshComponent::StaticClass(), FName("grassTile")));
		UHierarchicalInstancedStaticMeshComponent* basicTrees = Cast<UHierarchicalInstancedStaticMeshComponent>(worldManager->FindComponentByTag(UHierarchicalInstancedStaticMeshComponent::StaticClass(), FName("basicTree")));
		//declare a reference to store the tileGenCollision objects to destroy
		TArray<AActor*> tileGenCollisions;
		//define the area to delete
		FBox box = FBox::BuildAABB(GetActorLocation() + FVector(-cornerToCenterChunkXOffset + tileSizeX, cornerToCenterChunkYOffset - tileSizeY, 0) - northDestructionBoundsOffset, northDestructionExtent);
		//DrawDebugBox(GetWorld(), box.GetCenter(), box.GetExtent(), FColor(0, 0, 0), true, 99);
		//init an array of object types to overlap
		TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
		objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
		objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
		objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Visibility));
		//declare the array for actors to ignore should that be nessisary
		TArray<AActor*> actorsToIgnore;
		actorsToIgnore.Add(worldManager);
		//get the grass tile indexes to delete
		for (int i = 0; i <= endLoops; i++) {
			for (UActorComponent* curMeshes : worldManager->GetComponentsByTag(UHierarchicalInstancedStaticMeshComponent::StaticClass(), FName("instancedMeshes"))) {
				overlappedTerrain = (Cast<UHierarchicalInstancedStaticMeshComponent>(curMeshes)->GetInstancesOverlappingBox(box, true));
				overlappedTerrain.Sort();
				//delete the grass tiles
				for (int cur : overlappedTerrain) {
					Cast<UHierarchicalInstancedStaticMeshComponent>(curMeshes)->RemoveInstance(cur);
				}
			} 
			/*
			//get the basic trees indexes to delete
			overlappedTerrain = basicTrees->GetInstancesOverlappingBox(box, true);
			overlappedTerrain.Sort();
			//delete the basic trees
			for (int cur : overlappedTerrain) {
				basicTrees->RemoveInstance(cur);
			}*/
			//get and destroy the tileGenCollisions to destroy
			UKismetSystemLibrary::BoxOverlapActors(GetWorld(), box.GetCenter(), box.GetExtent(), objectTypes, AActor::StaticClass(), actorsToIgnore, tileGenCollisions);
			if (tileGenCollisions.Num() != 0) {
				for (AActor* cur : tileGenCollisions) {
					if (cur != nullptr)
					{
						cur->Destroy();
					}
				}
			}
		}
	});
}

//get the transform for the current tile to create 
FTransform AtileGenCollisions_C_bp::northGetTileTransform()
{
	//get the corner of the 'chunk' associated with this set of 'chunk' collisions
	FVector cornerChunkLocation = GetActorLocation() - FVector(cornerToCenterChunkXOffset, cornerToCenterChunkYOffset, 0);
	//get the tile off set based on the current tile being created.
	double chunkRelativeXTileLocation = curTileX * tileSizeX;
	double chunkRelativeYTileLocation = curTileY * tileSizeY;
	//get the offset based on the current chunk being created
	double chunkOffset = curChunkIndex * chunkLengthY;
	//add the placement offset to get the corner of tiles to generate in the north
	//add the location of the chunk
	//add the location of the tile within the chunk
	//do this for both x and y
	double curTileXLocation = chunkRelativeXTileLocation + northXPlacementOffset + cornerChunkLocation.X;
	double curTileYLocation = chunkRelativeYTileLocation + northYPlacementOffset + cornerChunkLocation.Y + chunkOffset;
	//create and return the transform/location
	return FTransform(FRotator(0, 0, 0), FVector(curTileXLocation, curTileYLocation, 0), FVector(1, 1, 1));
}

//run the tile creation loop
void AtileGenCollisions_C_bp::northProcess(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//loop through chunk location 0 - chunksToGen
	while (curChunkIndex <= chunksToGen) {
		newBiomeID();
		//loop through the y locations of tiles within a chunk
		while (curTileY <= yTilesPerChunk) {
			//loop through the x locations of tiles within a chunk/the x locations of a y location column
			while (curTileX <= xTilesPerChunk) {
				//spawn a tile using the northGetTileTransform
				if (spawnTile(northGetTileTransform())) {
					//if true the spawn limit of chunks for this tick has been reached
					//call set timer to set the timer for the next tick
					setTimerNorth(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
					//end tile generation for this tick
					return;
				}
				//after the tile is spawned increment the curTileX value for the next tile
				curTileX++;
			}
			//the the column of x values for this y value is looped through
			//increment the curTileY value for the next column
			curTileY++;
			//reset the curTileX value for the next column
			curTileX = 0;
		}
		//after the x and y locations of have been looped through for the current chunk
		//increment curChunkIndex for the next chunk		
		curChunkIndex++;
		//reset the curTileY value for the next chunk
		curTileY = 0;
	}
}

void AtileGenCollisions_C_bp::endSouth()
{
	//reset curChunkIndex for next generation
	curChunkIndex = 0;
	//start the destruction of the previous tiles
	AsyncTask(ENamedThreads::GameThread, [this]() {
		//array for storing the static meshes to be removed
		TArray<int> overlappedTerrain;
		//get references to the world meshes
		//UKismetSystemLibrary::PrintString(GetWorld(), UKismetStringLibrary::Conv_BoolToString(worldManager != nullptr));
		UHierarchicalInstancedStaticMeshComponent* grassTiles = Cast<UHierarchicalInstancedStaticMeshComponent>(worldManager->FindComponentByTag(UHierarchicalInstancedStaticMeshComponent::StaticClass(), FName("grassTile")));
		UHierarchicalInstancedStaticMeshComponent* basicTrees = Cast<UHierarchicalInstancedStaticMeshComponent>(worldManager->FindComponentByTag(UHierarchicalInstancedStaticMeshComponent::StaticClass(), FName("basicTree")));
		//declare a reference to store the tileGenCollision objects to destroy
		TArray<AActor*> tileGenCollisions;
		//define the area to delete
		FBox box = FBox::BuildAABB(GetActorLocation() + FVector(-cornerToCenterChunkXOffset + tileSizeX, cornerToCenterChunkYOffset - tileSizeY, 0) - southDestructionBoundsOffset, southDestructionExtent);
		//DrawDebugBox(GetWorld(), box.GetCenter(), box.GetExtent(), FColor(0, 0, 0), true, 99);
		//init an array of object types to overlap
		TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
		objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
		objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
		objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Visibility));
		//declare the array for actors to ignore should that be nessisary
		TArray<AActor*> actorsToIgnore;
		actorsToIgnore.Add(worldManager);
		//get the grass tile indexes to delete
		for (int i = 0; i <= endLoops; i++) {
			for (UActorComponent* curMeshes : worldManager->GetComponentsByTag(UHierarchicalInstancedStaticMeshComponent::StaticClass(), FName("instancedMeshes"))) {
				overlappedTerrain = (Cast<UHierarchicalInstancedStaticMeshComponent>(curMeshes)->GetInstancesOverlappingBox(box, true));
				overlappedTerrain.Sort();
				//delete the grass tiles
				for (int cur : overlappedTerrain) {
					Cast<UHierarchicalInstancedStaticMeshComponent>(curMeshes)->RemoveInstance(cur);
				}
			}
			/*
			//get the basic trees indexes to delete
			overlappedTerrain = basicTrees->GetInstancesOverlappingBox(box, true);
			overlappedTerrain.Sort();
			//delete the basic trees
			for (int cur : overlappedTerrain) {
				basicTrees->RemoveInstance(cur);
			}*/
			//get and destroy the tileGenCollisions to destroy
			UKismetSystemLibrary::BoxOverlapActors(GetWorld(), box.GetCenter(), box.GetExtent(), objectTypes, AActor::StaticClass(), actorsToIgnore, tileGenCollisions);
			if (tileGenCollisions.Num() != 0) {
				for (AActor* cur : tileGenCollisions) {
					if (cur != nullptr)
					{
						cur->Destroy();
					}
				}
			}
		}
	});
}

void AtileGenCollisions_C_bp::southOverLapped(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//enter async thread for procedural generation
	//AsyncTask(ENamedThreads::AnyThread, [OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult, this]()
	//	{
			//printDebug("South Overlapped");
			//call checkShouldGen
			if (checkShouldGen(OtherActor)) {
				//printDebug("southActive");
				//if checkShouldGen is true begin procedural generation to the north
				//remove tiles to the south
				exitDir = "south";
				changeX(false);
				endSouth();
				//begin generation of tiles to the north
				southProcess(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
			}
	//});
}

FTransform AtileGenCollisions_C_bp::southGetTileTransform()
{
	//get the corner of the 'chunk' associated with this set of 'chunk' collisions
	FVector cornerChunkLocation = GetActorLocation() - FVector(cornerToCenterChunkXOffset, cornerToCenterChunkYOffset, 0);
	//get the tile off set based on the current tile being created.
	double chunkRelativeXTileLocation = curTileX * tileSizeX;
	double chunkRelativeYTileLocation = curTileY * tileSizeY;
	//get the offset based on the current chunk being created
	double chunkOffset = curChunkIndex * chunkLengthY;
	//add the placement offset to get the corner of tiles to generate in the south
	//add the location of the chunk
	//add the location of the tile within the chunk
	//do this for both x and y
	double curTileXLocation = chunkRelativeXTileLocation + southXPlacementOffset + cornerChunkLocation.X;
	double curTileYLocation = chunkRelativeYTileLocation + southYPlacementOffset + cornerChunkLocation.Y + chunkOffset;
	//create and return the transform/location
	return FTransform(FRotator(0, 0, 0), FVector(curTileXLocation, curTileYLocation, 0), FVector(1, 1, 1));
}

void AtileGenCollisions_C_bp::southProcess(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//loop through chunk location 0 - chunksToGen
	while (curChunkIndex <= chunksToGen) {
		newBiomeID();
		//loop through the y locations of tiles within a chunk
		while (curTileY <= yTilesPerChunk) {
			//loop through the x locations of tiles within a chunk/the x locations of a y location column
			while (curTileX <= xTilesPerChunk) {
				//spawn a tile using the northGetTileTransform
				if (spawnTile(southGetTileTransform())) {
					//if true the spawn limit of chunks for this tick has been reached
					//call set timer to set the timer for the next tick
					setTimerSouth(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
					//end tile generation for this tick
					return;
				}
				//after the tile is spawned increment the curTileX value for the next tile
				curTileX++;
			}
			//the the column of x values for this y value is looped through
			//increment the curTileY value for the next column
			curTileY++;
			//reset the curTileX value for the next column
			curTileX = 0;
		}
		//after the x and y locations of have been looped through for the current chunk
		//increment curChunkIndex for the next chunk
		curChunkIndex++;
		//reset the curTileY value for the next chunk
		curTileY = 0;
	}
}

void AtileGenCollisions_C_bp::endEast()
{
	//reset curChunkIndex for next generation
	curChunkIndex = 0;
	//start the destruction of the previous tiles
	AsyncTask(ENamedThreads::GameThread, [this]() {
		//array for storing the static meshes to be removed
		TArray<int> overlappedTerrain;
		//get references to the world meshes
		//UKismetSystemLibrary::PrintString(GetWorld(), UKismetStringLibrary::Conv_BoolToString(worldManager != nullptr));
		UHierarchicalInstancedStaticMeshComponent* grassTiles = Cast<UHierarchicalInstancedStaticMeshComponent>(worldManager->FindComponentByTag(UHierarchicalInstancedStaticMeshComponent::StaticClass(), FName("grassTile")));
		UHierarchicalInstancedStaticMeshComponent* basicTrees = Cast<UHierarchicalInstancedStaticMeshComponent>(worldManager->FindComponentByTag(UHierarchicalInstancedStaticMeshComponent::StaticClass(), FName("basicTree")));
		//declare a reference to store the tileGenCollision objects to destroy
		TArray<AActor*> tileGenCollisions;
		//define the area to delete
		FBox box = FBox::BuildAABB(GetActorLocation() + FVector(-cornerToCenterChunkXOffset + tileSizeX, cornerToCenterChunkYOffset - tileSizeY, 0) - eastDestructionBoundsOffset, eastDestructionExtent);
		//DrawDebugBox(GetWorld(), box.GetCenter(), box.GetExtent(), FColor(0, 0, 0), true, 99);
		//init an array of object types to overlap
		TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
		objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
		objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
		objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Visibility));
		//declare the array for actors to ignore should that be nessisary
		TArray<AActor*> actorsToIgnore;
		actorsToIgnore.Add(worldManager);
		//get the grass tile indexes to delete
		for (int i = 0; i <= endLoops; i++) {
			for (UActorComponent* curMeshes : worldManager->GetComponentsByTag(UHierarchicalInstancedStaticMeshComponent::StaticClass(), FName("instancedMeshes"))) {
				overlappedTerrain = (Cast<UHierarchicalInstancedStaticMeshComponent>(curMeshes)->GetInstancesOverlappingBox(box, true));
				overlappedTerrain.Sort();
				//delete the grass tiles
				for (int cur : overlappedTerrain) {
					Cast<UHierarchicalInstancedStaticMeshComponent>(curMeshes)->RemoveInstance(cur);
				}
			}
			/*
			//get the basic trees indexes to delete
			overlappedTerrain = basicTrees->GetInstancesOverlappingBox(box, true);
			overlappedTerrain.Sort();
			//delete the basic trees
			for (int cur : overlappedTerrain) {
				basicTrees->RemoveInstance(cur);
			}*/
			//get and destroy the tileGenCollisions to destroy
			UKismetSystemLibrary::BoxOverlapActors(GetWorld(), box.GetCenter(), box.GetExtent(), objectTypes, AActor::StaticClass(), actorsToIgnore, tileGenCollisions);
			if (tileGenCollisions.Num() != 0) {
				for (AActor* cur : tileGenCollisions) {
					if (cur != nullptr)
					{
						cur->Destroy();
					}
				}
			}
		}
	});
}

void AtileGenCollisions_C_bp::eastOverLapped(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//enter async thread for procedural generation
	//AsyncTask(ENamedThreads::AnyThread, [OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult, this]()
	//	{
			//printDebug("East Overlapped");
			//call checkShouldGen
			if (checkShouldGen(OtherActor)) {
				//printDebug("eastActive");
				//if checkShouldGen is true begin procedural generation to the north
				//remove tiles to the south
				exitDir = "east";
				changeY(true);
				endEast();
				//begin generation of tiles to the north
				eastProcess(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
			}
	//});
}
FTransform AtileGenCollisions_C_bp::eastGetTileTransform()
{
	//get the corner of the 'chunk' associated with this set of 'chunk' collisions
	FVector cornerChunkLocation = GetActorLocation() - FVector(cornerToCenterChunkXOffset, cornerToCenterChunkYOffset, 0);
	//get the tile off set based on the current tile being created.
	double chunkRelativeXTileLocation = curTileX * tileSizeX;
	double chunkRelativeYTileLocation = curTileY * tileSizeY;
	//get the offset based on the current chunk being created
	double chunkOffset = curChunkIndex * chunkLengthY;
	//add the placement offset to get the corner of tiles to generate in the south
	//add the location of the chunk
	//add the location of the tile within the chunk
	//do this for both x and y
	double curTileXLocation = chunkRelativeXTileLocation + eastXPlacementOffset + cornerChunkLocation.X + chunkOffset;
	double curTileYLocation = chunkRelativeYTileLocation + eastYPlacementOffset + cornerChunkLocation.Y;
	//create and return the transform/location
	return FTransform(FRotator(0, 0, 0), FVector(curTileXLocation, curTileYLocation, 0), FVector(1, 1, 1));
}

void AtileGenCollisions_C_bp::eastProcess(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//loop through chunk location 0 - chunksToGen
	while (curChunkIndex <= chunksToGen) {
		newBiomeID();
		//loop through the y locations of tiles within a chunk
		while (curTileY <= yTilesPerChunk) {
			//loop through the x locations of tiles within a chunk/the x locations of a y location column
			while (curTileX <= xTilesPerChunk) {
				//spawn a tile using the northGetTileTransform
				if (spawnTile(eastGetTileTransform())) {
					//if true the spawn limit of chunks for this tick has been reached
					//call set timer to set the timer for the next tick
					setTimerEast(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
					//end tile generation for this tick
					return;
				}
				//after the tile is spawned increment the curTileX value for the next tile
				curTileX++;
			}
			//the the column of x values for this y value is looped through
			//increment the curTileY value for the next column
			curTileY++;
			//reset the curTileX value for the next column
			curTileX = 0;
		}
		//after the x and y locations of have been looped through for the current chunk
		//increment curChunkIndex for the next chunk
		curChunkIndex++;
		//reset the curTileY value for the next chunk
		curTileY = 0;
	}
}

void AtileGenCollisions_C_bp::endWest()
{
	//reset curChunkIndex for next generation
	curChunkIndex = 0;
	//start the destruction of the previous tiles
	AsyncTask(ENamedThreads::GameThread, [this]() {
		//array for storing the static meshes to be removed
		TArray<int> overlappedTerrain;
		//get references to the world meshes
		//UKismetSystemLibrary::PrintString(GetWorld(), UKismetStringLibrary::Conv_BoolToString(worldManager != nullptr));
		UHierarchicalInstancedStaticMeshComponent* grassTiles = Cast<UHierarchicalInstancedStaticMeshComponent>(worldManager->FindComponentByTag(UHierarchicalInstancedStaticMeshComponent::StaticClass(), FName("grassTile")));
		UHierarchicalInstancedStaticMeshComponent* basicTrees = Cast<UHierarchicalInstancedStaticMeshComponent>(worldManager->FindComponentByTag(UHierarchicalInstancedStaticMeshComponent::StaticClass(), FName("basicTree")));
		//declare a reference to store the tileGenCollision objects to destroy
		TArray<AActor*> tileGenCollisions;
		//define the area to delete
		FBox box = FBox::BuildAABB(GetActorLocation() + FVector(-cornerToCenterChunkXOffset + tileSizeX, cornerToCenterChunkYOffset - tileSizeY, 0) - westDestructionBoundsOffset, westDestructionExtent);
		//DrawDebugBox(GetWorld(), box.GetCenter(), box.GetExtent(), FColor(0, 0, 0), true, 99);
		//init an array of object types to overlap
		TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
		objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
		objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
		objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Visibility));
		//declare the array for actors to ignore should that be nessisary
		TArray<AActor*> actorsToIgnore;
		actorsToIgnore.Add(worldManager);
		//get the grass tile indexes to delete
		for (int i = 0; i <= endLoops; i++) {
			for (UActorComponent* curMeshes : worldManager->GetComponentsByTag(UHierarchicalInstancedStaticMeshComponent::StaticClass(), FName("instancedMeshes"))) {
				overlappedTerrain = (Cast<UHierarchicalInstancedStaticMeshComponent>(curMeshes)->GetInstancesOverlappingBox(box, true));
				overlappedTerrain.Sort();
				//delete the grass tiles
				for (int cur : overlappedTerrain) {
					Cast<UHierarchicalInstancedStaticMeshComponent>(curMeshes)->RemoveInstance(cur);
				}
			}
			/*
			//get the basic trees indexes to delete
			overlappedTerrain = basicTrees->GetInstancesOverlappingBox(box, true);
			overlappedTerrain.Sort();
			//delete the basic trees
			for (int cur : overlappedTerrain) {
				basicTrees->RemoveInstance(cur);
			}*/
			//get and destroy the tileGenCollisions to destroy
			UKismetSystemLibrary::BoxOverlapActors(GetWorld(), box.GetCenter(), box.GetExtent(), objectTypes, AActor::StaticClass(), actorsToIgnore, tileGenCollisions);
			if (tileGenCollisions.Num() != 0) {
				for (AActor* cur : tileGenCollisions) {
					if (cur != nullptr)
					{
						cur->Destroy();
					}
				}
			}
		}
	});
}

void AtileGenCollisions_C_bp::westOverLapped(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//enter async thread for procedural generation
	//AsyncTask(ENamedThreads::AnyThread, [OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult, this]()
	//	{
			//printDebug("West Overlapped");
			//call checkShouldGen
			if (checkShouldGen(OtherActor)) {
				//printDebug("westActive");
				//if checkShouldGen is true begin procedural generation to the north
				//remove tiles to the south
				exitDir = "west";
				changeY(false);
				endWest();
				//begin generation of tiles to the north
				westProcess(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
			}
	//});
}

FTransform AtileGenCollisions_C_bp::westGetTileTransform()
{
	//get the corner of the 'chunk' associated with this set of 'chunk' collisions
	FVector cornerChunkLocation = GetActorLocation() - FVector(cornerToCenterChunkXOffset, cornerToCenterChunkYOffset, 0);
	//get the tile off set based on the current tile being created.
	double chunkRelativeXTileLocation = curTileX * tileSizeX;
	double chunkRelativeYTileLocation = curTileY * tileSizeY;
	//get the offset based on the current chunk being created
	double chunkOffset = curChunkIndex * chunkLengthY;
	//add the placement offset to get the corner of tiles to generate in the south
	//add the location of the chunk
	//add the location of the tile within the chunk
	//do this for both x and y
	double curTileXLocation = chunkRelativeXTileLocation + westXPlacementOffset + cornerChunkLocation.X + chunkOffset;
	double curTileYLocation = chunkRelativeYTileLocation + westYPlacementOffset + cornerChunkLocation.Y;
	//create and return the transform/location
	return FTransform(FRotator(0, 0, 0), FVector(curTileXLocation, curTileYLocation, 0), FVector(1, 1, 1));
}

void AtileGenCollisions_C_bp::westProcess(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//loop through chunk location 0 - chunksToGen
	while (curChunkIndex <= chunksToGen) {
		newBiomeID();
		//loop through the y locations of tiles within a chunk
		while (curTileY <= yTilesPerChunk) {
			//loop through the x locations of tiles within a chunk/the x locations of a y location column
			while (curTileX <= xTilesPerChunk) {
				//spawn a tile using the northGetTileTransform
				if (spawnTile(westGetTileTransform())) {
					//if true the spawn limit of chunks for this tick has been reached
					//call set timer to set the timer for the next tick
					setTimerWest(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
					//end tile generation for this tick
					return;
				}
				//after the tile is spawned increment the curTileX value for the next tile
				curTileX++;
			}
			//the the column of x values for this y value is looped through
			//increment the curTileY value for the next column
			curTileY++;
			//reset the curTileX value for the next column
			curTileX = 0;
		}
		//after the x and y locations of have been looped through for the current chunk
		//increment curChunkIndex for the next chunk
		curChunkIndex++;
		//reset the curTileY value for the next chunk
		curTileY = 0;
	}
}

//do logic for tile spawn
bool AtileGenCollisions_C_bp::spawnTile(FTransform tileTransform)
{
		//switch from async thread to game thread for spawning
		AsyncTask(ENamedThreads::GameThread, [this, tileTransform]() {
			UHierarchicalInstancedStaticMeshComponent* grassTiles = Cast<UHierarchicalInstancedStaticMeshComponent>(worldManager->FindComponentByTag(UHierarchicalInstancedStaticMeshComponent::StaticClass(), FName("grassTile")));
			//add an instance to the tile manager with the current tiles generated transform
			grassTiles->AddInstance(tileTransform, true);
			});
		//attempt to spawn the tileGenCollision for this chunk on this tile
		spawnTileGenCollision(tileTransform);
		//attempt to spawn a tree at this tile
		tryResourceGen(tileTransform);
		//incremeant tile index to count tiles generated this chunk
		curTileIndex++;
		//check that we have not reached the tilesToGenPerTick limit
		if (curTileIndex == tilesToGenPerTick) {
			//if tilesToGenPerTick limit is reached reset the curTileIndex for the next tick and return true
			curTileIndex = 0;
			return true;
		}
		//otherwise return false
		return false;
}

//check if this tile is the center of its chunk
bool AtileGenCollisions_C_bp::checkIfTileCenter()
{
	//return if the curTileX and curTileY are both half of the total tiles
	return ((curTileX == xTilesPerChunk/2) && (curTileY == yTilesPerChunk/2));
}

//attempt to spawn a tileGenCollision actor
void AtileGenCollisions_C_bp::spawnTileGenCollision(FTransform transform)
{
	//check if this tile is the center of its 'chunk'
	if (checkIfTileCenter()) {
		//if it is the center spawn the tileGenCollisionsObject on that tile
		//declare a variable for the spawn params
		FActorSpawnParameters spawnParams;
		/*
			If params are need put them here
		*/
		//switch to game thread for spawning
		AsyncTask(ENamedThreads::GameThread, [this, transform, spawnParams]() {
			//spawn the actor
			GetWorld()->SpawnActor<AtileGenCollisions_C_bp>(this->GetClass(), transform, spawnParams);
		});
	}
}

FTransform AtileGenCollisions_C_bp::randomizeRotation(FTransform transformToChange)
{
	return FTransform(FRotator(0, UKismetMathLibrary::RandomFloatInRange(0, 360), 0), transformToChange.GetLocation(), transformToChange.GetScale3D());
}

void AtileGenCollisions_C_bp::tryResourceGen(FTransform transform)
{
		FTransform transformToUse = randomizeRotation(transform);
		if (!tryTreeGen(transformToUse)) {
			if (!tryRockGen(transformToUse)) {
				tryMonsterGen(transform);
			}
		}
}

//attempt to spawn a basic tree
bool AtileGenCollisions_C_bp::tryTreeGen(FTransform transform)
{
	float chanceToGen;
	switch (biomeID)
	{
		case 0:
			chanceToGen = 0.075;
			break;
		case 1:
			chanceToGen = 0.01;
			break;
		case 2:
			chanceToGen = 0;
			break;
		case 3:
			chanceToGen = 0;
			break;
		case 4:
			chanceToGen = 0;
			break;
		case 5:
			chanceToGen = 0;
			break;
		case 6:
			chanceToGen = 0.025;
			break;
		default:
			chanceToGen = treeGenChanceForTile;
			break;
	}
	//roll the chance of generating a tree
	if (UKismetMathLibrary::RandomBoolWithWeight(chanceToGen)) {
		//switch to game thread for spawning
		AsyncTask(ENamedThreads::GameThread, [this,transform]() {
			UHierarchicalInstancedStaticMeshComponent* basicTrees = Cast<UHierarchicalInstancedStaticMeshComponent>(worldManager->FindComponentByTag(UHierarchicalInstancedStaticMeshComponent::StaticClass(), FName("basicTree")));
			//add a tree instance with the current tiles transform
			basicTrees->AddInstance(transform, true);
			return true;
		});
	}
	return false;
}

//attempt to spawn a basic rock
bool AtileGenCollisions_C_bp::tryRockGen(FTransform transform)
{
	float chanceToGen;
	switch (biomeID) {
		case 0:
			chanceToGen = 0.01;
			break;
		case 1:
			chanceToGen = 0.075;
			break;
		case 2:
			chanceToGen = 0;
			break;
		case 3:
			chanceToGen = 0;
			break;
		case 4:
			chanceToGen = 0;
			break;
		case 5:
			chanceToGen = 0;
			break;
		case 6:
			chanceToGen = 0.025;
			break;
		default:
			chanceToGen = rockGenChanceForTile;
			break;
	}
	//roll the chance of generating a tree
	if (UKismetMathLibrary::RandomBoolWithWeight(chanceToGen)) {
		//switch to game thread for spawning
		AsyncTask(ENamedThreads::GameThread, [this, transform]() {
			UHierarchicalInstancedStaticMeshComponent* basicRocks = Cast<UHierarchicalInstancedStaticMeshComponent>(worldManager->FindComponentByTag(UHierarchicalInstancedStaticMeshComponent::StaticClass(), FName("basicRock")));
			//add a tree instance with the current tiles transform
			basicRocks->AddInstance(transform, true);
			return true;
		});
	}
	return false;
}

//attempt to spawn a monster
bool AtileGenCollisions_C_bp::tryMonsterGen(FTransform transform)
{
	float chanceToGen;
	switch (biomeID) {
	case 0:
		chanceToGen = monsterGenChanceForTile;
		break;
	case 1:
		chanceToGen = monsterGenChanceForTile;
		break;
	case 2:
		chanceToGen = 0;
		break;
	case 3:
		chanceToGen = 0;
		break;
	case 4:
		chanceToGen = 0.015;
		break;
	case 5:
		chanceToGen = 0;
		break;
	case 6:
		chanceToGen = monsterGenChanceForTile;
		break;
	default:
		chanceToGen = monsterGenChanceForTile;
		break;
	}
	//roll the chance of generating a tree
	if (UKismetMathLibrary::RandomBoolWithWeight(chanceToGen)) {
		//switch to game thread for spawning
		AsyncTask(ENamedThreads::GameThread, [this, transform]() {
			genMonster(transform);
			return true;
			});
	}
	return false;
}

void AtileGenCollisions_C_bp::playerDetectorBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OtherActor->IsA(APawn::StaticClass()) && (OtherActor->GetClass() == playerClass))) {
		shouldGen = true;
	}
}

void AtileGenCollisions_C_bp::playerDetectorEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if ((OtherActor->IsA(APawn::StaticClass()) && (OtherActor->GetClass() == playerClass))) {
		shouldGen = false;
	}
}

//set a timer for the next tick and restart the north process
void AtileGenCollisions_C_bp::setTimerNorth(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//switch to game thread to set the time
	AsyncTask(ENamedThreads::GameThread, [OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult, this]() {
		//create the timer by function
		GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this, OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult]()
			{
				//debug for how many times the timer was activating
				//UKismetSystemLibrary::PrintString(GetWorld(), "Timer Activated");
				//when the timer ends reenter the northProcess
				northProcess(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
			}));
	});
}

void AtileGenCollisions_C_bp::setTimerSouth(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//switch to game thread to set the time
	AsyncTask(ENamedThreads::GameThread, [OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult, this]() {
		//create the timer by function
		GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this, OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult]()
			{
				//debug for how many times the timer was activating
				//UKismetSystemLibrary::PrintString(GetWorld(), "Timer Activated");
				//when the timer ends reenter the southProcess
				southProcess(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
			}));
		});
}

void AtileGenCollisions_C_bp::setTimerEast(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//switch to game thread to set the time
	AsyncTask(ENamedThreads::GameThread, [OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult, this]() {
		//create the timer by function
		GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this, OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult]()
			{
				//debug for how many times the timer was activating
				//UKismetSystemLibrary::PrintString(GetWorld(), "Timer Activated");
				//when the timer ends reenter the southProcess
				eastProcess(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
			}));
		});
}

void AtileGenCollisions_C_bp::setTimerWest(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//switch to game thread to set the time
	AsyncTask(ENamedThreads::GameThread, [OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult, this]() {
		//create the timer by function
		GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this, OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult]()
			{
				//debug for how many times the timer was activating
				//UKismetSystemLibrary::PrintString(GetWorld(), "Timer Activated");
				//when the timer ends reenter the southProcess
				westProcess(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
			}));
		});
}

//event tied to the OnComponentBeginOverlap event for the north tile
void AtileGenCollisions_C_bp::northOverlapped(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	////printDebug("northOverlapped");
	//enter async thread for procedural generation
	//AsyncTask(ENamedThreads::AnyThread, [OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult, this]()
		//{
			//call checkShouldGen
			if (checkShouldGen(OtherActor)) {
				////printDebug("northActive");
				//if checkShouldGen is true begin procedural generation to the north
				//remove tiles to the south
				exitDir = "north";
				changeX(true);
				endNorth();
				//begin generation of tiles to the north
				northProcess(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
			}
		//});
}