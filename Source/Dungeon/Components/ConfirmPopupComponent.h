#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ConfirmPopupComponent.generated.h"

class ADungeonPlayerController;
class UUW_ConfirmPopup;

UENUM()
enum class EPopupState : uint8
{
	Confirm, Cancel, Wait
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEON_API UConfirmPopupComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UConfirmPopupComponent();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//property
private:
	TFunction<bool()> ReserveFinishedEvent;
	UPROPERTY()UUW_ConfirmPopup* PopupWidget;
	TMap<ADungeonPlayerController*, EPopupState>States;
protected:
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UUW_ConfirmPopup> PopupClass;
public:

	//function
private:
	UFUNCTION(Reliable, Server)void Server_Confirm();
	UFUNCTION(Reliable, Server)void Server_Cancel();
protected:
public:
	UFUNCTION(Client, Reliable)void Client_CreatePopup(const FString& InString, const TArray<AActor*>& NewPortraitActors, float NewTimeLimit = 10);
	UFUNCTION(Client, Reliable)void Client_UpdateSign(AActor* PortraitActor, bool NewState);
	UFUNCTION(Client, Reliable)void Client_ConfirmedSequence();

	void SendPopupAllPlayers(FString InString, TFunction<bool()> FinishedEvent, float NewTimeLimit = 10);
	void UpdateState(ADungeonPlayerController* InPlayer, EPopupState NewState);
	void UpdateSign(ADungeonPlayerController* InPlayer, EPopupState NewState);
};
