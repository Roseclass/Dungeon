#include "Objects/Weapon.h"
#include "Global.h"
#include "Components/ShapeComponent.h"

#include "DungeonPlayerController.h"
#include "Characters/DungeonCharacterBase.h"
#include "Components/SkillComponent.h"
#include "Components/InventoryComponent.h"

#include "Objects/ItemObject.h"

#include "Abilities/GA_Skill.h"
#include "Abilities/GameplayEffectContexts.h"
#include "Abilities/MMC_Damage.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::FindComponents()
{
	Super::FindComponents();

	// find collision
	TArray<UShapeComponent*> shapeComponents;
	GetComponents<UShapeComponent>(shapeComponents);
	for (UShapeComponent* component : shapeComponents)
	{
		for (auto i : component->ComponentTags)
		{
			if (i == FName("OverlapCollision"))
				CollisionComponents.Add(component);
		}
	}
	for (UShapeComponent* component : CollisionComponents)
	{
		component->OnComponentBeginOverlap.Clear();
		component->OnComponentHit.Clear();
		component->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnComponentBeginOverlap);
		component->OnComponentHit.AddDynamic(this, &AWeapon::OnComponentHit);
	}
}

void AWeapon::SetOwnerCharacter(ADungeonCharacterBase* InCharacter)
{
	Super::SetOwnerCharacter(InCharacter);
	IGenericTeamAgentInterface* id = Cast<IGenericTeamAgentInterface>(InCharacter);
	CheckNull(id);
	SetTeamID(id->GetGenericTeamId());
}

void AWeapon::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CheckFalse(HasAuthority());
	CheckTrue(HitActors.Contains(OtherActor));
	ADungeonCharacterBase* owner = Cast<ADungeonCharacterBase>(GetOwner());
	CheckNull(owner);

	IGenericTeamAgentInterface* other = Cast<IGenericTeamAgentInterface>(OtherActor);
	ADungeonCharacterBase* otherCh = Cast<ADungeonCharacterBase>(OtherActor);
	CheckTrue(!other && !otherCh);

	USkillComponent* skill = CHelpers::GetComponent<USkillComponent>(OtherActor);
	CheckNull(skill);
	CheckTrue(skill->IsDead());

	// ignore alliance
	CheckTrue(other->GetGenericTeamId() == TeamID);

	// �ٸ� ������ effect�� ������
	// ��Ʈ����Ʈ���� �ջ굥������ ������
	// ������ ����


	TArray<TSubclassOf<UGameplayEffect>> effects;
	if(CommonEffectClass)effects.Add(CommonEffectClass);

	// get owner's equpment effects
	{	
		UInventoryComponent* inv = CHelpers::GetComponent<UInventoryComponent>(owner);
		if (inv)inv->GetEquipmentEffectClasses(effects);
	}

	// Apply Damage Effect
	{
		// Set instigator asc
		USkillComponent* hitASC = Cast<USkillComponent>(otherCh->GetAbilitySystemComponent());
		USkillComponent* instigatorASC = Cast<USkillComponent>(owner->GetAbilitySystemComponent());

		// Make effectcontext handle
		FDamageEffectContext* context = new FDamageEffectContext();
		FGameplayEffectContextHandle EffectContextHandle = FGameplayEffectContextHandle(context);
		EffectContextHandle.AddInstigator(owner->GetController(), this);
		EffectContextHandle.AddHitResult(SweepResult);

		context->BaseDamage = instigatorASC->GetPower();
		context->CalculatedDamage = context->BaseDamage;

		if (DamageData)
		{
			context->CalculatedDamage += DamageData->Additive;
			context->CalculatedDamage *= DamageData->Multiplicitive;
		}
		else CLog::Print("AWeapon::OnComponentBeginOverlap DamageData is nullptr");

		ADungeonPlayerController* pc = Cast<ADungeonPlayerController>(owner->GetController());
		for (auto effct : effects)
		{
			if (pc)
			{	
				// Pre-calculate MMC value and setting DamageText datas
				UMMC_Damage* MyMMC = Cast<UMMC_Damage>(UMMC_Damage::StaticClass()->GetDefaultObject());
				instigatorASC->Cient_DamageText(MyMMC->CalculateDamageTextValue(context, hitASC), 0, OtherActor->GetActorLocation());
			}
			// Must use EffectToTarget for auto mmc
			instigatorASC->ApplyGameplayEffectToTarget(effct.GetDefaultObject(), hitASC, UGameplayEffect::INVALID_LEVEL, EffectContextHandle);
		}
	}
}

void AWeapon::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

}

void AWeapon::OnCollision(const FDamageEhancementData* InDamageData)
{
	for (auto i : CollisionComponents)
		i->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DamageData = InDamageData;
}

void AWeapon::OffCollision()
{
	for (auto i : CollisionComponents)
		i->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DamageData = nullptr;
}

void AWeapon::ResetHitActors()
{
	HitActors.Empty();
}
