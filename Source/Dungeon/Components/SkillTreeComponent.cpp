#include "Components/SkillTreeComponent.h"
#include "Global.h"

#include "SaveManager.h"
#include "Characters/PlayerCharacter.h"
#include "DungeonPlayerController.h"
#include "Objects/SkillActor.h"
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

void USkillTreeComponent::Init(const TArray<ASkillActor*>& Array, TFunction<void(int32, ASkillActor*)> OnPopupClicked)
{
	for (auto i : Array)
	{
		if (!i->GetParent())
			RootActors.Add(i);
	}

	if (WidgetClass)
	{
		APlayerCharacter* character = Cast<APlayerCharacter>(GetOwner());
		CheckNull(character);
		ADungeonPlayerController* controller = Cast<ADungeonPlayerController>(character->GetController());
		CheckNull(controller);

		Widget = CreateWidget<UUW_SkillTree, ADungeonPlayerController>(controller, WidgetClass);
		Widget->AddToViewport();
		Widget->Init(Array, this, OnPopupClicked);
	}
}

void USkillTreeComponent::AddPoints()
{

}

void USkillTreeComponent::Acquire(ASkillActor* SkillActor)
{
	// TODO::add point condition

	SkillActor->Server_SetAcquired();
}

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
	SaveData->PlayerData.SkillPoints;

	for (auto i : RootActors)
	{
		if (i->GetSkillTreeState() != ESkillTreeSkillState::Acquired)continue;
		SaveData->PlayerData.AcquiredSkills.Add(i->StaticClass());
		TQueue<ASkillActor*> q;
		q.Enqueue(i);
		while (!q.IsEmpty())
		{
			ASkillActor* cur;q.Dequeue(cur);
			for (auto s : cur->GetChildren())
			{
				if (s->GetSkillTreeState() != ESkillTreeSkillState::Acquired)continue;
				SaveData->PlayerData.AcquiredSkills.Add(s->StaticClass());
				q.Enqueue(s);
			}
		}
	}
}

void USkillTreeComponent::LoadData(USaveGameData* const ReadData)
{
	ReadData->PlayerData.SkillPoints;

	for (auto i : RootActors)
	{
		if (ReadData->PlayerData.AcquiredSkills.Find(i->StaticClass()) == INDEX_NONE)continue;
		TQueue<ASkillActor*> q;
		q.Enqueue(i); i->Server_SetAcquired();
		while (!q.IsEmpty())
		{
			ASkillActor* cur; q.Dequeue(cur);
			for (auto s : cur->GetChildren())
			{
				if (ReadData->PlayerData.AcquiredSkills.Find(s->StaticClass()) == INDEX_NONE)continue;
				q.Enqueue(s); s->Server_SetAcquired();
			}
		}
	}
}
