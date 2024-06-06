#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillTreeComponent.generated.h"

/**
 * 스킬트리를 제어하는 컴포넌트
 */

class USaveGameData;
class UUW_SkillTree;
class USkillComponent;

struct FSkillData;

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

	//property
private:
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UUW_SkillTree> WidgetClass;
	
	TArray<FVector2D>RootDatas;
	TMap<FVector2D, TArray<FVector2D>>TreeDatas;
	UPROPERTY()UUW_SkillTree* Widget;
protected:
public:

	//function
private:
protected:
public:
	void Init(const TArray<const FSkillData*> InDatas, USkillComponent* InSkillComp, TFunction<void(int32, int32)> OnPopupClicked);
	void AddPoints();
	//void Acquire(ASkillActor* SkillActor);

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
