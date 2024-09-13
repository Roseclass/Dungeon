#include "Objects/Projectile_FrostNova.h"
#include "Global.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/SphereComponent.h"

#include "Characters/DungeonCharacterBase.h"
#include "Components/SkillComponent.h"

#include "NiagaraComponent.h"

AProjectile_FrostNova::AProjectile_FrostNova()
{
	CHelpers::CreateComponent(this, &RootCollision, "RootCollision");
	CHelpers::CreateComponent(this, &Niagara, "Niagara", RootCollision);
}

void AProjectile_FrostNova::BeginPlay()
{
	Super::BeginPlay();

	CheckFalse(HasAuthority());

	//SetLifeSpan
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
		}), WaitTime, false);
	}

	// bind curve
	if (SizeCurve)
	{
		SizeTimelineFloat.BindUFunction(this, "SizeSequenceTickFunction");
		SizeTimeLine.AddInterpFloat(SizeCurve, SizeTimelineFloat);
		SizeTimeLine.PlayFromStart();
	}
}

void AProjectile_FrostNova::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SizeTimeLine.TickTimeline(DeltaTime);
}

void AProjectile_FrostNova::SizeSequenceTickFunction(float Value)
{
	float elapsedTime = GetWorld()->GetTimeSeconds() - CreationTime;
	float rate = elapsedTime / LifeTime;
	RootCollision->SetSphereRadius(rate * MaxSize);
}
