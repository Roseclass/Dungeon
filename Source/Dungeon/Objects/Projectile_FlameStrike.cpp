#include "Objects/Projectile_FlameStrike.h"
#include "Global.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/SphereComponent.h"

#include "Components/SceneComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"

#include "Characters/DungeonCharacterBase.h"
#include "Components/SkillComponent.h"

AProjectile_FlameStrike::AProjectile_FlameStrike()
{
	Type = EProjectileType::AOE_Normal;

	CHelpers::CreateComponent(this, &RootCollision, "RootCollision");
	CHelpers::CreateComponent(this, &Niagara, "Niagara", RootCollision);
}

void AProjectile_FlameStrike::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
		SetLifeSpan(DotDuration);

	//ReserveSquence
	if (HasAuthority())
	{
		FTimerHandle WaitHandle;
		float WaitTime = BurstDuration;
		GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			Deactivate();
		}), WaitTime, false);
	}
}

void AProjectile_FlameStrike::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckFalse(HasAuthority());

	CurrentDelay += DeltaTime;
	if (CurrentDelay > DotDamageDelay)
	{
		SendDotDamage();
		CurrentDelay = 0;
	}
}

void AProjectile_FlameStrike::SendDotDamage()
{
	for (auto i : DotActors)
	{
		//SendDamage
		SendDamage(DotGamePlayEffectClass, 0, DotDamage, i, FHitResult());
	}
}

void AProjectile_FlameStrike::OnDotBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// already hit?
	if (DotActors.Contains(OtherActor))return;

	// overlap with DungeonCharacterBase
	ADungeonCharacterBase* base = Cast<ADungeonCharacterBase>(OtherActor);
	if (!base)return;

	// is deadmode?
	USkillComponent* skill = CHelpers::GetComponent<USkillComponent>(OtherActor);
	if (!skill)return;
	if (skill->IsDead())return;

	// ignore alliance
	CheckTrue(base->GetGenericTeamId() == TeamID);

	// set properties
	DotActors.AddUnique(OtherActor);
}

void AProjectile_FlameStrike::OnDotEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	CheckFalse(HasAuthority());

	// set properties
	DotActors.Remove(OtherActor);
}

void AProjectile_FlameStrike::FindCollision()
{
	Super::FindCollision();
	
	RootCollision->OnComponentBeginOverlap.AddDynamic(this, &AProjectile_FlameStrike::OnDotBeginOverlap);
	RootCollision->OnComponentEndOverlap.AddDynamic(this, &AProjectile_FlameStrike::OnDotEndOverlap);
}
