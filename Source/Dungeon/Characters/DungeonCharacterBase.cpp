#include "Characters/DungeonCharacterBase.h"
#include "Global.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Components/DecalComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "Characters/AttributeSetBase.h"

#include "SaveManager.h"
#include "DungeonPlayerController.h"
#include "Components/SkillComponent.h"
#include "Components/MontageComponent.h"
#include "Components/StateComponent.h"
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
	CHelpers::CreateActorComponent<UAbilitySystemComponent>(this, &AbilitySystem, "AbilitySystem");
	AttributeSet = CreateDefaultSubobject<UAttributeSetBase>(TEXT("AttributeSet"));

	CHelpers::CreateActorComponent<USkillComponent>(this, &Skill, "Skill");
	CHelpers::CreateActorComponent<UMontageComponent>(this, &Montage, "Montage");
	CHelpers::CreateActorComponent<UStateComponent>(this, &State, "State");
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
	//Status->AdjustCurrentHealth(-DamageAmount);
	//Skill->Abort();

	// set montage datas
	{
		Montage->SetDamageCauser(DamageCauser);

		FVector force = GetActorLocation() - DamageCauser->GetActorLocation();
		force.Z = 0;
		force.Normalize(); 

		Montage->SetForce(damageType->DamageImpulse * force);
	}

	// is dead?
	//float hp = Status->GetCurrentHealth_Server();
	//if (hp <= 0)
	//{
	//	SetDeadMode();
	//	return result;
	//}

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

UAbilitySystemComponent* ADungeonCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

void ADungeonCharacterBase::HealthChanged(const FOnAttributeChangeData& Data)
{
	if (!AttributeSet)
	{
		CLog::Print("ADungeonCharacterBase::HealthChanged, AttributeSet is nullptr", -1, 10, FColor::Red);
		return;
	}
	float maxHealth = AttributeSet->GetMaxHealth();

	if (!HealthBarWidget)
	{
		CLog::Print("ADungeonCharacterBase::HealthChanged, HealthBarWidget is nullptr", -1, 10, FColor::Red);
		return;
	}
	HealthBarWidget->SetPercent(Data.NewValue / maxHealth);
}

void ADungeonCharacterBase::MaxHealthChanged(const FOnAttributeChangeData& Data)
{

}
void ADungeonCharacterBase::ManaChanged(const FOnAttributeChangeData& Data)
{

}

void ADungeonCharacterBase::MaxManaChanged(const FOnAttributeChangeData& Data)
{

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
		HealthBarWidget = Cast<UUW_HealthBar>(HealthBar->GetWidget());
	if (HealthBarWidget)
		HealthBarWidget->SetName(Name);
}

void ADungeonCharacterBase::Init()
{
	HealthBarWidget = Cast<UUW_HealthBar>(HealthBar->GetWidget());

	//HealthBarWidget->Init(Name,Status->GetLevel());

	State->OnStateTypeChanged.AddUFunction(this, "ChangeState");


	AbilitySystem->InitAbilityActorInfo(this, this);
	if (HasAuthority())
	{
		FTimerHandle WaitHandle;
		float WaitTime = 10;
		GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			CLog::Print("GiveAbility");
			AbilitySystem->GiveAbility(FGameplayAbilitySpec(DefaultAbilities[0]));
		}), WaitTime, false);
	}
	if (HasAuthority())
	{
		FTimerHandle WaitHandle;
		float WaitTime = 20;
		GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			CLog::Print("PLAY");
			FGameplayAbilitySpec* handle = AbilitySystem->FindAbilitySpecFromClass(DefaultAbilities[0]);
			AbilitySystem->TryActivateAbility(handle->Handle);
		}), WaitTime, false);
	}

	//{
	//	FTimerHandle WaitHandle;
	//	float WaitTime = 3;
	//	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
	//	{
	//		AbilitySystem->InitAbilityActorInfo(this, this);
	//	}), WaitTime, false);
	//}

	// Attribute change callbacks
	HealthChangedDelegateHandle = AbilitySystem->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &ADungeonCharacterBase::HealthChanged);
	MaxHealthChangedDelegateHandle = AbilitySystem->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxHealthAttribute()).AddUObject(this, &ADungeonCharacterBase::MaxHealthChanged);
	ManaChangedDelegateHandle = AbilitySystem->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetManaAttribute()).AddUObject(this, &ADungeonCharacterBase::ManaChanged);
	MaxManaChangedDelegateHandle = AbilitySystem->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxManaAttribute()).AddUObject(this, &ADungeonCharacterBase::MaxManaChanged);
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

		if (!HealthBarWidget)
			HealthBarWidget = Cast<UUW_HealthBar>(HealthBar->GetWidget());
		HealthBarWidget->Dead();

		//ignore cursor
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		//ignore character
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

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
	return State->IsIdleMode() /*&& Status->CanUse()*/;
}
bool ADungeonCharacterBase::CanMove()
{
	return !State->IsKnockBackMode() && !State->IsKnockDownMode() /*&& Status->CanMove()*/;
}

void ADungeonCharacterBase::SetUse()
{
	//Status->SetUse();
}

void ADungeonCharacterBase::SetCannotUse()
{
	//Status->SetCannotUse();
}

void ADungeonCharacterBase::SetMove()
{
	//Status->SetMove();
}

void ADungeonCharacterBase::SetStop()
{
	//Status->SetStop();
}

void ADungeonCharacterBase::UnsetSkill()
{

}

void ADungeonCharacterBase::SpawnProjectile()
{
	//Skill->SpawnProjectile();
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
	State->SetDeadMode();
	SetStop();
}
