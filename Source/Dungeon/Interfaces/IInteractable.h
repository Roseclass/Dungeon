#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IInteractable.generated.h"

UINTERFACE(MinimalAPI)
class UIInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */

class ADungeonPlayerController;

class DUNGEON_API IIInteractable
{
	GENERATED_BODY()
public:
	virtual void Interact(ADungeonPlayerController* InPlayer) = 0;			//��ȣ�ۿ�
	virtual void PreInteractEvent(ADungeonPlayerController* InPlayer) = 0;	//����Ʈ���̽��� �����Ǹ� ȣ��
	virtual void InInteractEvent(ADungeonPlayerController* InPlayer) = 0;	//����Ʈ���̽��� �����Ǵ� ���� ȣ��
	virtual void PostInteractEvent(ADungeonPlayerController* InPlayer) = 0;	//����Ʈ���̽��� �������� ȣ��
	virtual bool IsInteractable() = 0;
};
