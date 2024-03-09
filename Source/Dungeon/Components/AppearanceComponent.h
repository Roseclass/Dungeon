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
struct FAppearancePartColor
{
	GENERATED_BODY()
public:
	UPROPERTY()TMap<FName,FLinearColor>VectorParams;
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
	TArray<int32> MeshIndices;
	TArray<TArray<TSoftObjectPtr<USkeletalMesh>>> AppearanceAssets;
	TMap<EAppearancePart, FAppearancePartColor> AppearanceColors;
protected:
	UPROPERTY(EditDefaultsOnly)
		UDataTable* DataTable;
public:

	//function
private:
	void ChangeColor(EAppearancePart InMeshPart, FName Parameter, FLinearColor NewColor);
protected:
public:
	UFUNCTION(BlueprintCallable)void Init(const TMap<EAppearancePart, USkeletalMeshComponent*>& InMeshes);
	UFUNCTION(BlueprintCallable)void ChangeAppearance(EAppearancePart InMeshPart, int32 InIndex);
	void ChangeColorData(EAppearancePart InMeshPart, FName Parameter, FLinearColor NewColor);

	void SaveData(USaveGameData* SaveData);
	void LoadData(USaveGameData* const ReadData);
};
