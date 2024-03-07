#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "Characters/CharcterAppearanceData.h"
#include "AppearanceComponent.generated.h"

/**
 * ������ �����͸� ������� ĳ������ ������ ����
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
	FName Parameter;
	FLinearColor Color;
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
	TArray<TArray<FAppearancePartColor>> AppearanceColors;
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
