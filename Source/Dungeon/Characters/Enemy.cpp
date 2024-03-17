#include "Characters/Enemy.h"
#include "Global.h"
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"

#include "Components/SkillComponent.h"
#include "Components/MontageComponent.h"
#include "Components/StateComponent.h"
#include "Components/StatusComponent.h"
#include "Components/InventoryComponent.h"

#include "Widgets/UW_HealthBar.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	TeamID = 2;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	//scene
	CHelpers::CreateComponent(this, &HealthBarRoot, "HealthBarRoot", RootComponent);
	CHelpers::CreateComponent(this, &HealthBar, "HealthBar", HealthBarRoot);

	//actor
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	Init();
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float result = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// refresh status
	Status->AdjustCurrentHealth(-DamageAmount);

	// hit react by montage component

	return result;
}

FGenericTeamId AEnemy::GetGenericTeamId() const
{
	return TeamID;
}

void AEnemy::ChangeHealthBarPercent(float NewPercent)
{
	if (!HealthBarWidget)
	{
		CLog::Print("ChangeHealthBarPercent, HealthBarWidget is nullptr", -1, 10, FColor::Red);
		return;
	}
	HealthBarWidget->SetPercent(NewPercent);
}

void AEnemy::Multicast_UseSkill_Implementation(int32 InIndex)
{
	CLog::Print("UseSkill " + FString::FromInt(InIndex));
	Skill->UseSkill(InIndex);
}

void AEnemy::Init()
{
	Super::Init();

	HealthBarWidget = Cast<UUW_HealthBar>(HealthBar->GetWidget());
	HealthBarWidget->Init(Name, Level);

	Status->OnCurrentHealthChanged.BindUFunction(this, "ChangeHealthBarPercent");

	if (HasAuthority())
		Skill->SpawnSkillActors();
}

void AEnemy::UseSkill(int32 Idx)
{
	Super::UseSkill(Idx);
	State->SetSkillMode();
	Multicast_UseSkill(Idx);
}
