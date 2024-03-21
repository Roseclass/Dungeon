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

DECLARE_MULTICAST_DELEGATE(FDialogWidget_OnExit);
DECLARE_MULTICAST_DELEGATE_OneParam(FDialogWidget_OnSpeakFinished, UBehaviorTreeComponent*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FDialogWidget_OnReplyFinished, int32, UBehaviorTreeComponent*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FDialogWidget_OnQuestReplyFinished, UBehaviorTreeComponent*, UDialogReplyObject*);
DECLARE_MULTICAST_DELEGATE_OneParam(FDialogWidget_OnPointFinished, UBehaviorTreeComponent*);

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
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

		//property
private:
	EDialogState DialogState;
	UBehaviorTreeComponent* BTComponent;
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
	FDialogWidget_OnPointFinished OnPointReplyFinished;
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
	void SetBTComponent(UBehaviorTreeComponent* InComponent);
	void SetDialogState(EDialogState InState);
	void Speak(FText InText);
	void Reply(TArray<FText> Replies);
	void Quest(const TArray<FQuestTreeData>& Quests);
	void Point(FText InText, bool Clear = 0);
	void Exit();

};
