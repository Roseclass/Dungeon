#include "Abilities/GameplayCueNotify_DamageText.h"
#include "Global.h"

#include "DungeonPlayerController.h"
#include "Characters/PlayerCharacter.h"
#include "Characters/Enemy.h"
#include "Objects/DamageText.h"
#include "Objects/DamageDealer.h"
#include "Abilities/GameplayEffectContexts.h"

UGameplayCueNotify_DamageText::UGameplayCueNotify_DamageText()
{
 
}

bool UGameplayCueNotify_DamageText::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	const FDamageEffectContext* effectContext = static_cast<const FDamageEffectContext*>(Parameters.EffectContext.Get());
	if (!effectContext)return false;	

	FTransform transform;
	transform.SetTranslation(Parameters.Location);
	ADamageText* txt = GetWorld()->SpawnActorDeferred<ADamageText>(DamageTextClass, transform);
	txt->Init(effectContext->CalculatedDamage, effectContext->bIsCritical);
	UGameplayStatics::FinishSpawningActor(txt, transform);

	return false;
}