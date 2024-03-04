#include "Components/AppearanceComponent.h"
#include "Global.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

UAppearanceComponent::UAppearanceComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	Meshes.Init(nullptr, int32(EAppearancePart::Max));
	AppearanceAssets.Init(TArray<TSoftObjectPtr<USkeletalMesh>>(), int32(EAppearancePart::Max));
}

void UAppearanceComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UAppearanceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAppearanceComponent::Init(const TMap<EAppearancePart, USkeletalMeshComponent*>& InMeshes)
{
	for (auto i : InMeshes)
		Meshes[int32(i.Key)] = i.Value;

	if (DataTable)
	{
		TArray<FAppearanceDataTable*> datas;
		DataTable->GetAllRows<FAppearanceDataTable>("", datas);
		for (auto data : datas)
			AppearanceAssets[int32(data->MeshPart)].Add(data->Asset);
	}
}

void UAppearanceComponent::ChangeAppearance(EAppearancePart InMeshPart, int32 InIndex)
{
	int32 idx = int32(InMeshPart);
	CheckNull(Meshes[idx]);
	CheckFalse(AppearanceAssets[idx].IsValidIndex(InIndex));

	USkeletalMesh* mesh = nullptr;

	if (AppearanceAssets[idx][InIndex].IsPending())
	{
		const FSoftObjectPath& AssetRef = AppearanceAssets[idx][InIndex].ToSoftObjectPath();
		auto& assetLoader = UAssetManager::GetStreamableManager();
		mesh = Cast<USkeletalMesh>(assetLoader.LoadSynchronous(AssetRef));
	}
	else mesh = Cast<USkeletalMesh>(AppearanceAssets[idx][InIndex].Get());

	Meshes[idx]->SetSkeletalMesh(mesh);
}