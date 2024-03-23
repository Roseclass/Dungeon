#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/QuestListComponent.h"
#include "UW_Dialog.generated.h"

/**
 * 
 */

class UListView;
class UBorder;
class UImage;
class UTextBlock;
class UDialogReplyObject;
class UBehaviorTreeComponent;
class ADungeonPlayerController;
class ANPC;

DECLARE_MULTICAST_DELEGATE(FDialogWidget_OnExit);
DECLARE_MULTICAST_DELEGATE_OneParam(FDialogWidget_OnSpeakFinished, ANPC*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FDialogWidget_OnReplyFinished, ANPC*, int32);
DECLARE_MULTICAST_DELEGATE_TwoParams(FDialogWidget_OnQuestReplyFinished, UBehaviorTreeComponent*, UDialogReplyObject*);

UENUM(BlueprintType)
enum class EDialogState : uint8
{
	Speak, Reply, Max
};

UCLASS()
class DUNGEON_API UUW_Dialog : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct()override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

		//property
private:
	ADungeonPlayerController* PlayerController;
	EDialogState DialogState;
	UBehaviorTreeComponent* BTComponent;
	ANPC* InteractingNPC;
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UBorder* ReplyBorder;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UListView* RepliesListView;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* NPCPortrait;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* NPCName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UBorder* SpeakBorder;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* SpeakText;

public:
	FDialogWidget_OnExit OnExit;
	FDialogWidget_OnSpeakFinished OnSpeakFinished;
	FDialogWidget_OnReplyFinished OnReplyFinished;
	FDialogWidget_OnQuestReplyFinished OnQuestReplyFinished;
	//function
private:
	UFUNCTION() void OnReplyClicked(UDialogReplyObject* ClickedObject);
	UFUNCTION() void OnQuestClicked(UDialogReplyObject* ClickedObject);
	UFUNCTION() void OnPointClicked(UDialogReplyObject* ClickedObject);
	void HideReplySection();
	void RevealReplySection();
	void HideSpeakSection();
	void RevealSpeakSection();
protected:
public:
	void Init(UObject* Portrait,FText Name);
	void SetBTComponent(UBehaviorTreeComponent* NewBTComponent);
	void SetInteractingNPC(ANPC* NewInteractingNPC);
	void SetDialogState(EDialogState InState);
	void Speak(FText InText);
	void Reply(TArray<FText> Replies);
	void Quest(const TArray<FQuestTreeData>& Quests);
	void Exit();

};
