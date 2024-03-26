#pragma once

#include "CoreMinimal.h"
#include "Characters/DungeonCharacterBase.h"
#include "Interfaces/ISave.h"
#include "PlayerCharacter.generated.h"

/**
 * player character's basic class
 * 
 */

class UCameraComponent;
class USceneCaptureComponent2D;
class USpringArmComponent;
class UStaticMeshComponent;
class UAppearanceComponent;
class USkillTreeComponent;
class UQuestComponent;

struct FSkillData;

enum class EItemMode : uint8;
enum class EAppearancePart : uint8;

UCLASS(Blueprintable)
class APlayerCharacter : public ADungeonCharacterBase, public IISave
{
	GENERATED_BODY()

public:
	APlayerCharacter();
protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaSeconds) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual FGenericTeamId GetGenericTeamId() const;

	//property
private:
	//scene
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* TopDownCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* CameraBoom;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Minimap", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* MinimapIcon;

	USpringArmComponent* MinimapArm;
	USceneCaptureComponent2D* MinimapCapture;

	//actor
	UPROPERTY(VisibleDefaultsOnly)
		UAppearanceComponent* Appearance;

	UPROPERTY(VisibleDefaultsOnly)
		USkillTreeComponent* SkillTree;

	UPROPERTY(VisibleDefaultsOnly)
		UQuestComponent* Quest;
protected:
public:

	//function
private:
	void OffAllWidget();
protected:
	virtual void Init()override;
public:
	void InitClientWidget();
	
	//Appearance
	UFUNCTION()void ChangeAppearance(EAppearancePart InMeshPart, int32 InIndex);
	UFUNCTION()void ChangeColorData(EAppearancePart InMeshPart, FName Parameter, FLinearColor NewColor);

	//Skill
	virtual void UseSkill(int32 Idx)override;
	void UseLeft();
	void UseRight();
	void UseQ();
	void UseW();
	void UseE();
	void UseR();
	void ChangeQuickSlotData(int32 Index, ASkillActor* InSkillActor);

	//Inventory
	virtual void TryAddItem(AWeapon* InObject);

	//widget Toggle
	void ToggleSkillTree();
	void ToggleInventory();

	//Save Load
	virtual	FString GetUniqueSaveName() override;
	virtual	void OnBeforeSave(USaveGameData* SaveData) override;
	virtual	void OnAfterLoad(USaveGameData* const ReadData) override;

	FORCEINLINE UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
};

