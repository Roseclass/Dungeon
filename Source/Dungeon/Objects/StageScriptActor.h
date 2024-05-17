#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "StageScriptActor.generated.h"

/**
 * 
 */

UCLASS()
class DUNGEON_API AStageScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()
	//property
private:
protected:
public:
	//function
private:
protected:
public:
	UFUNCTION(BlueprintImplementableEvent)void On();
	UFUNCTION(BlueprintImplementableEvent)void Off();
};
