#include "Objects/Projectile_ThunderStorm.h"
#include "Global.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/SphereComponent.h"

#include "Components/SceneComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"

#include "Characters/DungeonCharacterBase.h"
#include "Components/SkillComponent.h"

AProjectile_ThunderStorm::AProjectile_ThunderStorm()
{
	CHelpers::CreateComponent(this, &RootCollision, "RootCollision");
	CHelpers::CreateComponent(this, &Niagara, "Niagara", RootCollision);
}

void AProjectile_ThunderStorm::BeginPlay()
{
	Super::BeginPlay();

	Niagara->SetFloatParameter("LoopDuration", LifeTime);

	if (HasAuthority())
		SetLifeSpan(LifeTime + DestoryDelay);

	//ReserveSquence
	if (HasAuthority())
	{
		FTimerHandle WaitHandle;
		float WaitTime = LifeTime;
		GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			Deactivate();
		}), WaitTime, false);
	}
}

void AProjectile_ThunderStorm::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckFalse(HasAuthority());
	CheckFalse(IsActivated());

	CurrentDelay += DeltaTime;
	if (CurrentDelay > DotDamageDelay)
	{
		SpawnThunderAndSendDamage();
		CurrentDelay = 0;
	}
}

void AProjectile_ThunderStorm::SpawnThunderAndSendDamage()
{
	// find next target
	TArray<TEnumAsByte<EObjectTypeQuery>> filter;
	filter.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	TArray<AActor*> ignore;
	TArray<AActor*> arr;

	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		GetActorLocation(),
		RootCollision->GetScaledSphereRadius(),
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
	CheckTrue(arr.IsEmpty())return;

	ADungeonCharacterBase* owner = Cast<ADungeonCharacterBase>(GetOwner());
	if (!owner)
	{
		CLog::Print("AProjectile_ThunderStorm::SpawnThunderAndSendDamage there is no owner");
		return;
	}
	
	USkillComponent* skill =  Cast<USkillComponent>(owner->GetAbilitySystemComponent());
	if (!skill)
	{
		CLog::Print("AProjectile_ThunderStorm::SpawnThunderAndSendDamage skill is nullptr");
		return;
	}

	//SendDamage
	for (auto i : arr)
	{
		SendDamage(GamePlayEffectClass, Force, Damage, i, FHitResult());
		skill->Multicast_FXEffect_Transform(ThunderFX, i->GetActorTransform());
	}
}