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
	CheckFalse(AppearanceColors.Contains(InMeshPart));
	for (auto i : AppearanceColors[InMeshPart].VectorParams)
		ChangeColor(InMeshPart, i.Key, i.Value);
}

void UAppearanceComponent::ChangeColorData(EAppearancePart InMeshPart,FName Parameter, FLinearColor NewColor)
{
	if (!AppearanceColors.Contains(InMeshPart))
		AppearanceColors.Add(InMeshPart);
	ChangeColor(InMeshPart, Parameter, NewColor);
	
	if (!AppearanceColors[InMeshPart].VectorParams.Contains(Parameter))
		AppearanceColors[InMeshPart].VectorParams.Add(Parameter);

	AppearanceColors[InMeshPart].VectorParams[Parameter] = NewColor;
}

void UAppearanceComponent::SaveData(USaveGameData* SaveData)
{
	SaveData->PlayerData.AppearanceColors.Empty();
	for (auto map : AppearanceColors)
	{
		SaveData->PlayerData.AppearanceColors.Add(map.Key);
		for (auto param : map.Value.VectorParams)
			SaveData->PlayerData.AppearanceColors[map.Key].VectorParams.Emplace(param.Key, param.Value);
	}

	SaveData->PlayerData.MeshIndices.Init(0, MeshIndices.Num());
	for (int32 i = 0; i < MeshIndices.Num(); ++i)
		SaveData->PlayerData.MeshIndices[i] = MeshIndices[i];
}

void UAppearanceComponent::LoadData(USaveGameData* const ReadData)
{
	AppearanceColors.Empty();
	for (auto map : ReadData->PlayerData.AppearanceColors)
		for (auto param : map.Value.VectorParams)
			ChangeColorData(map.Key, param.Key, param.Value);

	MeshIndices = ReadData->PlayerData.MeshIndices;
	for (int32 i = 0; i < MeshIndices.Num(); ++i)
		ChangeAppearance(EAppearancePart(i), MeshIndices[i]);
}
