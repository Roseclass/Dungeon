#include "Components/AppearanceComponent.h"
#include "Global.h"

UAppearanceComponent::UAppearanceComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAppearanceComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UAppearanceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

