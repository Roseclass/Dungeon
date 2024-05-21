#include "Objects/DamageDealer.h"
#include "Global.h"
#include "Components/ShapeComponent.h"

#include "Characters/DungeonCharacterBase.h"
#include "Components/StateComponent.h"

#include "Objects/CustomDamageType.h"

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

	CLog::Print(FString::Printf(TEXT("%s : %i"), *GetName(), TeamID), -1, 0);
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
	UStateComponent* state = CHelpers::GetComponent<UStateComponent>(OtherActor);
	if (!state)return;
	if (state->IsDeadMode())return;

	// ignore alliance
	CheckTrue(base->GetGenericTeamId() == TeamID);

	// set properties
	OverlappedActors.AddUnique(OtherActor);
	CurrentOverlappedActor = OtherActor;

	// send Damage
	SendDamage(Damage, OtherActor, SweepResult);
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

void ADamageDealer::SendDamage(float InDamage, AActor* OtherActor, const FHitResult& SweepResult)
{
	CheckFalse(HasAuthority());

	ACharacter* ch = Cast<ACharacter>(GetOwner());
	AController* inst = nullptr;
	if (ch)inst = ch->GetController();

	switch (DamageType)
	{
	case EDamageType::Normal:SendNormalDamage(InDamage, OtherActor, SweepResult, inst); break;
	case EDamageType::Point:SendPointDamage(InDamage, OtherActor, SweepResult, inst);break;
	case EDamageType::Radial:SendRadialDamage(InDamage, OtherActor, SweepResult, inst); break;
	case EDamageType::Radial_Falloff:SendRadial_FalloffDamage(InDamage, OtherActor, SweepResult, inst); break;
	case EDamageType::Max:break;
	default:break;
	}

	DamagedActors.AddUnique(OtherActor);
}

void ADamageDealer::SendNormalDamage(float InDamage, AActor* OtherActor, const FHitResult& SweepResult, AController* EventInstigator)
{
	FDamageEvent damageEvent;
	damageEvent.DamageTypeClass = DamageTypeClass;
	OtherActor->TakeDamage(InDamage, damageEvent, EventInstigator, this);
}

void ADamageDealer::SendPointDamage(float InDamage, AActor* OtherActor, const FHitResult& SweepResult, AController* EventInstigator)
{
	
}

void ADamageDealer::SendRadialDamage(float InDamage, AActor* OtherActor, const FHitResult& SweepResult, AController* EventInstigator)
{
}

void ADamageDealer::SendRadial_FalloffDamage(float InDamage, AActor* OtherActor, const FHitResult& SweepResult, AController* EventInstigator)
{

}