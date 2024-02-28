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

	if (HasAuthority())
	{
		APlayerController* con = UGameplayStatics::GetPlayerControllerFromID(GetWorld(), 0);
		SetOwner(con);
	}
	FTimerHandle WaitHandle;
	float WaitTime = 10;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
	{
		stest();
	}), WaitTime, false);

}

void AItemManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AItemManager::test_Implementation()
{
	CLog::Print("test_Implementation");
}
void AItemManager::stest_Implementation()
{
	test();
}
void AItemManager::Server_ChangeVisibility_Implementation(AWeapon* InItem, EItemMode NewMode)
{
	
	InItem->SetMode(NewMode);
}

void AItemManager::ChangeVisibility(AWeapon* InItem, EItemMode NewMode)
{
	Server_ChangeVisibility(InItem, NewMode);
}
