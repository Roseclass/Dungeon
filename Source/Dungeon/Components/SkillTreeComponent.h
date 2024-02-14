#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillTreeComponent.generated.h"

/**
 * 스킬트리를 제어하는 컴포넌트
 */

class ASkillActor;
class UUW_SkillTree;

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
	
	TArray<ASkillActor*>RootActors;
	UUW_SkillTree* Widget;
protected:
public:

	//function
private:
protected:
public:
	void Init(const TArray<ASkillActor*>& Array, TFunction<void(int32, ASkillActor*)> OnPopupClicked);
	void AddPoints();
	void Acquire(ASkillActor* SkillActor);

	FORCEINLINE const TArray<ASkillActor*>& GetRootActors() { return RootActors; }
};
