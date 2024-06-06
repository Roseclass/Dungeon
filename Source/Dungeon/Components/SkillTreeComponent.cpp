#include "Components/SkillTreeComponent.h"
#include "Global.h"

#include "SaveManager.h"
#include "Characters/PlayerCharacter.h"
#include "DungeonPlayerController.h"
#include "Components/SkillComponent.h"
#include "Widgets/UW_SkillTree.h"

USkillTreeComponent::USkillTreeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USkillTreeComponent::BeginPlay()
{
	Super::BeginPlay();

}

void USkillTreeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USkillTreeComponent::Init(const TArray<const FSkillData*> Array, USkillComponent* InSkillComp, TFunction<void(int32, int32)> OnPopupClicked)
{
	for (auto i : Array)
	{
		if (i->ParentPosition == FVector2D(-1, -1))
		{
			RootDatas.Add(i->PannelPosition);
			continue;
		}
		TreeDatas.FindOrAdd(i->ParentPosition).Add(i->PannelPosition);
	}

	if (WidgetClass && !Widget)
	{
		APlayerCharacter* character = Cast<APlayerCharacter>(GetOwner());
		CheckNull(character);
		ADungeonPlayerController* controller = Cast<ADungeonPlayerController>(character->GetController());
		CheckNull(controller);

		Widget = CreateWidget<UUW_SkillTree, ADungeonPlayerController>(controller, WidgetClass);
		Widget->AddToViewport();
		Widget->Init(Array, this, InSkillComp, OnPopupClicked);
	}
}

void USkillTreeComponent::AddPoints()
{

}

//void USkillTreeComponent::Acquire(ASkillActor* SkillActor)
//{
//	// TODO::add point condition
//	//	태그를 통해 관리
//
//	SkillActor->Server_SetAcquired();
//}

bool USkillTreeComponent::IsWidgetVisible()
{
	if (!Widget)return 0;
	return Widget->IsVisible();
}

void USkillTreeComponent::OnWidget()
{
	CheckNull(Widget);
	Widget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void USkillTreeComponent::OffWidget()
{
	CheckNull(Widget);
	Widget->SetVisibility(ESlateVisibility::Collapsed);
}

void USkillTreeComponent::SaveData(USaveGameData* SaveData)
{
	// reset Datas
	//SaveData->PlayerData.SkillPoints;
	//SaveData->PlayerData.AcquiredSkills.Empty();

	//for (auto i : RootActors)
	//{
	//	if (i->GetSkillTreeState() != ESkillTreeSkillState::Acquired)continue;
	//	SaveData->PlayerData.AcquiredSkills.Add(i->GetClass());
	//	TQueue<ASkillActor*> q;
	//	q.Enqueue(i);
	//	while (!q.IsEmpty())
	//	{
	//		ASkillActor* cur;q.Dequeue(cur);
	//		for (auto s : cur->GetChildren())
	//		{
	//			if (s->GetSkillTreeState() != ESkillTreeSkillState::Acquired)continue;
	//			SaveData->PlayerData.AcquiredSkills.Add(s->GetClass());
	//			q.Enqueue(s);
	//		}
	//	}
	//}
}

void USkillTreeComponent::LoadData(USaveGameData* const ReadData)
{
	ReadData->PlayerData.SkillPoints;

	//// refresh widget
	//for (auto i : RootActors)
	//{
	//	TQueue<ASkillActor*> q;
	//	q.Enqueue(i);
	//	while (!q.IsEmpty())
	//	{
	//		ASkillActor* cur; q.Dequeue(cur);
	//		cur->RefreshWidget();
	//		for (auto s : cur->GetChildren())
	//			q.Enqueue(s);
	//	}
	//}

	//// load state
	//for (auto i : RootActors)
	//{
	//	if (ReadData->PlayerData.AcquiredSkills.Find(i->GetClass()) == INDEX_NONE)continue;
	//	TQueue<ASkillActor*> q;
	//	q.Enqueue(i); i->Server_SetAcquired();
	//	while (!q.IsEmpty())
	//	{
	//		ASkillActor* cur; q.Dequeue(cur);
	//		for (auto s : cur->GetChildren())
	//		{
	//			if (ReadData->PlayerData.AcquiredSkills.Find(s->GetClass()) == INDEX_NONE)continue;
	//			q.Enqueue(s); s->Server_SetAcquired();
	//		}
	//	}
	//}
}
