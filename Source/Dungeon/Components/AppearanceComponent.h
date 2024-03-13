#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "Characters/CharcterAppearanceData.h"
#include "AppearanceComponent.generated.h"

/**
 * 정해진 데이터를 기반으로 캐릭터의 외형을 변경
 */

class USkeletalMesh;
class USkeletalMeshComponent;
class USaveGameData;

USTRUCT()
struct FAppearanceDataTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere)
		EAppearancePart MeshPart;
	
	UPROPERTY(EditAnywhere)
		TSoftObjectPtr<USkeletalMesh> Asset;
};

USTRUCT()
struct FAppearanceVectorParam
{
	GENERATED_BODY()
public:
	UPROPERTY()FName Name;
	UPROPERTY()FLinearColor Color;
};

USTRUCT()
struct FAppearancePartColor
{
	GENERATED_BODY()
public:
	UPROPERTY()TArray<FAppearanceVectorParam>VectorParams;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEON_API UAppearanceComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAppearanceComponent();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//property
private:
	TArray<USkeletalMeshComponent*> Meshes;
	TArray<TArray<TSoftObjectPtr<USkeletalMesh>>> AppearanceAssets;
	UPROPERTY(Replicated, ReplicatedUsing = "OnRep_Meshindices")TArray<int32> MeshIndices;
	UPROPERTY(Replicated, ReplicatedUsing = "OnRep_AppearanceColors")TArray<FAppearancePartColor> AppearanceColors;
	TArray<int32> MeshIndices_Client;
	TArray<FAppearancePartColor> AppearanceColors_Client;
protected:
	UPROPERTY(EditDefaultsOnly)
		UDataTable* DataTable;
public:

	//function
private:
	UFUNCTION()void OnRep_Meshindices();
	UFUNCTION()void OnRep_AppearanceColors();
	void LoadSkeletalMeshAsync(const FSoftObjectPath& AssetRef, TFunction<void(USkeletalMesh*)> OnLoaded);
	void ChangeColor(EAppearancePart InMeshPart, FName Parameter, FLinearColor NewColor);
	void ChangeAppearance(EAppearancePart InMeshPart, int32 InIndex);
protected:
public:
	void Init(const TMap<EAppearancePart, USkeletalMeshComponent*>& InMeshes);
	UFUNCTION(Reliable, Server)void Server_ChangeAppearance(EAppearancePart InMeshPart, int32 InIndex);
	UFUNCTION(Reliable, Server)void Server_ChangeColor(EAppearancePart InMeshPart, FName Parameter, FLinearColor NewColor);

	void SaveData(USaveGameData* SaveData);
	void LoadData(USaveGameData* const ReadData);
};
