#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillTreeComponent.generated.h"

/**
 * ��ųƮ���� �����ϴ� ������Ʈ
 */

class USaveGameData;
class UUW_SkillTree;

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
	
	TArray<const FSkillData*>RootDatas;
	UPROPERTY()UUW_SkillTree* Widget;
protected:
public:

	//function
private:
protected:
public:
	void Init(const TArray<const FSkillData*> InDatas, TFunction<void(int32, const FSkillData&)> OnPopupClicked);
	void AddPoints();
	//void Acquire(ASkillActor* SkillActor);

	//Widget
	bool IsWidgetVisible();
	void OnWidget();
	void OffWidget();

	// save
	void SaveData(USaveGameData* SaveData);
	void LoadData(USaveGameData* const ReadData);

	FORCEINLINE const TArray<const FSkillData*>& GetRootDatas() { return RootDatas; }
};
