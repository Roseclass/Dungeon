#include "Components/InventoryComponent.h"
#include "Global.h"

#include "Characters/DungeonCharacter.h"
#include "Objects/Weapon.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	InitDefault();
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UInventoryComponent::InitDefault()
{
	ADungeonCharacter* owner = Cast<ADungeonCharacter>(GetOwner());
	if (DefaultWeapon)
	{
		CurrentWeapon = GetWorld()->SpawnActor<AWeapon>(DefaultWeapon);
		CurrentWeapon->OffCollision();
		FAttachmentTransformRules f = { EAttachmentRule::SnapToTarget, 1 };
		CurrentWeapon->AttachToComponent(owner->GetMesh(), f, CurrentWeapon->GetSocketName());
		CurrentWeapon->SetOwner(owner);
		CurrentWeapon->SetTeamID(owner->GetGenericTeamId());
	}
}

void UInventoryComponent::OnCollision()
{
	if (CurrentWeapon)CurrentWeapon->OnCollision();
}

void UInventoryComponent::OffCollision()
{
	if (CurrentWeapon)CurrentWeapon->OffCollision();
}

void UInventoryComponent::ResetHittedActors()
{
	if (CurrentWeapon)CurrentWeapon->ResetHittedActors();
}
