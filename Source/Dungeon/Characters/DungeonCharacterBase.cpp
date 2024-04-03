#include "Characters/DungeonCharacterBase.h"
#include "Global.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Components/DecalComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "SaveManager.h"
#include "DungeonPlayerController.h"
#include "Components/SkillComponent.h"
#include "Components/MontageComponent.h"
#include "Components/StateComponent.h"
#include "Components/StatusComponent.h"
#include "Components/InventoryComponent.h"

#include "Objects/CustomDamageType.h"

ADungeonCharacterBase::ADungeonCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	//actor
	CHelpers::CreateActorComponent<USkillComponent>(this, &Skill, "Skill");
	CHelpers::CreateActorComponent<UMontageComponent>(this, &Montage, "Montage");
	CHelpers::CreateActorComponent<UStateComponent>(this, &State, "State");
	CHelpers::CreateActorComponent<UStatusComponent>(this, &Status, "Status");
	CHelpers::CreateActorComponent<UInventoryComponent>(this, &Inventory, "Inventory");
}

void ADungeonCharacterBase::ADungeonCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	Init();
}

void ADungeonCharacterBase::ADungeonCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float ADungeonCharacterBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float result = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	CLog::Print(DamageAmount, -1, 10, FColor::Red);

	if (!DamageEvent.DamageTypeClass->IsChildOf(UCustomDamageType::StaticClass()))
	{
		FString name = "";
		if (DamageCauser)name = DamageCauser->GetName();
		CLog::Print(name + "'s DamageTypeClass is no custom", -1, 10, FColor::Red);
		return result;
	}

	UCustomDamageType* damageType = Cast<UCustomDamageType>(NewObject<UDamageType>(this, DamageEvent.DamageTypeClass));
	if (!damageType)
	{
		FString name = "";
		if (DamageCauser)name = DamageCauser->GetName();
		CLog::Print(name + "'s damageType is nullptr", -1, 10, FColor::Red);
		return result;
	}


	// None, Normal, KnockBack, KnockDown, Max

	switch (damageType->ReactionType)
	{
	case EReactionType::None:HitReaction_None(); break;
	case EReactionType::Normal:HitReaction_Normal(DamageCauser); break;
	case EReactionType::KnockBack:HitReaction_KnockBack(damageType->DamageImpulse,DamageCauser); break;
	case EReactionType::KnockDown:HitReaction_KnockDown(damageType->DamageImpulse,DamageCauser); break;
	default:break;
	}

	return result;
}

FGenericTeamId ADungeonCharacterBase::GetGenericTeamId() const
{
	return TeamID;
}

void ADungeonCharacterBase::HitReaction_None()
{

}

void ADungeonCharacterBase::HitReaction_Normal(AActor* InCauser)
{
	Montage->PlayHitMontage(InCauser);
}

void ADungeonCharacterBase::HitReaction_KnockBack(float InForce, AActor* InCauser)
{

}

void ADungeonCharacterBase::HitReaction_KnockDown(float InForce, AActor* InCauser)
{
	FVector force = GetActorLocation() - InCauser->GetActorLocation();
	force.Z = 0;
	force.Normalize();

	SetActorRotation(UKismetMathLibrary::MakeRotFromX(-force));

	GetCharacterMovement()->StopMovementImmediately();

	Montage->PlayKnockDownMontage(force * InForce);
}

void ADungeonCharacterBase::Init()
{
	State->OnStateTypeChanged.AddUFunction(this, "ChangeState");
}

void ADungeonCharacterBase::ChangeState(EStateType PrevType, EStateType NewType)
{
	if (NewType == EStateType::Dead)
	{
		// play dead montage
	}
}

bool ADungeonCharacterBase::CanUse()
{
	return Status->CanUse();
}
bool ADungeonCharacterBase::CanMove()
{
	return Status->CanMove();
}

void ADungeonCharacterBase::SetUse()
{
	Status->SetUse();
}

void ADungeonCharacterBase::SetCannotUse()
{
	Status->SetCannotUse();
}

void ADungeonCharacterBase::SetMove()
{
	Status->SetMove();
}

void ADungeonCharacterBase::SetStop()
{
	Status->SetStop();
}

void ADungeonCharacterBase::UnsetSkill()
{

}

void ADungeonCharacterBase::SpawnProjectile()
{
	Skill->SpawnProjectile();
}

void ADungeonCharacterBase::OnCollision()
{
	Inventory->OnCollision();
}

void ADungeonCharacterBase::OffCollision()
{
	Inventory->OffCollision();
}

void ADungeonCharacterBase::ResetHittedActors()
{
	Inventory->ResetHittedActors();
}

void ADungeonCharacterBase::SetIdleMode()
{
	State->SetIdleMode();
}

void ADungeonCharacterBase::SetSkillMode()
{
	State->SetSkillMode();
}

void ADungeonCharacterBase::SetHittedMode()
{
	State->SetHittedMode();
}

void ADungeonCharacterBase::SetDeadMode()
{
	State->SetDeadMode();
}
