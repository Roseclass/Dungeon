#include "Components/DamageTextComponent.h"
#include "Global.h"
#include "Objects/DamageText.h"

UDamageTextComponent::UDamageTextComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDamageTextComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UDamageTextComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UDamageTextComponent::SpawnDamageText(FVector NewLocation, float InDamage, bool IsCritical)
{
	CheckNull(DamageTextClass);
	FTransform transform;
	transform.SetTranslation(NewLocation);
	ADamageText* txt = GetWorld()->SpawnActorDeferred<ADamageText>(DamageTextClass, transform);
	txt->InitialLifeSpan = 1;
	UGameplayStatics::FinishSpawningActor(txt, transform);
	txt->Init(InDamage, IsCritical);
}
