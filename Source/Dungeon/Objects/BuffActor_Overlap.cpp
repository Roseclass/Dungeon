#include "Objects/BuffActor_Overlap.h"
#include "Global.h"
#include "Components/ShapeComponent.h"
#include "AbilitySystemInterface.h"

#include "Components/SkillComponent.h"

ABuffActor_Overlap::ABuffActor_Overlap()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABuffActor_Overlap::BeginPlay()
{
	Super::BeginPlay();
	FindComponents();
}

void ABuffActor_Overlap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABuffActor_Overlap::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CheckFalse(HasAuthority());
	IAbilitySystemInterface* other = Cast<IAbilitySystemInterface>(OtherActor);
	CheckNull(other);
	USkillComponent* skill = Cast<USkillComponent>(other->GetAbilitySystemComponent());
	CheckNull(skill);
	skill->ApplyGameplayEffectToSelf(EffectClass.GetDefaultObject(), UGameplayEffect::INVALID_LEVEL, skill->MakeEffectContext());
	Destroy();
}

void ABuffActor_Overlap::FindComponents()
{
	TArray<UShapeComponent*> shapeComponents;
	GetComponents<UShapeComponent>(shapeComponents);
	for (UShapeComponent* component : shapeComponents)
	{
		for (auto i : component->ComponentTags)
		{
			if (i == OverlapTag)
				OverlapCollisionComponents.Add(component);
		}
	}
	if (!OverlapCollisionComponents.Num())
		CLog::Print("ABuffActor_Overlap::FindComponents OverlapCollisionComponent is nullptr", -1, 10, FColor::Red);

	for (UShapeComponent* component : OverlapCollisionComponents)
	{
		component->OnComponentBeginOverlap.Clear();
		component->OnComponentBeginOverlap.AddDynamic(this, &ABuffActor_Overlap::OnComponentBeginOverlap);
	}
}