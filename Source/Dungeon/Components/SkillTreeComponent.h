#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/NetSerialization.h"
#include "SkillTreeComponent.generated.h"

/**
 * 스킬트리를 제어하는 컴포넌트
 */

class USaveGameData;
class UUW_SkillTree;
class USkillComponent;

struct FSkillData;

UENUM(BlueprintType)
enum class ESkillTreeSkillState : uint8
{
	Locked, Unlocked,
	Acquired_1, Acquired_2, Acquired_3,
	Acquired_4, Acquired_5, Acquired_6,
	Acquired_7, Acquired_8, Acquired_9, Acquired_10,
	Max
};

USTRUCT(BlueprintType)
struct FSkillStateItem : public FFastArraySerializerItem
{
	GENERATED_BODY()
		UPROPERTY()ESkillTreeSkillState SkillState;
};

USTRUCT(BlueprintType)
struct FSkillStateArray : public FFastArraySerializer
{
	GENERATED_BODY()
		UPROPERTY()TArray<FSkillStateItem> Items;

	void MarkItemDirty(FSkillStateItem& Item)
	{
		MarkItemDirty(Item);
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEON_API USkillTreeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USkillTreeComponent();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//property
private:
	USkillComponent* SkillComp;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UUW_SkillTree> WidgetClass;
	
	TArray<FVector2D>RootDatas;
	TMap<FVector2D, TArray<FVector2D>>TreeDatas;
	UPROPERTY()UUW_SkillTree* Widget;

	TArray<TArray<int32>> SkillTreeDatas;
	TArray<ESkillTreeSkillState> SkillWidgetStates;
	UPROPERTY(ReplicatedUsing = "OnRep_SkillStates")FSkillStateArray SkillStates;
protected:
public:

	//function
private:
	void MakeTreeDatas(const TArray<const FSkillData*> InSkillDatas);

	UFUNCTION()void OnRep_SkillStates();

	UFUNCTION(Reliable, Server)void Server_UpdateSkillState(int32 InSkillID, ESkillTreeSkillState InSkillState);
	UFUNCTION(Reliable, Server)void Server_LevelUpSkillState(int32 InSkillID);
protected:
public:
	void Init(const TArray<const FSkillData*> InDatas, USkillComponent* InSkillComp, TFunction<void(int32, int32)> OnPopupClicked);
	void AddPoints();
	void LevelUp(int32 InSkillID);

	//Widget
	bool IsWidgetVisible();
	void OnWidget();
	void OffWidget();

	// save
	void SaveData(USaveGameData* SaveData);
	void LoadData(USaveGameData* const ReadData);

	FORCEINLINE TArray<FVector2D> GetRootDatas() const { return RootDatas; }
	FORCEINLINE TMap<FVector2D, TArray<FVector2D>> GetTreeDatas() const { return TreeDatas; }
};
