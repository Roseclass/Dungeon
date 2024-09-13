#include "Abilities/GameplayCueNotify_NiagaraFX.h"
#include "Global.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"

#include "Abilities/GameplayEffectContexts.h"

UGameplayCueNotify_NiagaraFX::UGameplayCueNotify_NiagaraFX()
{

}

bool UGameplayCueNotify_NiagaraFX::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	const FNiagaraFXEffectContext* effectContext = static_cast<const FNiagaraFXEffectContext*>(Parameters.EffectContext.Get());
	if (!effectContext)return false;

	if (effectContext->AttachTarget)
		UNiagaraFunctionLibrary::SpawnSystemAttached(effectContext->FX, effectContext->AttachTarget->GetRootComponent(), NAME_None, FVector(), FRotator(), EAttachLocation::SnapToTarget, 1);
	else
	{
		UNiagaraComponent* fx = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), effectContext->FX, effectContext->Transform.GetLocation());
		fx->SetWorldTransform(effectContext->Transform);
	}

	return false;
}