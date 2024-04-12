#include "Components/AppearanceComponent.h"
#include "Global.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

#include "SaveManager.h"
#include "Characters/PlayerCharacter.h"
#include "Components/AppearanceMeshComponent.h"

UAppearanceComponent::UAppearanceComponent()
{
	SetIsReplicatedByDefault(1);
	PrimaryComponentTick.bCanEverTick = true;
	Meshes.Init(nullptr, int32(EAppearancePart::Max));
	AppearanceAssets.Init(TArray<TSoftObjectPtr<USkeletalMesh>>(), int32(EAppearancePart::Max));
	MeshIndices.Init(0, int32(EAppearancePart::Max));
	AppearanceColors.Init(FAppearancePartColor(), int32(EAppearancePart::Max));
	MeshIndices_Client.Init(0, int32(EAppearancePart::Max));
	AppearanceColors_Client.Init(FAppearancePartColor(), int32(EAppearancePart::Max));
}

void UAppearanceComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UAppearanceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAppearanceComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicated 변수를 여기에 추가
	DOREPLIFETIME_CONDITION(UAppearanceComponent, MeshIndices, COND_None);
	DOREPLIFETIME_CONDITION(UAppearanceComponent, AppearanceColors, COND_None);
	DOREPLIFETIME_CONDITION(UAppearanceComponent, bShowHair, COND_None);
}

void UAppearanceComponent::OnRep_Meshindices()
{
	for (int32 i = 0; i < int32(EAppearancePart::Max); ++i)
	{
		if (MeshIndices[i] == MeshIndices_Client[i])continue;
		MeshIndices_Client[i] = MeshIndices[i];
		ChangeAppearance(EAppearancePart(i), MeshIndices[i]);
	}
}

void UAppearanceComponent::OnRep_AppearanceColors()
{
	for (int32 i = 0; i < int32(EAppearancePart::Max); ++i)
	{
		FAppearancePartColor& params = AppearanceColors_Client[i];
		for (int32 j = 0; j < AppearanceColors[i].VectorParams.Num(); ++j)
		{
			bool flag = 0;
			for (auto& param : params.VectorParams)
			{
				if (param.Name == AppearanceColors[i].VectorParams[j].Name)flag = 1;
				if (flag && param.Color != AppearanceColors[i].VectorParams[j].Color)
				{
					param.Color = AppearanceColors[i].VectorParams[j].Color;
					ChangeColor(EAppearancePart(i), param.Name, param.Color);
				}
			}
			if (!flag)
			{
				FAppearanceVectorParam x;
				x.Name = AppearanceColors[i].VectorParams[j].Name;
				x.Color = AppearanceColors[i].VectorParams[j].Color;
				params.VectorParams.Add(x);
				ChangeColor(EAppearancePart(i), x.Name, x.Color);
			}
		}
	}
}

void UAppearanceComponent::OnRep_ShowHair()
{
	for (auto i : Meshes)
	{
		UAppearanceMeshComponent* mesh = Cast<UAppearanceMeshComponent>(i);
		if (!mesh)continue;
		if (EAppearancePart::Hair != mesh->GetAppearancePart())continue;
		mesh->SetVisibility(bShowHair);
		break;
	}
}

