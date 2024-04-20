#include "Characters/DungeonCharacterBase.h"
#include "Global.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Components/DecalComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "SaveManager.h"
#include "DungeonPlayerController.h"
#include "Components/SkillComponent.h"
#include "Components/MontageComponent.h"
#include "Components/StateComponent.h"
#include "Components/StatusComponent.h"
#include "Components/InventoryComponent.h"

#include "Objects/CustomDamageType.h"
#include "Widgets/UW_HealthBar.h"

ADungeonCharacterBase::ADungeonCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	//scene
	CHelpers::CreateComponent(this, &HealthBarRoot, "HealthBarRoot", RootComponent);
	CHelpers::CreateComponent(this, &HealthBar, "HealthBar", HealthBarRoot);

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

	// is already dead?
	if (State->IsDeadMode())return result;

	// refresh status
	Status->AdjustCurrentHealth(-DamageAmount);

	// set montage datas
	{
		Montage->SetDamageCauser(DamageCauser);

		FVector force = GetActorLocation() - DamageCauser->GetActorLocation();
		force.Z = 0;
		force.Normalize(); 

		Montage->SetForce(damageType->DamageImpulse * force);
	}

	// is dead?
	float hp = Status->GetCurrentHealth_Server();
	if (hp <= 0)
	{
		SetDeadMode();
		return result;
	}

	// play reaction
	// None, Normal, KnockBack, KnockDown, Max
	switch (damageType->ReactionType)
	{
	case EReactionType::None:HitReaction_None(); break;
	case EReactionType::Normal:SetHitMode(); break;
	case EReactionType::KnockBack:SetKnockBackMode(); break;
	case EReactionType::KnockDown:SetKnockDownMode(); break;
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

void ADungeonCharacterBase::HitReaction_Normal()
{
	Montage->PlayHitMontage();
}

void ADungeonCharacterBase::HitReaction_KnockBack()
{

}

void ADungeonCharacterBase::HitReaction_KnockDown()
{
	GetCharacterMovement()->StopMovementImmediately();
	Montage->PlayKnockDownMontage();
}

void ADungeonCharacterBase::ChangeHealthBarMax(float NewMax)
{
	if (!HealthBarWidget)
	{
		CLog::Print("ChangeHealthBarPercent, HealthBarWidget is nullptr", -1, 10, FColor::Red);
		return;
	}
	HealthBarWidget->SetMax(NewMax);
}

void ADungeonCharacterBase::ChangeHealthBarPercent(float NewPercent)
{
	if (!HealthBarWidget)
	{
		CLog::Print("ChangeHealthBarPercent, HealthBarWidget is nullptr", -1, 10, FColor::Red);
		return;
	}
	HealthBarWidget->SetPercent(NewPercent);
}

void ADungeonCharacterBase::ChangeHealthBarRegen(float NewRegen)
{
	if (!HealthBarWidget)
	{
		CLog::Print("ChangeHealthBarPercent, HealthBarWidget is nullptr", -1, 10, FColor::Red);
		return;
	}
	HealthBarWidget->SetRegen(NewRegen);
}

void ADungeonCharacterBase::Init()
{
	State->OnStateTypeChanged.AddUFunction(this, "ChangeState");

	Status->OnMaxHealthChanged.AddUFunction(this, "ChangeHealthBarMax");
	Status->OnCurrentHealthChanged.AddUFunction(this, "ChangeHealthBarPercent");
	Status->OnHealthRegenChanged.AddUFunction(this, "ChangeHealthBarRegen");
	Status->Update();
}

void ADungeonCharacterBase::ChangeState(EStateType PrevType, EStateType NewType)
{
	CheckTrue(PrevType == NewType);

	if (NewType == EStateType::Dead)
	{
		Montage->PlayDeadMontage();
	}
	else if (NewType == EStateType::Hit)
	{
		HitReaction_Normal();
	}
	else if (NewType == EStateType::KnockBack)
	{
		HitReaction_KnockBack();
	}
	else if (NewType == EStateType::KnockDown)
	{
		HitReaction_KnockDown();
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

void ADungeonCharacterBase::ResetHitActors()
{
	Inventory->ResetHitActors();
}

void ADungeonCharacterBase::SetIdleMode()
{
	State->SetIdleMode();
}

void ADungeonCharacterBase::SetSkillMode()
{
	State->SetSkillMode();
}

void ADungeonCharacterBase::SetHitMode()
{
	State->SetHitMode();
}

void ADungeonCharacterBase::SetKnockBackMode()
{
	State->SetKnockBackMode();
}

void ADungeonCharacterBase::SetKnockDownMode()
{
	State->SetKnockDownMode();
}

void ADungeonCharacterBase::SetDeadMode()
{
	State->SetDeadMode();
}
