#include "Objects/ItemManager.h"
#include "Global.h"

#include "DungeonPlayerController.h"
#include "Characters/DungeonCharacter.h"
#include "Objects/Weapon.h"
#include "Objects/ItemObject.h"

AItemManager::AItemManager()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = 1;
}

void AItemManager::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle WaitHandle;
	float WaitTime = 10;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
	{
		
	}), WaitTime, false);

}

void AItemManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AItemManager::Server_SetItemLocation_Implementation(AWeapon* InItem, const FVector& NewLocation, bool bSweep, const FHitResult& OutSweepHitResult, ETeleportType Teleport)
{
	FHitResult f = OutSweepHitResult;
	InItem->SetActorLocation(NewLocation, bSweep, &f, Teleport);
}

void AItemManager::Server_SetItemRotation_Implementation(AWeapon* InItem, const FRotator& NewRotation, ETeleportType Teleport)
{
	InItem->SetActorRotation(NewRotation, Teleport);
}

void AItemManager::Server_ChangeVisibility_Implementation(AWeapon* InItem, EItemMode NewMode)
{
	InItem->SetMode(NewMode);
}

void AItemManager::Server_AttachItemToComponent_Implementation(AWeapon* InItem, USceneComponent* Parent, EAttachmentRule Location, EAttachmentRule Rotation, EAttachmentRule Scale, bool bWeldSimulatedBodies, const FName& SocketName)
{
	FAttachmentTransformRules rule = { Location ,Rotation ,Scale ,bWeldSimulatedBodies };

	InItem->AttachToComponent(Parent, rule, SocketName);
}

void AItemManager::SetItemLocation(AWeapon* InItem, const FVector& NewLocation, bool bSweep, FHitResult* OutSweepHitResult, ETeleportType Teleport)
{
	FHitResult f;
	if (OutSweepHitResult == nullptr)
		OutSweepHitResult = &f;
	Server_SetItemLocation(InItem, NewLocation, bSweep, *OutSweepHitResult, Teleport);
}

void AItemManager::SetItemRotation(AWeapon* InItem, FRotator NewRotation, ETeleportType Teleport)
{
	Server_SetItemRotation(InItem, NewRotation, Teleport);
}

void AItemManager::AttachItemToComponent(AWeapon* InItem, USceneComponent* Parent, const FAttachmentTransformRules& AttachmentRules, FName SocketName)
{
	Server_AttachItemToComponent(
		InItem, 
		Parent, 
		AttachmentRules.LocationRule,
		AttachmentRules.RotationRule,
		AttachmentRules.ScaleRule,
		AttachmentRules.bWeldSimulatedBodies,
		SocketName);
}

void AItemManager::ChangeVisibility(AWeapon* InItem, EItemMode NewMode)
{
	Server_ChangeVisibility(InItem, NewMode);
}

AWeapon* AItemManager::SpawnItem(UClass* Class, FTransform const& Transform, const FActorSpawnParameters& SpawnParameters)
{
	AWeapon* result = GetWorld()->SpawnActor<AWeapon>(Class, Transform, SpawnParameters);
	if (result)result->SetManager(this);
	return result;
}

AWeapon* AItemManager::SpawnItemDeferred(UClass* Class, FTransform const& Transform, AActor* NewOwner, APawn* NewInstigator, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride)
{
	AWeapon* result = GetWorld()->SpawnActorDeferred<AWeapon>(Class, Transform, NewOwner, NewInstigator, CollisionHandlingOverride);
	if (result)result->SetManager(this);
	return result;
}
