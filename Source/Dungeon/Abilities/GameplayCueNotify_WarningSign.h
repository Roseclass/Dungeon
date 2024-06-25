#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "GameplayCueNotify_WarningSign.generated.h"

/**
 * 
 */

UCLASS(Blueprintable, Category = "GameplayCueNotify")
class DUNGEON_API UGameplayCueNotify_WarningSign : public UGameplayCueNotify_Static
{
	GENERATED_BODY()
public:
	UGameplayCueNotify_WarningSign();
protected:
public:
	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const override;

	//property
private:
protected:
public:

	//function
private:
protected:
public:
};