void UAppearanceComponent::LoadSkeletalMeshAsync(const FSoftObjectPath& AssetRef, TFunction<void(USkeletalMesh*)> OnLoaded)
{
	FStreamableManager& AssetLoader = UAssetManager::GetStreamableManager();
	AssetLoader.RequestAsyncLoad(AssetRef, [OnLoaded, AssetRef]()
	{
		USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(AssetRef.TryLoad());

		if (OnLoaded && LoadedMesh)
		{
			OnLoaded(LoadedMesh);
		}
	});
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

void UAppearanceComponent::ChangeAppearance(EAppearancePart InMeshPart, int32 InIndex)
{
	int32 idx = int32(InMeshPart);
	CheckNull(Meshes[idx]);
	CheckFalse(AppearanceAssets[idx].IsValidIndex(InIndex));

	USkeletalMesh* mesh = nullptr;

	if (!AppearanceAssets[idx][InIndex].IsValid())
	{
		const FSoftObjectPath& AssetRef = AppearanceAssets[idx][InIndex].ToSoftObjectPath();
		auto& assetLoader = UAssetManager::GetStreamableManager();
		mesh = Cast<USkeletalMesh>(assetLoader.LoadSynchronous(AssetRef));
	}
	else 
	{ 
		mesh = Cast<USkeletalMesh>(AppearanceAssets[idx][InIndex].Get());
	}

	Meshes[idx]->SetSkeletalMesh(mesh);
	for (auto i : AppearanceColors[idx].VectorParams)
		ChangeColor(InMeshPart, i.Name, i.Color);
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

	for (int32 i = 0; i < int32(EAppearancePart::Max); ++i)
		ChangeAppearance(EAppearancePart(i), MeshIndices_Client[i]);

	// avoid race condition with OnRep_Meshindices
	FTimerHandle WaitHandle;
	float WaitTime = 0.1;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
	{
		for (int32 i = 0; i < int32(EAppearancePart::Max); ++i)
			ChangeAppearance(EAppearancePart(i), MeshIndices[i]);

		for (auto i : Meshes)
		{
			UAppearanceMeshComponent* mesh = Cast<UAppearanceMeshComponent>(i);
			if (!mesh)continue;
			if (EAppearancePart::Hair != mesh->GetAppearancePart())continue;
			bShowHair = i->IsVisible();
			break;
		}
	}), WaitTime, false);
}

void UAppearanceComponent::Server_ChangeAppearance_Implementation(EAppearancePart InMeshPart, int32 InIndex)
{
	int32 idx = int32(InMeshPart);
	MeshIndices[idx] = InIndex;
	ChangeAppearance(InMeshPart, InIndex);
}

void UAppearanceComponent::Server_ChangeColor_Implementation(EAppearancePart InMeshPart, FName Parameter, FLinearColor NewColor)
{
	int32 idx = int32(InMeshPart);
	FAppearancePartColor& params = AppearanceColors[idx];
	bool flag = 0;
	for (int32 j = 0; j < params.VectorParams.Num(); ++j)
	{
		if (params.VectorParams[j].Name == Parameter)flag = 1;
		if (flag && params.VectorParams[j].Color != NewColor)
			params.VectorParams[j].Color = NewColor;
	}
	if (!flag)
	{
		FAppearanceVectorParam x;
		x.Name = Parameter;
		x.Color = NewColor;
		params.VectorParams.Add(x);
	}
	ChangeColor(InMeshPart, Parameter, NewColor);
}

void UAppearanceComponent::Server_SetShowHair_Implementation(bool NewState)
{
	for (auto i : Meshes)
	{
		UAppearanceMeshComponent* mesh = Cast<UAppearanceMeshComponent>(i);
		if (!mesh)continue;
		if (EAppearancePart::Hair != mesh->GetAppearancePart())continue;
		CheckTrue(mesh->IsVisible() == NewState)
		break;
	}

	bShowHair = NewState;
	OnRep_ShowHair();
}

void UAppearanceComponent::SaveData(USaveGameData* SaveData)
{
	SaveData->PlayerData.AppearanceColors.Empty();
	for (int32 i = 0; i < int32(EAppearancePart::Max); ++i)
	{
		SaveData->PlayerData.AppearanceColors.Add(EAppearancePart(i));
		for (int32 j = 0; j < AppearanceColors[i].VectorParams.Num(); ++j)
		{
			FAppearanceVectorParam x;
			x.Name = AppearanceColors[i].VectorParams[j].Name;
			x.Color = AppearanceColors[i].VectorParams[j].Color;
			SaveData->PlayerData.AppearanceColors[EAppearancePart(i)].VectorParams.Add(x);
		}
	}

	SaveData->PlayerData.MeshIndices.Init(0, MeshIndices.Num());
	for (int32 i = 0; i < MeshIndices.Num(); ++i)
		SaveData->PlayerData.MeshIndices[i] = MeshIndices[i];
}

void UAppearanceComponent::LoadData(USaveGameData* const ReadData)
{
	for (auto map : ReadData->PlayerData.AppearanceColors)
		for (auto param : map.Value.VectorParams)
			Server_ChangeColor(map.Key, param.Name, param.Color);

	MeshIndices = ReadData->PlayerData.MeshIndices;
	for (int32 i = 0; i < MeshIndices.Num(); ++i)
		Server_ChangeAppearance(EAppearancePart(i), MeshIndices[i]);
}
