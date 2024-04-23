#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/ISave.h"
#include "LobbyCharacter.generated.h"

/*
* showing only on lobby
* no need to replicated
* cause lobby isnt multiplayed
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

	UPROPERTY(EditAnywhere)
		FString Name = "Player";

	UPROPERTY(EditDefaultsOnly, Category = "Zoom")
		FVector minLoc = { 50,30,120 }; 

	UPROPERTY(EditDefaultsOnly, Category = "Zoom")
		FVector maxLoc = { 20,10,150 }; 

	UPROPERTY(EditDefaultsOnly, Category = "Zoom")
		float minLen = 300;

	UPROPERTY(EditDefaultsOnly, Category = "Zoom")
		float maxLen = 100;

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

	FORCEINLINE void SetName(FString NewName) { Name = NewName; };
	FORCEINLINE FString GetName()const { return Name; };
};
