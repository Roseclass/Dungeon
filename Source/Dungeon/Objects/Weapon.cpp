#include "Objects/Weapon.h"
#include "Global.h"
#include "Components/ShapeComponent.h"

#include "Characters/DungeonCharacterBase.h"
#include "Components/SkillComponent.h"

#include "Objects/ItemManager.h"
#include "Objects/ItemObject.h"

#include "Abilities/GA_Skill.h"
#include "Abilities/GameplayEffectContexts.h"
#include "Abilities/GE_UniqueItemEffect.h"
#include "Abilities/MMC_Damage.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = 1;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicated 변수를 여기에 추가
	//DOREPLIFETIME_CONDITION(AWeapon, Mode, COND_None);
}

void AWeapon::OnRep_Mode()
{
	Super::OnRep_Mode();
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

void AWeapon::SpawnLootEffects()
{
	Super::SpawnLootEffects();
}

void AWeapon::SetEffectLocation()
{
	Super::SetEffectLocation();
}

void AWeapon::SortMesh()
{
	Super::SortMesh();
}

void AWeapon::ActivateEffect()
{
	Super::ActivateEffect();
}

void AWeapon::DeactivateEffect()
{
	Super::DeactivateEffect();
}

void AWeapon::SetPickableMode()
{
	// Off Collision
	for (UShapeComponent* component : CollisionComponents)
		component->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Super::SetPickableMode();
}

void AWeapon::SetInventoryMode()
{
	// Off Collision
	for (UShapeComponent* component : CollisionComponents)
		component->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Super::SetInventoryMode();
}

void AWeapon::SetEquipMode()
{
	// Off Collision
	for (UShapeComponent* component : CollisionComponents)
		component->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	Super::SetEquipMode();
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

	// Apply Base Damage Effect
	{
		// Make effectcontext handle
		FDamageEffectContext* context = new FDamageEffectContext();
		FGameplayEffectContextHandle EffectContextHandle = FGameplayEffectContextHandle(context);
		EffectContextHandle.AddInstigator(owner->GetController(), this);
		EffectContextHandle.AddHitResult(SweepResult);

		float tempDamage = Damage;

		if (DamageData)
		{
			tempDamage += DamageData->Additive;
			tempDamage *= DamageData->Multiplicitive;
		}
		else CLog::Print("AWeapon::OnComponentBeginOverlap DamageData is nullptr");

		context->BaseDamage = tempDamage;

		// Set instigator asc
		USkillComponent* hitASC = Cast<USkillComponent>(otherCh->GetAbilitySystemComponent());
		USkillComponent* instigatorASC = Cast<USkillComponent>(otherCh->GetAbilitySystemComponent());
		if (owner)instigatorASC = Cast<USkillComponent>(owner->GetAbilitySystemComponent());

		// Pre-calculate MMC value and setting DamageText datas
		UMMC_Damage* MyMMC = Cast<UMMC_Damage>(UMMC_Damage::StaticClass()->GetDefaultObject());
		instigatorASC->Cient_DamageText(MyMMC->CalculateDamageTextValue(context, hitASC), 0, OtherActor->GetActorLocation());

		// Must use EffectToTarget for auto mmc
		instigatorASC->ApplyGameplayEffectToTarget(CommonEffectClass.GetDefaultObject(), hitASC, UGameplayEffect::INVALID_LEVEL, EffectContextHandle);

		delete context;
		context = nullptr;
	}

	// Apply Damage Effect
	{
		// Make effectcontext handle
		FDamageEffectContext* context = new FDamageEffectContext();
		FGameplayEffectContextHandle EffectContextHandle = FGameplayEffectContextHandle(context);
		EffectContextHandle.AddInstigator(owner->GetController(), this);
		EffectContextHandle.AddHitResult(SweepResult);

		float tempDamage = Damage;

		if (DamageData)
		{
			tempDamage += DamageData->Additive;
			tempDamage *= DamageData->Multiplicitive;
		}
		else CLog::Print("AWeapon::OnComponentBeginOverlap DamageData is nullptr");

		context->BaseDamage = Damage;
		context->CalculatedDamage = tempDamage;

		// Set instigator asc
		USkillComponent* hitASC = Cast<USkillComponent>(otherCh->GetAbilitySystemComponent());
		USkillComponent* instigatorASC = Cast<USkillComponent>(owner->GetAbilitySystemComponent());

		TArray<TSubclassOf<UGameplayEffect>> effects = UniqueEffectClasses;
		effects.Add(CommonEffectClass);

		for (auto effct : effects)
		{
			// Pre-calculate MMC value and setting DamageText datas
			UMMC_Damage* MyMMC = Cast<UMMC_Damage>(UMMC_Damage::StaticClass()->GetDefaultObject());
			instigatorASC->Cient_DamageText(MyMMC->CalculateDamageTextValue(context, hitASC), 0, OtherActor->GetActorLocation());

			// Must use EffectToTarget for auto mmc
			instigatorASC->ApplyGameplayEffectToTarget(effct.GetDefaultObject(), hitASC, UGameplayEffect::INVALID_LEVEL, EffectContextHandle);

			delete context;
			context = nullptr;
		}
	}
}

void AWeapon::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

}

void AWeapon::SetOwnerCharacter(ACharacter* InCharacter)
{
	Super::SetOwnerCharacter(InCharacter);
	IGenericTeamAgentInterface* id = Cast<IGenericTeamAgentInterface>(InCharacter);
	CheckNull(id)
	SetTeamID(id->GetGenericTeamId());
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

void AWeapon::SetItemLocation(const FVector& NewLocation, bool bSweep, FHitResult* OutSweepHitResult, ETeleportType Teleport)
{
	Super::SetItemLocation(NewLocation, bSweep, OutSweepHitResult, Teleport);
}

void AWeapon::SetItemRotation(FRotator NewRotation, ETeleportType Teleport)
{
	Super::SetItemRotation(NewRotation, Teleport);
}

void AWeapon::AttachItemToComponent(USceneComponent* Parent, const FAttachmentTransformRules& AttachmentRules, FName InSocketName)
{
	Super::AttachItemToComponent(Parent,AttachmentRules,InSocketName);
}

void AWeapon::ChangeVisibility(EItemMode InMode)
{
	Super::ChangeVisibility(InMode);
}

void AWeapon::SetMode(EItemMode InMode)
{
	Super::SetMode(InMode);
}

void AWeapon::GetUniqueEffectDescriptions(TArray<FString>& Descriptions)const
{
	// TODO::Check
	for (auto effect : UniqueEffectClasses)
	{
		UGE_UniqueItemEffect * unique = Cast<UGE_UniqueItemEffect>(effect.GetDefaultObject());
		if (!unique)continue;
		Descriptions.Add(unique->GetDescription());
	}
}