#include "Components/MPCComponent.h"

UMPCComponent::UMPCComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMPCComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMPCComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

