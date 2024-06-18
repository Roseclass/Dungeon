#include "Characters/Enemy.h"
#include "Global.h"
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"

#include "Characters/EnemyAIController.h"

#include "Components/SkillComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/DamageTextComponent.h"
#include "Components/LootComponent.h"
#include "Components/HitEffectComponent.h"
#include "Components/QuestComponent.h"

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
	QuestObjective_Init();
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FGenericTeamId AEnemy::GetGenericTeamId() const
{
	return TeamID;
}

void AEnemy::CompleteCondition()
{
	LinkedComponent->CheckCondition(this);
}

//void AEnemy::Multicast_TakeDamage_Implementation(float InDamage, bool IsCritical)
//{
//	HitEffect->Glow();
//	DamageText->SpawnDamageText(GetActorLocation(), InDamage, IsCritical);
//}

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
	//SequenceStart();
}

void AEnemy::UseSkill(int32 Idx)
{
	Super::UseSkill(Idx);
	//Multicast_UseSkill(Idx);
}

//void AEnemy::SetDeadMode()
//{
//	Super::SetDeadMode();
//	CompleteCondition();
//
//	AEnemyAIController* controller = Cast<AEnemyAIController>(GetController());
//	if(controller)controller->StopLogic("");
//
//	// Drop Items
//	Loot->DropItems();
//}

void AEnemy::GenerateLootItems()
{
	Loot->GenerateItems();
}

void AEnemy::SpawnWarningSign(int32 InIndex)
{

}
