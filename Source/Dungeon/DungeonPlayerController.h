#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "DungeonPlayerController.generated.h"

class UNiagaraSystem;
class APlayerCharacter;
class AEnemy;
class ANPC;
class AItemManager;
class AWeapon;
class UConfirmPopupComponent;
class IIInteractable;
class UUW_Main;
class UUW_Dialog;

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
	UPROPERTY(Replicated)int32 Index;
	bool bInputPressed;
	float FollowTime;

	AEnemy* Target;
	UPROPERTY(ReplicatedUsing = "OnRep_ItemManager")AItemManager* ItemManager;//Load::
	AWeapon* Item;
	IIInteractable* Iteractable;
protected:
	UPROPERTY(VisibleDefaultsOnly)
		UConfirmPopupComponent* ConfirmPopup;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		float ShortPressThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		UNiagaraSystem* FXCursor;

	UPROPERTY(EditDefaultsOnly, Category = "HUD")
		TSubclassOf<UUW_Main> MainWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Dialog")
		TSubclassOf<UUW_Dialog> DialogWidgetClass;

	uint32 bMoveToMouseCursor : 1;
	UPROPERTY()UUW_Main* MainWidget;
	UPROPERTY()UUW_Dialog* DialogWidget;
public:

	//function
private:
	UFUNCTION() void OnRep_ItemManager();

	UFUNCTION(Client, Reliable)void Client_CreateMainWidget();

	UFUNCTION(Reliable, Server)void Server_ReplicateRotation(FRotator NewRotation, ADungeonPlayerController* Exception);
	UFUNCTION(NetMulticast, Reliable)void Multicast_ReplicateRotation(FRotator NewRotation, ADungeonPlayerController* Exception);
	UFUNCTION(Client, Reliable)void Client_ReplicateRotation(FRotator NewRotation);

	UFUNCTION(Reliable, Server)void Server_Interaction(AActor* InInteractable);
	UFUNCTION(Reliable, Server)void Server_SelectReply(AActor* InInteractable, int32 NextPoint);
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
	UFUNCTION(Client, Reliable)void Client_DialogInit(ANPC* InNPC);
	UFUNCTION(Client, Reliable)void Client_DialogSpeak(const FText& InText);
	UFUNCTION(Client, Reliable)void Client_DialogReply(const TArray<FText>& InReplies);
	UFUNCTION(Client, Reliable)void Client_DialogExit();

	// input
	void SetUIOnlyMode();
	void SetGameAndUIMode();

	// widget control
	void RevealMainWidget();
	void HideWidget();
	void HideAddtiveWidget();

	// etc.
	void StopPawnImmediately();

	// setter
	FORCEINLINE void SetIndex(int32 InIndex) { Index = InIndex; }

	// getter
	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE int32 GetIndex() const { return Index; }
	FORCEINLINE UUW_Main* GetMainWidget() const { return MainWidget; }
	FORCEINLINE AItemManager* GetItemManager() const { return ItemManager; }
};


