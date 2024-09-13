#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TeleportAfterimage.generated.h"

class ADungeonCharacterBase;
class UPoseableMeshComponent;

class USceneComponent;

UCLASS()
class DUNGEON_API ATeleportAfterimage : public AActor
{
	GENERATED_BODY()
	
public:	
	ATeleportAfterimage();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;

	//property
private:
	TArray<UPoseableMeshComponent*> PoseableMeshes;
protected:
	//scene
	UPROPERTY(VisibleAnywhere)
		USceneComponent* RootScene;

public:

	//function
private:
protected:
public:
	void Init(ADungeonCharacterBase* InBase);
};
