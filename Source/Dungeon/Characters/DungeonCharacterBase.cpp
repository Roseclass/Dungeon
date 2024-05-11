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

void ADungeonCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicated 변수를 여기에 추가
	DOREPLIFETIME_CONDITION(ADungeonCharacterBase, Name, COND_None);
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
	// restore state(bp notify)
}

void ADungeonCharacterBase::HitReaction_KnockBack()
{
	// stop 
	GetCharacterMovement()->StopMovementImmediately();

	// set move for addforce
	SetMove();

	// restore state(bp notify)
}

void ADungeonCharacterBase::HitReaction_KnockDown()
{
	// stop 
	GetCharacterMovement()->StopMovementImmediately();

	// set move for addforce
	SetMove();

	// restore state(bp notify)
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

void ADungeonCharacterBase::OnRep_Name()
{
	if (!HealthBarWidget)
	{
		CLog::Print("OnRep_Name, HealthBarWidget is nullptr", -1, 10, FColor::Red);
		return;
	}
	HealthBarWidget->SetName(Name);
}

void ADungeonCharacterBase::Init()
{
	HealthBarWidget = Cast<UUW_HealthBar>(HealthBar->GetWidget());

	HealthBarWidget->Init(Name,Status->GetLevel());

	State->OnStateTypeChanged.AddUFunction(this, "ChangeState");

	Status->OnMaxHealthChanged.AddUFunction(this, "ChangeHealthBarMax");
	Status->OnCurrentHealthChanged.AddUFunction(this, "ChangeHealthBarPercent");
	Status->OnHealthRegenChanged.AddUFunction(this, "ChangeHealthBarRegen");

	Status->Update();
}

void ADungeonCharacterBase::Server_SetName_Implementation(const FText& NewName)
{
	Name = NewName;
	OnRep_Name();
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
		Montage->PlayHitMontage();
	}
	else if (NewType == EStateType::KnockBack)
	{
		//playmontage
	}
	else if (NewType == EStateType::KnockDown)
	{
		Montage->PlayKnockDownMontage();
	}
}

void ADungeonCharacterBase::HideHealthBar()
{
	HealthBar->SetHiddenInGame(1);
}

void ADungeonCharacterBase::RevealHealthBar()
{
	HealthBar->SetHiddenInGame(0);
}

bool ADungeonCharacterBase::CanUse()
{
	return State->IsIdleMode() && Status->CanUse();
}
bool ADungeonCharacterBase::CanMove()
{
	return !State->IsKnockBackMode() && !State->IsKnockDownMode() && Status->CanMove();
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
	HitReaction_Normal();
}

void ADungeonCharacterBase::SetKnockBackMode()
{
	State->SetKnockBackMode();
	HitReaction_KnockBack();
}

void ADungeonCharacterBase::SetKnockDownMode()
{
	State->SetKnockDownMode();
	HitReaction_KnockDown();
}

void ADungeonCharacterBase::SetDeadMode()
{
	//ignore cursor
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	//ignore character
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	State->SetDeadMode();
	SetStop();
}
