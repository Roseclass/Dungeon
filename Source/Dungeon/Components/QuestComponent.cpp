#include "Components/QuestComponent.h"
#include "Global.h"
#include "GameFramework/Character.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "DungeonPlayerController.h"
#include "Characters/PlayerCharacter.h"
#include "Objects/Quest.h"

#include "Widgets/UW_Quest.h"

UQuestComponent::UQuestComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(1);
}

void UQuestComponent::BeginPlay()
{
	Super::BeginPlay();

	ACharacter* character = Cast<ACharacter>(GetOwner());
	ADungeonPlayerController* controller = Cast<ADungeonPlayerController>(character->GetController());
	CheckNull(controller);

	if (character->IsLocallyControlled())
	{
		if (QuestWidgetClass)
		{
			QuestWidget = CreateWidget<UUW_Quest, APlayerController>(controller, QuestWidgetClass);
			QuestWidget->SetVisibility(ESlateVisibility::Collapsed);
			QuestWidget->AddToViewport();
		}else CLog::Print("QuestWidgetClass is null");
	}
}

void UQuestComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UQuestComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicated ������ ���⿡ �߰�
	DOREPLIFETIME_CONDITION(UQuestComponent, Quest, COND_None);
}

void UQuestComponent::OnRep_Quest()
{
	SetPreview(Quest);
}

void UQuestComponent::SetQuest(AQuest* InQuest)
{
	Quest = InQuest;
	QuestWidget->SetQuest(InQuest);
}

void UQuestComponent::RemoveQuest(AQuest* InQuest)
{
	CheckFalse(Quest == InQuest);
	Quest->Destroy();
	Quest = nullptr;
}

void UQuestComponent::ToggleWidget()
{
	CheckFalse(bCanToggle);
	if (!QuestWidget) { CLog::Print("UQuestComponent::ToggleWidget, QuestWidget Is Nullptr"); return; }
	QuestWidget->IsVisible() ? OffWidget() : OnWidget();
}

void UQuestComponent::OnWidget()
{
	if (!QuestWidget) { CLog::Print("UQuestComponent::OnWidget, QuestWidget Is Nullptr"); return; }
	if (QuestWidget->IsVisible())return;

	QuestWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UQuestComponent::OffWidget()
{
	if (!QuestWidget) { CLog::Print("UQuestComponent::OffWidget, QuestWidget Is Nullptr"); return; }

	QuestWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void UQuestComponent::SetCanToggle()
{
	bCanToggle = 1;
}

void UQuestComponent::SetCannotToggle()
{
	bCanToggle = 0;
}

void UQuestComponent::SetPreview(AQuest* InQuest)
{
	if (!InQuest)
	{
		CLog::Print("nullptr");
		return;
	}
	QuestWidget->SetQuest(InQuest);
}

void UQuestComponent::CheckCondition_Implementation(AActor* InObject)
{
	// this function call when enemy die, interact with object...
	Quest->CheckCondition(InObject);
}

void UQuestComponent::AddToQuestPool(AActor* InObject)
{
	Quest->AddToQuestPool(InObject);
}
