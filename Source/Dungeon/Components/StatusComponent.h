#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatusComponent.generated.h"

/**
 * 여러가지 수치(체력..)들을 관리하고
 * 움직임 여부, 스킬 사용 여부를 관리함
 * 
 * canmove,canuse는 어디서 갱신되나요? >> 몽타주에 노티파이로 붙여놨습니다
 */

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEON_API UStatusComponent : public UActorComponent
{
	GENERATED_BODY()
	//property
private:
	bool bCanUse = 1;
	bool bCanMove = 1;
protected:
public:

	//engine
private:
protected:
public:
	UStatusComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//function
private:
protected:
public:
	void SetUse();
	void SetCannotUse();

	void SetMove();
	void SetStop();

	FORCEINLINE bool CanUse() {return bCanUse;}
	FORCEINLINE bool CanMove() {return bCanMove;}
};
