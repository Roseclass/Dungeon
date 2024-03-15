#include "Behavior/EQC_Player.h"
#include "Global.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

#include "Characters/DungeonCharacter.h"
#include "Characters/Enemy.h"
#include "Components/BehaviorComponent.h"

void UEQC_Player::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	AEnemy* aiObject = Cast<AEnemy>(QueryInstance.Owner.Get());
	UBehaviorComponent* behavior = CHelpers::GetComponent<UBehaviorComponent>(aiObject->GetController());

	if (behavior->GetTarget())
		UEnvQueryItemType_Actor::SetContextHelper(ContextData, behavior->GetTarget());
}