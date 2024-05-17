#include "Objects/ItemManager.h"
#include "Global.h"
#include "Components/CapsuleComponent.h"

#include "DungeonPlayerController.h"
#include "Characters/Enemy.h"
#include "Characters/DungeonCharacterBase.h"
#include "Objects/ItemObject.h"

AItemManager::AItemManager()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = 1;
}

void AItemManager::BeginPlay()
{
	Super::BeginPlay();

	{
		TArray<AActor*> arr;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEqquipment::StaticClass(), arr);
		for (auto i : arr)
			Cast<AEqquipment>(i)->SetManager(this);
	}

	FTimerHandle WaitHandle;
	float WaitTime = 1;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
	{
		if (HasAuthority())
		{
			TArray<AActor*> arr;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), arr);
			for (auto i : arr)
				Cast<AEnemy>(i)->GenerateLootItems();
		}
	}), WaitTime, false);

}

void AItemManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AItemManager::Server_SetItemLocation_Implementation(AEqquipment* InItem, const FVector& NewLocation, bool bSweep, const FHitResult& OutSweepHitResult, ETeleportType Teleport)
{
	CheckNull(InItem);
	FHitResult f = OutSweepHitResult;
	InItem->SetActorLocation(NewLocation, bSweep, &f, Teleport);
}

void AItemManager::Server_SetItemRotation_Implementation(AEqquipment* InItem, const FRotator& NewRotation, ETeleportType Teleport)
{
	CheckNull(InItem);
	InItem->SetActorRotation(NewRotation, Teleport);
}

void AItemManager::Server_AttachItemToComponent_Implementation(AEqquipment* InItem, USceneComponent* Parent, EAttachmentRule Location, EAttachmentRule Rotation, EAttachmentRule Scale, bool bWeldSimulatedBodies, const FName& SocketName)
{
	CheckNull(InItem);
	FAttachmentTransformRules rule = { Location ,Rotation ,Scale ,bWeldSimulatedBodies };

	InItem->AttachToComponent(Parent, rule, SocketName);

	AWeapon* weapon = Cast<AWeapon>(InItem);
	if(weapon)
		weapon->SetActorRelativeTransform(weapon->GetTransform());
}

void AItemManager::Server_ChangeVisibility_Implementation(AEqquipment* InItem, EItemMode NewMode)
{
	CheckNull(InItem);
	InItem->SetMode(NewMode);

	if (NewMode == EItemMode::Loot)
	{
		ACharacter* ch = InItem->GetOwnerCharacter();
		// check location
		if (!ch)
		{
			CLog::Print("AItemManager::Server_ChangeVisibility_Implementation OwnerCharacter is nullptr", -1, 10, FColor::Red);
			return;
		}

		// find drop location
		float halfSize = ch->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

		FVector start = ch->GetActorLocation();
		start.Z += halfSize;

		FVector end = ch->GetActorLocation();

		TArray<AActor*> arr; FHitResult hit; TArray<TEnumAsByte<EObjectTypeQuery>> types;
		types.Add(EObjectTypeQuery::ObjectTypeQuery1);//world static
		types.Add(EObjectTypeQuery::ObjectTypeQuery2);//world dynamic

		if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), start, start + ch->GetActorForwardVector() * 50, types, 0, arr, EDrawDebugTrace::None, hit, 1))
		{
			end = hit.Location;
			if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), end + FVector(0, 0, halfSize * 2 + 20), end - FVector(0, 0,5000), types, 0, arr, EDrawDebugTrace::None, hit, 1))
				end = hit.Location;
			else end = ch->GetActorLocation();
		}
		else if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), start + FVector(0, 0, halfSize * 2 + 20) + ch->GetActorForwardVector() * 50, start + ch->GetActorForwardVector() * 50 - FVector(0, 0, 5000), types, 0, arr, EDrawDebugTrace::None, hit, 1))
			end = hit.Location;
		end.Z += 5;

		InItem->SetOwnerCharacter(nullptr);

		CLog::Print(GetOwner()->GetName());

		Multicast_DropSequence(InItem, start, end);
	}
}

void AItemManager::Multicast_DropSequence_Implementation(AEqquipment* InItem, FVector Start, FVector End)
{
	CheckNull(InItem);
	InItem->PlayDropSequence(Start, End);
}

void AItemManager::SetItemLocation(AEqquipment* InItem, const FVector& NewLocation, bool bSweep, FHitResult* OutSweepHitResult, ETeleportType Teleport)
{
	CheckNull(InItem);
	FHitResult f;
	if (OutSweepHitResult == nullptr)
		OutSweepHitResult = &f;
	Server_SetItemLocation(InItem, NewLocation, bSweep, *OutSweepHitResult, Teleport);
}

void AItemManager::SetItemRotation(AEqquipment* InItem, FRotator NewRotation, ETeleportType Teleport)
{
	CheckNull(InItem);
	Server_SetItemRotation(InItem, NewRotation, Teleport);
}

void AItemManager::AttachItemToComponent(AEqquipment* InItem, USceneComponent* Parent, const FAttachmentTransformRules& AttachmentRules, FName SocketName)
{
	CheckNull(InItem);
	Server_AttachItemToComponent(
		InItem, 
		Parent, 
		AttachmentRules.LocationRule,
		AttachmentRules.RotationRule,
		AttachmentRules.ScaleRule,
		AttachmentRules.bWeldSimulatedBodies,
		SocketName);
}

void AItemManager::ChangeVisibility(AEqquipment* InItem, EItemMode NewMode)
{
	CheckNull(InItem);
	Server_ChangeVisibility(InItem, NewMode);
}

AEqquipment* AItemManager::SpawnItem(UClass* Class, FTransform const& Transform, const FActorSpawnParameters& SpawnParameters)
{
	AEqquipment* result = GetWorld()->SpawnActor<AEqquipment>(Class, Transform, SpawnParameters);
	if (result)result->SetManager(this);
	return result;
}

AEqquipment* AItemManager::SpawnItemDeferred(UClass* Class, FTransform const& Transform, AActor* NewOwner, APawn* NewInstigator, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride)
{
	AEqquipment* result = GetWorld()->SpawnActorDeferred<AEqquipment>(Class, Transform, NewOwner, NewInstigator, CollisionHandlingOverride);
	if (result)result->SetManager(this);
	return result;
}
