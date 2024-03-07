#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

/**
 * 
 */

class ALobbyCharacter;

UCLASS()
class DUNGEON_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

	//property
private:
protected:
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<ALobbyCharacter> LobbyCharacterClass;
public:

	//function
private:
protected:
public:
};
//
// ������ ���� ����ŭ ����,�ε�,��ũ
//