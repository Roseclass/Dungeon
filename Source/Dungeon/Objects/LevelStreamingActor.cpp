#include "Objects/LevelStreamingActor.h"
#include "Global.h"
#include "Components/SceneComponent.h"
#include "Components/MeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "SaveManager.h"
#include "DungeonPlayerController.h"
#include "Characters/PlayerCharacter.h"
#include "Components/QuestComponent.h"
#include "Components/ConfirmPopupComponent.h"
#include "Components/TravelEffectComponent.h"

#include "Objects/Quest.h"
#include "Objects/StageScriptActor.h"

ALevelStreamingActor::ALevelStreamingActor()
{
	PrimaryActorTick.bCanEverTick = true;
	CHelpers::CreateComponent(this, &InteractCollision, "InteractCollision");
}

void ALevelStreamingActor::BeginPlay()
{
	Super::BeginPlay();
	QuestObjective_Init();

	if (DataTable)
	{
		TArray<FStageData*> datas;
		DataTable->GetAllRows<FStageData>("", datas);
		for (auto data : datas)
		{
			Datas.Add(FStageData());
			Datas[Datas.Num() - 1].StageList = data->StageList;
			Datas[Datas.Num() - 1].StageName = data->StageName;
			Datas[Datas.Num() - 1].StageDescription = data->StageDescription;
			Datas[Datas.Num() - 1].StartLocations = data->StartLocations;
		}
	}

	// find mesh
	GetComponents<UMeshComponent>(MeshComponents);
}

void ALevelStreamingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALevelStreamingActor::StartInteract(ADungeonPlayerController* InPlayer)
{
	CheckFalse(bActive);

	// if host?
	if (HasAuthority() && InPlayer->IsLocalController())
	{
		SetOwner(InPlayer);

		switch (Type)
		{
		case EStreamingType::Fixed:Interact_Fixed(InPlayer);break;
		case EStreamingType::Quest:Interact_Quest(InPlayer);break;
		default:break;
		}
	}

}

void ALevelStreamingActor::EndInteract(ADungeonPlayerController* InPlayer)
{
}

void ALevelStreamingActor::StartCursorOver(ADungeonPlayerController* InPlayer)
{
	for (auto i : MeshComponents)
		i->SetRenderCustomDepth(1);
}

void ALevelStreamingActor::EndCursorOver(ADungeonPlayerController* InPlayer)
{
	for (auto i : MeshComponents)
		i->SetRenderCustomDepth(0);
}

bool ALevelStreamingActor::IsInteractable() 
{
	return bActive;
}

void ALevelStreamingActor::CompleteCondition()
{
	LinkedComponent->CheckCondition(this);
}

void ALevelStreamingActor::Interact_Fixed(ADungeonPlayerController* InPlayer)
{
	// send popup every clients
	UConfirmPopupComponent* confirm = CHelpers::GetComponent<UConfirmPopupComponent>(InPlayer);

	TFunction<bool()> func = [InPlayer, this]()->bool
	{
		UQuestComponent* quest = CHelpers::GetComponent<UQuestComponent>(InPlayer->GetPawn());
		if (!quest)return 0;
		CompleteCondition();
		for (auto i : Datas)
		{
			if (NextStage == i.StageList)LoadLevel(i);
			break;
		}
		return 1;
	};

	if (confirm)confirm->SendPopupAllPlayers(PopupMessage, func);
	else CLog::Print("nullptr");
}

void ALevelStreamingActor::Interact_Quest(ADungeonPlayerController* InPlayer)
{
	// send popup every clients
	UConfirmPopupComponent* confirm = CHelpers::GetComponent<UConfirmPopupComponent>(InPlayer);

	TFunction<bool()> func = [InPlayer, this]()->bool
	{
		UQuestComponent* quest = CHelpers::GetComponent<UQuestComponent>(InPlayer->GetPawn());
		if (!quest)return 0;
		CompleteCondition();
		LoadLevel(quest->GetQuest()->GetStreamingLevelName());
		return 1;
	};

	UQuestComponent* quest = CHelpers::GetComponent<UQuestComponent>(InPlayer->GetPawn());
	if (!quest)return;

	if (confirm)confirm->SendPopupAllPlayers(quest->GetQuest()->GetPopupMessage(), func);
	else CLog::Print("nullptr");

}

void ALevelStreamingActor::LoadLevel(FStageData InData)
{
	FLatentActionInfo temp;
	temp.CallbackTarget = this;
	temp.ExecutionFunction = FName("LoadLevelFinished");
	temp.Linkage = 0;
	UGameplayStatics::LoadStreamLevel(GetWorld(), InData.StageName, 1, 0, temp);

	CurrentStage = InData.StageList;

	LoadLevelStarted();
}

