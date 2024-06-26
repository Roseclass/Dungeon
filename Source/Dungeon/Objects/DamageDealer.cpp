#include "Objects/DamageDealer.h"
#include "Global.h"
#include "Components/ShapeComponent.h"

#include "Characters/DungeonCharacterBase.h"
#include "Components/SkillComponent.h"

#include "DungeonPlayerController.h"

#include "AbilitySystemInterface.h"
#include "AbilitySystemGlobals.h"
#include "GameplayEffect.h"
#include "GameplayCueManager.h"

#include "Abilities/AttributeSetBase.h"
#include "Abilities/GameplayEffectContexts.h"
#include "Abilities/MMC_Damage.h"
#include "GameplayEffectExecutionCalculation.h"

ADamageDealer::ADamageDealer()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADamageDealer::BeginPlay()
{
	Super::BeginPlay();
	FindCollision();
}

void ADamageDealer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADamageDealer::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CheckFalse(HasAuthority());

	// already hit?
	if (GetDamagedActors().Contains(OtherActor))return;

	// overlap with DungeonCharacterBase
	ADungeonCharacterBase* base = Cast<ADungeonCharacterBase>(OtherActor);
	if (!base)return;

	// is deadmode?
	USkillComponent* skill = CHelpers::GetComponent<USkillComponent>(OtherActor);
	if (!skill)return;
	if (skill->IsDead())return;

	// ignore alliance
	CheckTrue(base->GetGenericTeamId() == TeamID);

	// set properties
	OverlappedActors.AddUnique(OtherActor);
	CurrentOverlappedActor = OtherActor;

	// send Damage
	if (OtherActor && OtherActor != this)
    {
		// Check hit actor
		IAbilitySystemInterface* hitCharacter = Cast<IAbilitySystemInterface>(OtherActor);
        if (hitCharacter)
        {
			// Get asc
            UAbilitySystemComponent* hitASC = hitCharacter->GetAbilitySystemComponent();
            if (hitASC && GamePlayEffectClass)
            {
				// Make effectcontext handle
				FDamageEffectContext* context = new FDamageEffectContext();
				FGameplayEffectContextHandle EffectContextHandle = FGameplayEffectContextHandle(context);
				ADungeonCharacterBase* owner = Cast<ADungeonCharacterBase>(GetOwner());
				EffectContextHandle.AddInstigator(owner ? owner->GetController() : nullptr, this);
				EffectContextHandle.AddHitResult(SweepResult);
				context->BaseDamage = Damage;
				context->Force = Force;
				
				// Set instigator asc
				USkillComponent* instigatorASC = Cast<USkillComponent>(hitASC);
				if (owner)instigatorASC = Cast<USkillComponent>(owner->GetAbilitySystemComponent());

				// Pre-calculate MMC value and setting DamageText datas
				UMMC_Damage* MyMMC = Cast<UMMC_Damage>(UMMC_Damage::StaticClass()->GetDefaultObject());
				instigatorASC->Cient_DamageText(MyMMC->CalculateDamageTextValue(context, hitASC), 0, OtherActor->GetActorLocation());

				// Must use EffectToTarget for auto mmc
				instigatorASC->ApplyGameplayEffectToTarget(GamePlayEffectClass.GetDefaultObject(), hitASC, UGameplayEffect::INVALID_LEVEL, EffectContextHandle);

				delete context;
				context = nullptr;
            }
        }
    }
}

void ADamageDealer::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	CheckFalse(HasAuthority());

	// set properties
	OverlappedActors.Remove(OtherActor);
	if (CurrentOverlappedActor == OtherActor && OverlappedActors.Num())
		CurrentOverlappedActor = OverlappedActors.Last();
}

void ADamageDealer::ResetDamagedActors()
{
	// reset property
	DamagedActors.Empty();
}

void ADamageDealer::FindCollision()
{
	TArray<UShapeComponent*> shapeComponents;
	GetComponents<UShapeComponent>(shapeComponents);
	for (UShapeComponent* component : shapeComponents)
	{
		for (auto i : component->ComponentTags)
		{
			if (i == OverlapComponentTag)
			{
				CollisionComponents.Add(component);
				break;
			}
		}
	}
	for (UShapeComponent* component : CollisionComponents)
	{
		component->OnComponentBeginOverlap.Clear();
		component->OnComponentEndOverlap.Clear();
		component->OnComponentBeginOverlap.AddDynamic(this, &ADamageDealer::OnComponentBeginOverlap);
		component->OnComponentEndOverlap.AddDynamic(this, &ADamageDealer::OnComponentEndOverlap);
	}
}

void ADamageDealer::Activate()
{
	bAct = 1;
}

void ADamageDealer::Deactivate()
{
	bAct = 0;
}