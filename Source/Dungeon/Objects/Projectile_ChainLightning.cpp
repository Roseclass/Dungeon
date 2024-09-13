#include "Objects/Projectile_ChainLightning.h"
#include "Global.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"

#include "Components/SceneComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"

#include "Characters/DungeonCharacterBase.h"
#include "Components/SkillComponent.h"

AProjectile_ChainLightning::AProjectile_ChainLightning()
{
	Type = EProjectileType::AOE_Neutralizable;

	CHelpers::CreateComponent(this, &OverlapCollision, "OverlapCollision");
	CHelpers::CreateComponent(this, &Niagara, "Niagara", OverlapCollision);

	OverlapCollision->ComponentTags.Add(OverlapComponentTag);
}

void AProjectile_ChainLightning::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectile_ChainLightning::SendDamage(TSubclassOf<UGameplayEffect> EffectClass, float InForce, float InDamage, AActor* Target, const FHitResult& SweepResult)
{
	CheckTrue(Target == GetCurrentDamagedActor());
	Super::SendDamage(EffectClass, InForce, InDamage, Target, SweepResult);

	//decrease count
	--Count;

	if (!Count)
	{
		Deactivate();
		ProjectileMovement->StopMovementImmediately();
	}
	else
	{
		// find next target
		TArray<TEnumAsByte<EObjectTypeQuery>> filter;
		filter.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
		TArray<AActor*> ignore; ignore.Add(Target);
		TArray<AActor*> arr;

		UKismetSystemLibrary::SphereOverlapActors(
			GetWorld(),
			GetActorLocation(),
			500,
			filter,
			ADungeonCharacterBase::StaticClass(),
			ignore,
			arr
		);

		for (int32 i = 0; i < arr.Num(); ++i)
		{
			ADungeonCharacterBase* base = Cast<ADungeonCharacterBase>(arr[i]);
			if (base->GetGenericTeamId() == TeamID)
				arr.Remove(arr[i--]);
		}

		//theres no any other targets
		if (arr.IsEmpty())
		{
			Deactivate();
			ProjectileMovement->StopMovementImmediately();
			return;
		}

		//change direction
		int idx = UKismetMathLibrary::RandomIntegerInRange(0, arr.Num() - 1);
		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), arr[idx]->GetActorLocation()));
	}
}
