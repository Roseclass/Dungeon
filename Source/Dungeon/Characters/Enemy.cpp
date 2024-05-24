#include "Characters/Enemy.h"
#include "Global.h"
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"

#include "Characters/EnemyAIController.h"

#include "Components/SkillComponent.h"
#include "Components/MontageComponent.h"
#include "Components/StateComponent.h"
#include "Components/StatusComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/DamageTextComponent.h"
#include "Components/LootComponent.h"
#include "Components/HitEffectComponent.h"

#include "Objects/Quest.h"

#include "Widgets/UW_HealthBar.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	TeamID = 2;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);	

	//actor
	CHelpers::CreateActorComponent<UDamageTextComponent>(this, &DamageText, "DamageText");
	CHelpers::CreateActorComponent<ULootComponent>(this, &Loot, "Loot");
	CHelpers::CreateActorComponent<UHitEffectComponent>(this, &HitEffect, "HitEffect");
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float result = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// hit react by montage component
	Multicast_TakeDamage(DamageAmount, 0);

	return result;
}

FGenericTeamId AEnemy::GetGenericTeamId() const
{
	return TeamID;
}

void AEnemy::CompleteCondition()
{
	//TODO:: check condition, Find LinkedQuest
	LinkedComponent;
}

void AEnemy::Multicast_UseSkill_Implementation(int32 InIndex)
{
	Skill->UseSkill(InIndex);
}

void AEnemy::Multicast_TakeDamage_Implementation(float InDamage, bool IsCritical)
{
	HitEffect->Glow();
	DamageText->SpawnDamageText(GetActorLocation(), InDamage, IsCritical);
}

void AEnemy::SequenceStart()
{
	State->SetSequenceMode();
}

void AEnemy::SequenceEnd()
{
	State->SetIdleMode();
}

void AEnemy::Init()
{
	Super::Init();

	//if (HealthBarWidget)
	//{
	//	HealthBarWidget->SetEliteMonsterType();
	//}
	//else CLog::Print("AEnemy::Init HealthBarWidget cast failed", -1, 10, FColor::Red);

}

void AEnemy::PlaySequence_Implementation(int32 InIndex)
{
	SequenceStart();
}

void AEnemy::UseSkill(int32 Idx)
{
	Super::UseSkill(Idx);
	State->SetSkillMode();
	Multicast_UseSkill(Idx);
}

void AEnemy::SetDeadMode()
{
	Super::SetDeadMode();

	AEnemyAIController* controller = Cast<AEnemyAIController>(GetController());
	if(controller)controller->StopLogic("");

	// Drop Items
	Loot->DropItems();
}

void AEnemy::GenerateLootItems()
{
	Loot->GenerateItems();
}

void AEnemy::SpawnWarningSign(int32 InIndex)
{
	Skill->SpawnWarningSign(InIndex);
}
