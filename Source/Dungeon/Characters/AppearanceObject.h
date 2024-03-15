#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Characters/CharcterAppearanceData.h"
#include "AppearanceObject.generated.h"

/**
 * Item(except weapon)'s Data class
 */

UCLASS(Blueprintable)
class DUNGEON_API UAppearanceObject : public UObject
{
	GENERATED_BODY()

protected:
#if WITH_EDITOR
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	//property
private:
protected:
	UPROPERTY(EditAnywhere)
		FAppearanceData AppearanceData;
public:
	//function
private:
protected:
public:
	FORCEINLINE EAppearancePart GetPartType() const { return AppearanceData.GetPartType(); }
	FORCEINLINE int32 GetDataIndex()const { return AppearanceData.GetDataIndex(); }
};
