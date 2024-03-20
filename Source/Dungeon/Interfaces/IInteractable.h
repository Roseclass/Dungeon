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

class APlayerCharacter;

class DUNGEON_API IIInteractable
{
	GENERATED_BODY()
public:
	virtual void Interact(APlayerCharacter* InPlayer) = 0;			//��ȣ�ۿ�
	virtual void PreInteractEvent(APlayerCharacter* InPlayer) = 0;	//����Ʈ���̽��� �����Ǹ� ȣ��
	virtual void InInteractEvent(APlayerCharacter* InPlayer) = 0;	//����Ʈ���̽��� �����Ǵ� ���� ȣ��
	virtual void PostInteractEvent(APlayerCharacter* InPlayer) = 0;	//����Ʈ���̽��� �������� ȣ��
	virtual bool IsInteractable() = 0;
};
