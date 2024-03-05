#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LobbyCharacter.generated.h"

/*
* ĳ���� ����â���� ������ ĳ����
* ���� �ʿ� x
*/

class USkeletalMeshComponent;
class UAppearanceComponent;

enum class EAppearancePart : uint8;

UCLASS()
class DUNGEON_API ALobbyCharacter : public AActor
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
protected:
	UPROPERTY(VisibleDefaultsOnly)
		USkeletalMeshComponent* RootMesh;

	UPROPERTY(VisibleDefaultsOnly)
		UAppearanceComponent* Appearance;
public:

	//function
private:
protected:
public:
	//Appearance
	void ChangeAppearance(EAppearancePart InMeshPart, int32 InIndex);
};
