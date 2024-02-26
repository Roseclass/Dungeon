#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkillActor.generated.h"

/**
 * ��ų���� ���õ� ������, �Լ��� �����ִ� ����
 */

class AProjectile;
class ADungeonCharacter;
class ADungeonPlayerController;
class UImage;

DECLARE_MULTICAST_DELEGATE(FSkillTreeSkillStateChanged);

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

	UPROPERTY(EditAnywhere, Category = "CoolTime")
		float CoolTime = 10.0f;
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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//property
private:
	UPROPERTY(Replicated)ADungeonCharacter* OwnerCharacter;
	UPROPERTY(Replicated)ASkillActor* ParentSkill;
	UPROPERTY(Replicated)TArray<ASkillActor*> ChildrenSkills;

	bool bCoolTime;
	float CurrnetCoolTime;

protected:
	//Ʈ�����·� ��ųƮ������ ����
	UPROPERTY(EditDefaultsOnly)
		FSkillData Data;

	UPROPERTY(EditAnywhere, Category = "Widget")
		ESkillTreeSkillState SkillTreeState;

public:
	FSkillTreeSkillStateChanged OnLocked;
	FSkillTreeSkillStateChanged OnUnlocked;
	FSkillTreeSkillStateChanged OnAcquired;
	//function
private:
protected:
	UFUNCTION(NetMulticast, Reliable)virtual void Multicast_Use(ADungeonPlayerController* Exception);
	UFUNCTION(Reliable, Server)virtual void Server_Use(ADungeonPlayerController* Exception);

	FORCEINLINE ADungeonCharacter* GetOwnerCharacter() { return OwnerCharacter; }
public:
	virtual void Load();
	UFUNCTION(Client, Reliable)virtual void Client_Use();
	UFUNCTION(Reliable, Server)virtual void Server_SpawnProjectile();
	virtual void CoolTimeStart();


	//getter
	FORCEINLINE const FSkillData* GetSkillData() const { return &Data; }
	FORCEINLINE ASkillActor* GetParent() const { return ParentSkill; }
	FORCEINLINE const TArray<ASkillActor*>& GetChildren() const { return ChildrenSkills; };
	FORCEINLINE ESkillTreeSkillState GetSkillTreeState() const { return SkillTreeState; }
	FORCEINLINE float GetMaxCoolTime() const { return Data.CoolTime; }
	FORCEINLINE float GetCurrnetCoolTime() const { return CurrnetCoolTime; }
	FORCEINLINE bool IsCoolTime() const { return bCoolTime; }

	//setter
	FORCEINLINE void SetOwnerCharacter(ADungeonCharacter* InCharacter) { OwnerCharacter = InCharacter; };
	FORCEINLINE void SetParent(ASkillActor* InSkillActor) { ParentSkill = InSkillActor; };
	FORCEINLINE void AddChild(ASkillActor* InSkillActor) { ChildrenSkills.Add(InSkillActor); };
	void SetLocked();
	void SetUnlocked();
	void SetAcquired();

};
