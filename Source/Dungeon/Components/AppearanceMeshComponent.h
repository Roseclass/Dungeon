#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "AppearanceMeshComponent.generated.h"

/**
 * for attach to character's mesh, child mesh component
 * just checking part to be attached
 * use only needed part, otherwise may make mistake
 */

enum class EAppearancePart : uint8;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEON_API UAppearanceMeshComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly)
		EAppearancePart AppearancePart;
public:
	FORCEINLINE EAppearancePart GetAppearancePart() {return AppearancePart;}
};
