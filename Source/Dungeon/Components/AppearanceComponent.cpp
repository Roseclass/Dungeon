#include "Components/AppearanceComponent.h"
#include "Global.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

#include "SaveManager.h"

UAppearanceComponent::UAppearanceComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	Meshes.Init(nullptr, int32(EAppearancePart::Max));
	MeshIndices.Init(0, int32(EAppearancePart::Max));
	AppearanceAssets.Init(TArray<TSoftObjectPtr<USkeletalMesh>>(), int32(EAppearancePart::Max));
	AppearanceColors.Init(TArray<FAppearancePartColor>(), int32(EAppearancePart::Max));
}

void UAppearanceComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UAppearanceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAppearanceComponent::ChangeColor(EAppearancePart InMeshPart, FName Parameter, FLinearColor NewColor)
{
	int32 idx = int32(InMeshPart);
	CheckNull(Meshes[idx]);

	UMaterialInstanceDynamic* mat = Cast<UMaterialInstanceDynamic>(Meshes[idx]->GetMaterial(0));
	if (!mat)
	{
		Meshes[idx]->CreateDynamicMaterialInstance(0);
		mat = Cast<UMaterialInstanceDynamic>(Meshes[idx]->GetMaterial(0));
		CheckNull(mat);
	}
	mat->SetVectorParameterValue(Parameter, NewColor);
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
	MeshIndices[idx] = InIndex;
	CheckFalse(AppearanceColors[idx].IsValidIndex(InIndex));
	for (auto i : AppearanceColors[idx])
		ChangeColor(InMeshPart, i.Parameter, i.Color);
}

void UAppearanceComponent::ChangeColorData(EAppearancePart InMeshPart,FName Parameter, FLinearColor NewColor)
{
	int32 idx = int32(InMeshPart);
	CheckFalse(AppearanceColors.IsValidIndex(idx));
	ChangeColor(InMeshPart, Parameter, NewColor);
	for (auto i : AppearanceColors[idx])
	{
		if (i.Parameter == Parameter)
		{
			i.Color = NewColor;
			return;
		}
	}
	FAppearancePartColor f;
	f.Parameter = Parameter;
	f.Color = NewColor;
	AppearanceColors[idx].Add(f);
}

void UAppearanceComponent::SaveData(USaveGameData* SaveData)
{
	SaveData->PlayerData.AppearanceColors.Init(TArray<FAppearancePartColor>(),AppearanceColors.Num());
	for (int32 i = 0; i < AppearanceColors.Num(); ++i)
		for (auto f : AppearanceColors[i])
			SaveData->PlayerData.AppearanceColors[i].Add(f);

	SaveData->PlayerData.MeshIndices.Init(0, MeshIndices.Num());
	for (int32 i = 0; i < MeshIndices.Num(); ++i)
		SaveData->PlayerData.MeshIndices[i] = MeshIndices[i];

	CLog::Print(SaveData->PlayerData.AppearanceColors.Num());
	CLog::Print(SaveData->PlayerData.MeshIndices.Num());
}

void UAppearanceComponent::LoadData(USaveGameData* const ReadData)
{
	AppearanceColors = ReadData->PlayerData.AppearanceColors;
	for (int32 i = 0; i < AppearanceColors.Num(); ++i)
		for(auto f : AppearanceColors[i])
			ChangeColorData(EAppearancePart(i), f.Parameter,f.Color);

	MeshIndices = ReadData->PlayerData.MeshIndices;
	for (int32 i = 0; i < MeshIndices.Num(); ++i)
		ChangeAppearance(EAppearancePart(i), MeshIndices[i]);
}