void ALevelStreamingActor::LoadLevel(FName InLevelName)
{
	FLatentActionInfo temp;
	temp.CallbackTarget = this;
	temp.ExecutionFunction = FName("LoadLevelFinished");
	temp.Linkage = 0;
	UGameplayStatics::LoadStreamLevel(GetWorld(), InLevelName, 1, 0, temp);

	for(auto& i : Datas)
		if (i.StageName == InLevelName)
		{
			CurrentStage = i.StageList;
			break;
		}

	LoadLevelStarted();
}

void ALevelStreamingActor::LoadLevelStarted()
{
	// this is server

	// get all player characters
	TArray<AActor*> characters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), characters);

	// set values
	SequenceMap.Empty();
	for (auto i : characters)
	{
		APlayerCharacter* ch = Cast<APlayerCharacter>(i);
		if (!ch)continue;
		SequenceMap.Add(ch, 0);

		UTravelEffectComponent* travel = CHelpers::GetComponent<UTravelEffectComponent>(ch);
		if(!travel)continue;
		travel->OnStartSequenceFinishedCheck.AddDynamic(this, &ALevelStreamingActor::ForwardSequenceFinished);
	}

	// play forward sequence
	Multicast_ForwardSequence();
}

void ALevelStreamingActor::LoadLevelFinished()
{
	// this is server

	bLoad = 1;

	// play reverse sequence
	StartReverseSequence();
}

void ALevelStreamingActor::Multicast_ForwardSequence_Implementation()
{
	APlayerCharacter* pawn = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerControllerFromID(GetWorld(), 0)->GetPawn());

	if (!pawn)
	{
		CLog::Print("ALevelStreamingActor::Multicast_ForwardSequence pawn is nullptr", -1, 10, FColor::Red);
		return;
	}

	TScriptInterface<IISave> ptr = pawn;
	USaveManager::SaveActor(ptr);
	USaveManager::ActivateSlot();

	UTravelEffectComponent* travel = CHelpers::GetComponent<UTravelEffectComponent>(pawn);
	if (travel)
		travel->Play();
}

void ALevelStreamingActor::Multicast_ReverseSequence_Implementation(EStageList InStage)
{
	AStageScriptActor* mainStage = Cast<AStageScriptActor>(UGameplayStatics::GetActorOfClass(GetWorld(), AStageScriptActor::StaticClass()));
	if (mainStage)
	{
		if (InStage == EStageList::Main)mainStage->On();
		else mainStage->Off();
	}
	else CLog::Print("ALevelStreamingActor::Multicast_ReverseSequence NO Stage Actor", -1, 10, FColor::Red);

	APawn* pawn = UGameplayStatics::GetPlayerControllerFromID(GetWorld(), 0)->GetPawn();

	UTravelEffectComponent* travel = CHelpers::GetComponent<UTravelEffectComponent>(pawn);
	if (travel)
		travel->Reverse();
}

void ALevelStreamingActor::ForwardSequenceFinished(APlayerCharacter* InPlayerCharacter)
{
	SequenceMap.FindOrAdd(InPlayerCharacter) = 1;
	StartReverseSequence();
}

void ALevelStreamingActor::StartReverseSequence()
{
	// is load?
	CheckFalse(bLoad);

	// check sequencemap
	for (auto i : SequenceMap)
		CheckFalse(i.Value);

	// set values
	int32 idx = 0;
	for (auto i : SequenceMap)
	{
		while (1)
			if (!GetCurrentData().StartLocations.IsValidIndex(idx))
			{
				CLog::Print("ALevelStreamingActor::StartReverseSequence " + FString::FromInt(idx) + "is not valid idx");
				if (!idx)return;
				--idx;
				continue;
			}
			else break;
		FVector loc = GetCurrentData().StartLocations[idx++];
		i.Key->SetActorLocation(loc);
	}

	// reeset values
	bLoad = 0;
	SequenceMap.Empty();

	FTimerHandle WaitHandle;
	float WaitTime = 0.5;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
	{
		Multicast_ReverseSequence(CurrentStage);
	}), WaitTime, false);

	if (CurrentStage == EStageList::Main)
	{
		FTimerHandle UnloadHandle;
		float UnloadTime = 1;
		GetWorld()->GetTimerManager().SetTimer(UnloadHandle, FTimerDelegate::CreateLambda([&]()
		{
			for (auto& i : Datas)
				if (i.StageList == LocatedStage)
				{
					FLatentActionInfo LatentInfo;
					UGameplayStatics::UnloadStreamLevel(this, i.StageName, LatentInfo, 0);
					break;
				}
		}), UnloadTime, false);
	}
}

void ALevelStreamingActor::Activate_Implementation()
{
	// execute when accept quest or triggered
	bActive = 1;

	// sequence is implemented in the blueprint
}

void ALevelStreamingActor::Deactivate_Implementation()
{
	// execute when quest end or triggered
	bActive = 0;

	// sequence is implemented in the blueprint
}

FStageData ALevelStreamingActor::GetCurrentData()const
{
	for (auto& i : Datas)
		if (i.StageList == CurrentStage)
			return i;
	return FStageData();
}
