#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkillActor.generated.h"

/**
 * ��ų���� ���õ� ������, �Լ��� �����ִ� ����
 */

class AProjectile;
class AWarningSign;
class ADungeonCharacterBase;
class ADungeonPlayerController;
class UImage;

DECLARE_MULTICAST_DELEGATE(FSkillTreeSkillStateChanged);
DECLARE_MULTICAST_DELEGATE_OneParam(FSkillUsing, ASkillActor*);

UENUM(BlueprintType)
enum class ESkillTreeSkillState : uint8
{
	Locked, Unlocked, Acquired
};

USTRUCT(BlueprintType)
struct FWaringSignData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		TSubclassOf<AWarningSign> WarningSignClass;

	UPROPERTY(EditAnywhere)
		float ForwardOffset = 0;

	UPROPERTY(EditAnywhere)
		float RightOffset = 0;

	UPROPERTY(EditAnywhere)
		FVector Scale = FVector(1, 1, 1);

	UPROPERTY(EditAnywhere)
		float Duration;

	UPROPERTY(EditAnywhere)
		float ExtraDuration;
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

	UPROPERTY(EditAnywhere, Category = "Damage")
		float BaseDamage = 10;

	UPROPERTY(EditAnywhere, Category = "Damage")
		float AdditiveDamage = 10;

	UPROPERTY(EditAnywhere, Category = "Damage")
		float BaseDamageRate = 1;

	UPROPERTY(EditAnywhere, Category = "Damage")
		float AdditiveDamageRate = 1;

	UPROPERTY(EditAnywhere, Category = "Widget")
		UObject* SkillImage;

	UPROPERTY(EditAnywhere, Category = "Widget")
		FVector2D PannelPosition;

	UPROPERTY(EditAnywhere, Category = "Widget")
		FVector2D ParentPosition;

	UPROPERTY(EditAnywhere, Category = "CoolDown")
		float CoolDown = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Range")
		float Range = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Condition")
		float ManaCost = 10.0f;

	UPROPERTY(EditAnywhere, Category = "WarningSign")
		TArray<FWaringSignData> WarningSignData;

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
	UPROPERTY(Replicated)ADungeonCharacterBase* OwnerCharacter;
	UPROPERTY(Replicated)ASkillActor* ParentSkill;
	UPROPERTY(Replicated, ReplicatedUsing = "OnRep_ChildrenSkills")TArray<ASkillActor*> ChildrenSkills;
	UPROPERTY(Replicated, ReplicatedUsing = "OnRep_CoolDown_Server")bool bCoolDown_Server;
	UPROPERTY(Replicated)float StartServerWorldTime;

	bool bCoolDown_Client;
	float StartWorldTime;

	bool bLoad;
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

	FSkillUsing OnSkillUsing;
	//function
private:
	UFUNCTION()void OnRep_ChildrenSkills();
	UFUNCTION()void OnRep_CoolDown_Server();
protected:
	UFUNCTION(NetMulticast, Reliable)virtual void Multicast_Use(ADungeonPlayerController* Exception);
	UFUNCTION(Client, Reliable)virtual void Client_UseAbort();
	UFUNCTION(Reliable, Server)virtual void Server_Use(ADungeonPlayerController* Exception);

	UFUNCTION(BlueprintImplementableEvent)float CalculateDamage(float InCharacterDamage);

	FORCEINLINE ADungeonCharacterBase* GetOwnerCharacter() { return OwnerCharacter; }
public:
	virtual void Load();
	UFUNCTION(Client, Reliable)virtual void Client_Use();
	virtual void SpawnProjectile();
	virtual void SpawnWarningSign(int32 InIndex);

	//getter
	FORCEINLINE const FSkillData* GetSkillData() const { return &Data; }
	FORCEINLINE ASkillActor* GetParent() const { return ParentSkill; }
	FORCEINLINE const TArray<ASkillActor*>& GetChildren() const { return ChildrenSkills; };
	FORCEINLINE ESkillTreeSkillState GetSkillTreeState() const { return SkillTreeState; }
	FORCEINLINE float GetCoolDown() const { return Data.CoolDown; }
	FORCEINLINE float GetManaCost() const { return Data.ManaCost; }
	FORCEINLINE bool IsCoolDown() const { return bCoolDown_Client; }
	float GetRemainingCoolDown() const;

	//setter
	FORCEINLINE void SetOwnerCharacter(ADungeonCharacterBase* InCharacter) { OwnerCharacter = InCharacter; };
	FORCEINLINE void SetParent(ASkillActor* InSkillActor) { ParentSkill = InSkillActor; };
	FORCEINLINE void AddChild(ASkillActor* InSkillActor) { ChildrenSkills.Add(InSkillActor); };
	void SetLocked();
	void SetUnlocked();
	void SetAcquired();

};

/*
* need limit condition
* mana?
* just use and filtering in server
* 
* how to know usable?
* change widget cover ex)lol,hots,dota
* 
* latency?
* use with widget condition and filtering in server
* most of all may be can use
* if this function use many times in sever that filtered by canuse state dont worry
* 
* mana						more mana				setcannotuse
* canuseskill(client) ==> canuseskill(sever) ==> useskill(server) ==> useskill(client)
*			canuseskill(client) ==> canuseskill(sever) ||
* 
* range?
* need filtering in controller
* 
*/