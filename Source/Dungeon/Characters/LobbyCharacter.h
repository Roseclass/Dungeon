#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/ISave.h"
#include "LobbyCharacter.generated.h"

/*
* 캐릭터 선택창에서 보여줄 캐릭터
* 복제 필요 x
*/

class UCameraComponent;
class USpringArmComponent;
class USkeletalMeshComponent;
class UAppearanceComponent;

enum class EAppearancePart : uint8;

UCLASS()
class DUNGEON_API ALobbyCharacter : public AActor, public IISave
{
	GENERATED_BODY()

public:
	ALobbyCharacter();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;
	
	//property
private:
	int32 ZoomRate;
protected:
	UPROPERTY(VisibleDefaultsOnly)
		UCameraComponent* Camera;

	UPROPERTY(VisibleDefaultsOnly)
		USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleDefaultsOnly)
		USkeletalMeshComponent* RootMesh;

	UPROPERTY(VisibleDefaultsOnly)
		UAppearanceComponent* Appearance;

	UPROPERTY(EditAnywhere)
		FString UniqueName = "Player";
public:

	//function
private:
protected:
public:
	//Appearance
	void ChangeAppearance(EAppearancePart InMeshPart, int32 InIndex);
	void ChangeColorData(EAppearancePart InMeshPart, FName Parameter, FLinearColor NewColor);

	//WidgetAction
	void OnWheel(float Delta);

	//IISave
	virtual	FString GetUniqueSaveName() override;
	virtual	void OnBeforeSave(USaveGameData* SaveData) override;
	virtual	void OnAfterLoad(USaveGameData* const ReadData) override;

};
