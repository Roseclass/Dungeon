#include "Components/QuestListComponent.h"
#include "Global.h"
#include "Particles/ParticleSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Characters/NPC.h"

UQuestListComponent::UQuestListComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UQuestListComponent::BeginPlay()
{
	Super::BeginPlay();

	//������ �о����
	{
		TArray<FQuestTreeData*> datas;
		DataTable->GetAllRows<FQuestTreeData>("", datas);
		for (auto i : datas)Quests.Add(*i); 
	}

	USkeletalMeshComponent* mesh = CHelpers::GetComponent<USkeletalMeshComponent>(GetOwner());
	if (!mesh)return;

	//������ �޽� ��ġ�� ����Ʈ ����ġ �Ŀ� �Ⱥ��̴� ���·� ����

	UNiagaraSystem* n = Cast<UNiagaraSystem>(RewardSign);
	UParticleSystem* p = Cast<UParticleSystem>(RewardSign);

	FVector loc = mesh->GetSocketLocation(SlotName);
	loc += AdditiveTransform.GetTranslation();

	FRotator rot = mesh->GetSocketRotation(SlotName);
	rot += AdditiveTransform.Rotator();

	if (n)
	{
		NSign = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), n, loc, rot, AdditiveTransform.GetScale3D());
		NSign->SetVisibility(0);
	}
	else if (p)
	{
		PSign = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), p, loc, rot, AdditiveTransform.GetScale3D());
		PSign->SetVisibility(0);
	}
}

void UQuestListComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UQuestListComponent::FindAvailableQuest(TArray<FQuestTreeData>& InArray, int32 InLevel)
{
	for (auto i : Quests)
	{
		if (i.QuestLevel > InLevel)continue;
		InArray.Add(i);
	}
	return InArray.Num() > 0;
}