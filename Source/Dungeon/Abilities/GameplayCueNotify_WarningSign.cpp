#include "Abilities/GameplayCueNotify_WarningSign.h"
#include "Global.h"

#include "Objects/WarningSign.h"
#include "Abilities/GameplayEffectContexts.h"

UGameplayCueNotify_WarningSign::UGameplayCueNotify_WarningSign()
{

}

bool UGameplayCueNotify_WarningSign::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	const FWarningSignEffectContext* effectContext = static_cast<const FWarningSignEffectContext*>(Parameters.EffectContext.Get());
	if (!effectContext)return false;

    AWarningSign* sign = GetWorld()->SpawnActorDeferred<AWarningSign>(effectContext->Class, effectContext->Transform, effectContext->Owner,
        Cast<APawn>(effectContext->GetInstigator()), effectContext->CollisionHandlingOverride);
    sign->Activate(effectContext->StartServerTime, effectContext->Duration, effectContext->ExtraDuration);
    sign->FinishSpawning(effectContext->Transform);

	return false;
}