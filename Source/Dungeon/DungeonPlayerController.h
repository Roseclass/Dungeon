#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "DungeonPlayerController.generated.h"

class UNiagaraSystem;
class APlayerCharacter;
class AEnemy;
class AItemManager;
class AWeapon;
class IIInteractable;
class UUW_Main;

UCLASS()
class ADungeonPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ADungeonPlayerController();
protected:
	virtual void BeginPlay() override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetupInputComponent() override;
	virtual void PostNetInit() override;
	virtual void OnPossess(APawn* InPawn)override;
public:

	//property
private:
	bool bInputPressed;
	float FollowTime;

	AEnemy* Target;
	UPROPERTY(ReplicatedUsing = "OnRep_ItemManager")AItemManager* ItemManager;//Load::
	AWeapon* Item;
	IIInteractable* Iteractable;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		float ShortPressThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		UNiagaraSystem* FXCursor;

	UPROPERTY(EditDefaultsOnly, Category = "HUD")
		TSubclassOf<UUW_Main> MainWidgetClass;

	uint32 bMoveToMouseCursor : 1;
	UPROPERTY()UUW_Main* MainWidget;
public:

	//function
private:
	UFUNCTION() void OnRep_ItemManager();

	UFUNCTION(Client, Reliable)void Client_CreateMainWidget();

	UFUNCTION(Reliable, Server)void Server_ReplicateRotation(FRotator NewRotation, ADungeonPlayerController* Exception);
	UFUNCTION(NetMulticast, Reliable)void Multicast_ReplicateRotation(FRotator NewRotation, ADungeonPlayerController* Exception);
	UFUNCTION(Client, Reliable)void Client_ReplicateRotation(FRotator NewRotation);

protected:
	//skill input
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();
	void OnRightClick();
	void OnQuickSlot0();
	void OnQuickSlot1();
	void OnQuickSlot2();
	void OnQuickSlot3();
	//widget input
	void OnSkillTree();
	void OnInventory();
public:

	FORCEINLINE UUW_Main* GetMainWidget() const { return MainWidget; }
	FORCEINLINE AItemManager* GetItemManager() const { return ItemManager; }
};


