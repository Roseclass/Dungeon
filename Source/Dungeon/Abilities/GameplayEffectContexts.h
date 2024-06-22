#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "UObject/Class.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffectContexts.generated.h"

/**
 * 
 */

//#include "Abilities/GameplayEffectContexts.h"

USTRUCT(BlueprintType)
struct FDamageTextEffectContext : public FGameplayEffectContext
{
    GENERATED_BODY()

    UPROPERTY()float CalculatedDamage;
    UPROPERTY()bool bIsCritical;
};