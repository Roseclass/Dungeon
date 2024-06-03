#pragma once

#include "CoreMinimal.h"
#include "Characters/DungeonCharacterBase.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "Interfaces/ISave.h"
#include "PlayerCharacter.generated.h"

/**
 * player character's basic class
 * 
 */

class UCameraComponent;
class USceneCaptureComponent2D;
class UTravelEffectComponent;
class USpringArmComponent;
class UStaticMeshComponent;
class UAppearanceComponent;
class UClearViewComponent;
class USkillTreeComponent;
class UQuestComponent;
class AEqquipment;

class UAbilitySystemComponent;
class UAttributeSetBase;
class UGameplayAbility;

struct FSkillData;

enum class EItemMode : uint8;
enum class EAppearancePart : uint8;

UCLASS(Blueprintable)
class APlayerCharacter : public ADungeonCharacterBase, public IISave, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	APlayerCharacter();
protected:
	virtual void BeginPlay() override;
public:

	virtual void Tick(float DeltaSeconds) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual FGenericTeamId GetGenericTeamId() const override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// for Gameplay Ability System
protected:
	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;
	FDelegateHandle ManaChangedDelegateHandle;
	FDelegateHandle MaxManaChangedDelegateHandle;

	// Attribute changed callbacks
	virtual void HealthChanged(const FOnAttributeChangeData& Data);
	virtual void MaxHealthChanged(const FOnAttributeChangeData& Data);
	virtual void ManaChanged(const FOnAttributeChangeData& Data);
	virtual void MaxManaChanged(const FOnAttributeChangeData& Data);

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
		UAbilitySystemComponent* AbilitySystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
		UAttributeSetBase* AttributeSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
		TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

	UPROPERTY(VisibleDefaultsOnly)
		UAppearanceComponent* Appearance;

	UPROPERTY(VisibleDefaultsOnly)
		UClearViewComponent* ClearView;

	UPROPERTY(VisibleDefaultsOnly)
		USkillTreeComponent* SkillTree;

	UPROPERTY(VisibleDefaultsOnly)
		UQuestComponent* Quest;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = true))
		UTravelEffectComponent* TravelPostProcess;

protected:
public:

	//function
private:
	void OffAllWidget();
protected:
	virtual void Init()override;
	virtual void ChangeState(EStateType PrevType, EStateType NewType);
public:
	void InitClientWidget();
	
	//Appearance
	UFUNCTION()void ChangeAppearance(EAppearancePart InMeshPart, int32 InIndex);
	UFUNCTION()void ChangeColorData(EAppearancePart InMeshPart, FName Parameter, FLinearColor NewColor);
	UFUNCTION()void SetShowHair(bool NewState);

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
	virtual void TryAddItem(AEqquipment* InObject);

	//widget
	void ToggleSkillTree();
	void ToggleInventory();
	void ToggleQuest();
	void RevealSkillTree();
	void RevealInventory();
	void RevealQuest();
	void HideSkillTree();
	void HideInventory();
	void HideQuest();

	//Save Load
	virtual	FString GetUniqueSaveName() override;
	virtual	void OnBeforeSave(USaveGameData* SaveData) override;
	virtual	void OnAfterLoad(USaveGameData* const ReadData) override;
	void OnAfterLoad_ClientWidget(USaveGameData* const ReadData);

	FORCEINLINE UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

};

