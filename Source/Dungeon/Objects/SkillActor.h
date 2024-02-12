#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkillActor.generated.h"

/**
 * 스킬사용과 관련된 데이터, 함수를 갖고있는 액터
 */

class AProjectile;
class ADungeonCharacter;
class UImage;

UENUM(BlueprintType)
enum class ESkillTreeSkillState : uint8
{
	Locked, Unlocked, Acquired
};

USTRUCT(BlueprintType)
struct FSkillData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Montage")
		UAnimMontage* Montage;

	UPROPERTY(EditAnywhere, Category = "Montage")
		float PlayRate = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Montage")
		FName StartSection;

	UPROPERTY(EditAnywhere, Category = "Montage")
		FName RepeatSection;

	UPROPERTY(EditAnywhere, Category = "Status")
		bool bCanMove = false;

	UPROPERTY(EditAnywhere, Category = "Projectile")
		TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Projectile")
		FName SocketName;

	UPROPERTY(EditAnywhere, Category = "Projectile")
		bool bUseSocketLocation = 1;

	UPROPERTY(EditAnywhere, Category = "Projectile")
		bool bUseSocketRotation = 0;

	UPROPERTY(EditAnywhere, Category = "Widget")
		UObject* SkillImage;

	UPROPERTY(EditAnywhere, Category = "Widget")
		FVector2D PannelPosition;

	UPROPERTY(EditAnywhere, Category = "Widget")
		FVector2D ParentPosition;
};

UCLASS()
class DUNGEON_API ASkillActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ASkillActor();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;
	
	//property
private:
	ADungeonCharacter* OwnerCharacter;
	ASkillActor* Parent;
	TArray<ASkillActor*> Children;
protected:
	//트리형태로 스킬트리가될 예정
	UPROPERTY(EditDefaultsOnly)
		FSkillData Data;

	UPROPERTY(EditAnywhere, Category = "Widget")
		ESkillTreeSkillState SkillTreeState;

public:

	//function
private:
protected:
	FORCEINLINE ADungeonCharacter* GetOwnerCharacter() { return OwnerCharacter; }
public:
	virtual void Use();
	virtual void SpawnProjectile();

	//getter

	FORCEINLINE FSkillData* GetSkillData() { return &Data; } const
	FORCEINLINE ASkillActor* GetParent() { return Parent; } const
	FORCEINLINE const TArray<ASkillActor*>& GetChildren() const { return Children; };
	FORCEINLINE ESkillTreeSkillState GetSkillTreeState() { return SkillTreeState; } const

	//setter
	FORCEINLINE void SetOwnerCharacter(ADungeonCharacter* InCharacter) { OwnerCharacter = InCharacter; };
	FORCEINLINE void SetParent(ASkillActor* InSkillActor) { Parent = InSkillActor; };
	FORCEINLINE void AddChild(ASkillActor* InSkillActor) { Children.Add(InSkillActor); };
	FORCEINLINE void SetLocked() { SkillTreeState = ESkillTreeSkillState::Locked; };
	FORCEINLINE void SetUnlocked() { SkillTreeState = ESkillTreeSkillState::Unlocked; };
	FORCEINLINE void SetAcquired() { SkillTreeState = ESkillTreeSkillState::Acquired; };

};
