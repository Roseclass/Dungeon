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
#include "Abilities/GameplayAbility.h"
#include "Abilities/AttributeSetBase.h"

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
	AttributeSet = CreateDefaultSubobject<UAttributeSetBase>(TEXT("AttributeSet"));

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

FGenericTeamId ADungeonCharacterBase::GetGenericTeamId() const
{
	return TeamID;
}

UAbilitySystemComponent* ADungeonCharacterBase::GetAbilitySystemComponent() const
{
	return Skill;
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

	//// set move for addforce
	//SetMove();

	// restore state(bp notify)
}

void ADungeonCharacterBase::HitReaction_KnockDown()
{
	// stop 
	GetCharacterMovement()->StopMovementImmediately();

	//// set move for addforce
	//SetMove();

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

	Skill->InitAbilityActorInfo(this, this);

	// Attribute change callbacks
	HealthChangedDelegateHandle = Skill->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &ADungeonCharacterBase::HealthChanged);
	MaxHealthChangedDelegateHandle = Skill->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxHealthAttribute()).AddUObject(this, &ADungeonCharacterBase::MaxHealthChanged);
	ManaChangedDelegateHandle = Skill->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetManaAttribute()).AddUObject(this, &ADungeonCharacterBase::ManaChanged);
	MaxManaChangedDelegateHandle = Skill->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxManaAttribute()).AddUObject(this, &ADungeonCharacterBase::MaxManaChanged);
}

void ADungeonCharacterBase::Server_SetName_Implementation(const FText& NewName)
{
	Name = NewName;
	OnRep_Name();
}

//void ADungeonCharacterBase::ChangeState(EStateType PrevType, EStateType NewType)
//{
//	CheckTrue(PrevType == NewType);
//
//	if (NewType == EStateType::Dead)
//	{
//		Montage->PlayDeadMontage();
//
//		if (!HealthBarWidget)
//			HealthBarWidget = Cast<UUW_HealthBar>(HealthBar->GetWidget());
//		HealthBarWidget->Dead();
//
//		//ignore cursor
//		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
//		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
//		//ignore character
//		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
//
//	}
//	else if (NewType == EStateType::Hit)
//	{
//		Montage->PlayHitMontage();
//	}
//	else if (NewType == EStateType::KnockBack)
//	{
//		//playmontage
//	}
//	else if (NewType == EStateType::KnockDown)
//	{
//		Montage->PlayKnockDownMontage();
//	}
//}

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
	return Skill->CanUse();
}
bool ADungeonCharacterBase::CanMove()
{
	return Skill->CanMove();
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