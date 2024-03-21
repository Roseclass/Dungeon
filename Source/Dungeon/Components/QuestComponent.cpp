#include "Components/QuestComponent.h"
#include "Global.h"
#include "GameFramework/Character.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "DungeonPlayerController.h"

#include "Objects/Quest.h"

UQuestComponent::UQuestComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UQuestComponent::BeginPlay()
{
	Super::BeginPlay();

	ACharacter* character = Cast<ACharacter>(GetOwner());
	ADungeonPlayerController* controller = Cast<ADungeonPlayerController>(character->GetController());
	CheckNull(controller);

	//QuestWidget = Cast<UCUserWidget_Quest>(controller->GetQuestWidget());
	//if (!QuestWidget)CLog::Print("UQuestComponent::BeginPlay(), Widget Cast Fail");

	//if (QuestWidgetClass)
	//{
	//	QuestWidget = CreateWidget<UCUserWidget_Quest, APlayerController>(controller, QuestWidgetClass);
	//	QuestWidget->SetVisibility(ESlateVisibility::Collapsed);
	//	QuestWidget->AddToViewport();
	//}

}

void UQuestComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UQuestComponent::Reset()
{
	for (auto i : Quests)
	{
		i->OnDestroyed.Clear();
		i->Destroy();
	}
	Quests.Empty();
}

void UQuestComponent::AddQuest(AQuest* InQuest)
{
	Quests.AddUnique(InQuest);
}

void UQuestComponent::RemoveQuest(AQuest* InQuest)
{
	Quests.Remove(InQuest);
	InQuest->Destroy();
}

AQuest* UQuestComponent::FindQuest(TSubclassOf<AQuest> InQuestClass)
{
	for (auto i : Quests)
	{
		if (i->GetClass() == InQuestClass)return i;
	}
	return nullptr;
}

void UQuestComponent::ToggleWidget()
{
	//if (!QuestWidget) { CLog::Print("UQuestComponent::ToggleWidget, QuestWidget Is Nullptr"); return; }
	//QuestWidget->IsVisible() ? OffWidget() : OnWidget();
}

void UQuestComponent::OnWidget()
{
	//if (!QuestWidget) { CLog::Print("UQuestComponent::OnWidget, QuestWidget Is Nullptr"); return; }
	//if (QuestWidget->IsVisible())return;
	//ACharacter* character = Cast<ACharacter>(GetOwner());
	//APlayerController* controller = Cast<APlayerController>(character->GetController());
	//QuestWidget->On(Quests);
	//QuestWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	//if (controller)
	//{
	//	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(controller, QuestWidget, EMouseLockMode::DoNotLock, 0);
	//	controller->SetShowMouseCursor(1);
	//}
}

void UQuestComponent::OffWidget()
{
	//if (!QuestWidget) { CLog::Print("UQuestComponent::OffWidget, QuestWidget Is Nullptr"); return; }
	//if (!QuestWidget->IsVisible())return;
	//ACharacter* character = Cast<ACharacter>(GetOwner());
	//APlayerController* controller = Cast<APlayerController>(character->GetController());
	//QuestWidget->SetVisibility(ESlateVisibility::Collapsed);
	//QuestWidget->Off();
	//if (controller)
	//{
	//	UWidgetBlueprintLibrary::SetInputMode_GameOnly(controller);
	//	controller->SetShowMouseCursor(0);
	//}
}

void UQuestComponent::GetQuestArray(TArray<AQuest*>& InArray)
{
	for (auto i : Quests)InArray.Add(i);
}

AQuest* UQuestComponent::GetCompletedQuest()
{
	//for (auto i : Quests)
	//{
	//	if (!i->GetRootObjectCollection()->GetIsCollectionCompleted())continue;
	//	return i;
	//}
	return nullptr;
}

void UQuestComponent::FindInteractQuest(AActor* InActor)
{
	CheckNull(InActor);

	//for (auto i : Quests)
	//{
	//	if (!i)continue;
	//	UCQuest_ObjectiveCollection* col = i->GetRootObjectCollection();
	//	if (!col)continue;
	//	UCQuest_Objective* obj = col->FindNextIncompleteObject();
	//	if (!obj)continue;
	//	UCQuest_ObjectiveInteract* inter = Cast<UCQuest_ObjectiveInteract>(obj);
	//	if (!inter)continue;
	//	inter->CheckTargetActor(InActor);
	//}
}