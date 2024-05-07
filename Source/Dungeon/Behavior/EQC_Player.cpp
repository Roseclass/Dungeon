#include "Behavior/EQC_Player.h"
#include "Global.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

#include "Characters/PlayerCharacter.h"
#include "Characters/Enemy.h"
#include "Components/BehaviorComponent.h"

void UEQC_Player::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	AEnemy* aiObject = Cast<AEnemy>(QueryInstance.Owner.Get());
	UBehaviorComponent* behavior = CHelpers::GetComponent<UBehaviorComponent>(aiObject->GetController());

	TArray<AActor*> arr;
	if (behavior && behavior->GetPerceptedPlayers())
	{
		for (auto i : behavior->GetPerceptedPlayers()->GetPlayers())
			arr.Add(i);
		UEnvQueryItemType_Actor::SetContextHelper(ContextData, arr);
	}
}