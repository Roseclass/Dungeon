#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_UniqueItemEffect.generated.h"

/**
 * 
 */

UCLASS()
class DUNGEON_API UGE_UniqueItemEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
    UGE_UniqueItemEffect();
protected:
public:

    //property
private:
protected:
public:

    //function
private:
protected:
public:
    UFUNCTION(BlueprintImplementableEvent)FString GetDescription()const;
};
