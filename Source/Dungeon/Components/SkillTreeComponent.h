#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillTreeComponent.generated.h"

/**
 * ��ųƮ���� �����ϴ� ������Ʈ
 */

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
protected:
public:

	//function
private:
protected:
public:
};
