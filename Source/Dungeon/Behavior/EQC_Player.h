#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EQC_Player.generated.h"

/**
 * 
 */

UCLASS()
class DUNGEON_API UEQC_Player : public UEnvQueryContext
{
	GENERATED_BODY()
protected:
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};
