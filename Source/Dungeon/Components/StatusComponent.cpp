#include "Components/StatusComponent.h"
#include "Global.h"

UStatusComponent::UStatusComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UStatusComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UStatusComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UStatusComponent::SetUse()
{
	bCanUse = 1;
}

void UStatusComponent::SetCannotUse()
{
	bCanUse = 0;
}

void UStatusComponent::SetMove()
{
	bCanMove = 1;
}

void UStatusComponent::SetStop()
{
	bCanMove = 0;
}
