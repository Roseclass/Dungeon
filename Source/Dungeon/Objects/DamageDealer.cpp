#include "Objects/DamageDealer.h"
#include "Global.h"
#include "Components/ShapeComponent.h"

#include "Characters/DungeonCharacterBase.h"
#include "Components/SkillComponent.h"

#include "Objects/CustomDamageType.h"

#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "Abilities/AttributeSetBase.h"
#include "Abilities/GameplayEffectContexts.h"
#include "Abilities/MMC_Damage.h"

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
		IAbilitySystemInterface* HitCharacter = Cast<IAbilitySystemInterface>(OtherActor);
        if (HitCharacter)
        {

            UAbilitySystemComponent* AbilitySystemComponent = HitCharacter->GetAbilitySystemComponent();
            if (AbilitySystemComponent && GamePlayEffectClass)
            {
				FGameplayEffectContextHandle EffectContextHandle = FGameplayEffectContextHandle(new FDamageTextEffectContext());
				EffectContextHandle.AddInstigator(GetOwner(), this);
				EffectContextHandle.AddHitResult(SweepResult);
				
				FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GamePlayEffectClass, 1.0f, EffectContextHandle);

				if (SpecHandle.IsValid())
				{
					FGameplayEffectSpec* EffectSpec = SpecHandle.Data.Get();
					if (EffectSpec)
					{
						FGameplayTag ForceTag = FGameplayTag::RequestGameplayTag(FName("Effect.Force"));
						float ForceValue = Force;
						EffectSpec->SetSetByCallerMagnitude(ForceTag, ForceValue);

						FGameplayTag DamageTag = FGameplayTag::RequestGameplayTag(FName("Effect.Damage"));
						float DamageValue = -Damage;
						EffectSpec->SetSetByCallerMagnitude(DamageTag, DamageValue);

						AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpec);
					}
				}
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