#include "Objects/Projectile_GlacialWave.h"
#include "Global.h"
#include "GameFramework/ProjectileMovementComponent.h"

#include "Components/SceneComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"

AProjectile_GlacialWave::AProjectile_GlacialWave()
{
	Type = EProjectileType::AOE_Normal;

	CHelpers::CreateComponent(this, &Root, "Root");
	CHelpers::CreateComponent(this, &Niagara, "Niagara", Root);
}

void AProjectile_GlacialWave::BeginPlay()
{
	Super::BeginPlay();

	Niagara->SetFloatParameter("LoopDuration", LifeTime);

	if(HasAuthority())
		SetLifeSpan(LifeTime + DestoryDelay);

	//ReserveSquence
	{
		FTimerHandle WaitHandle;
		float WaitTime = LifeTime;
		GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			if (HasAuthority())
			{
				Deactivate();
				ProjectileMovement->StopMovementImmediately();
			}

			//adjust spawnrate
			Niagara->SetIntParameter("SpawnCount", 0);

		}), WaitTime, false); 
	}
}