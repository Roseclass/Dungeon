#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PlayerDialogDatas.generated.h"

/**
 * 
 */

class ADungeonPlayerController;

UCLASS()
class DUNGEON_API UPlayerDialogDatas : public UObject
{
	GENERATED_BODY()
public:
	TMap<ADungeonPlayerController*, int32> PointMap;
};
