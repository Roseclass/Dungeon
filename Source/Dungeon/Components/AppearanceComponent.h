#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "AppearanceComponent.generated.h"

/**
 * 정해진 데이터를 기반으로 캐릭터의 외형을 변경
 */

class USkeletalMesh;

UENUM(BlueprintType)
enum class EAppearancePart : uint8
{
	HeadCoverings_BaseHair,
	HeadCoverings_NoFacialHair,
	HeadCoverings_NoHair,
	Hair,
	HeadAttachment_Hair,
	HeadAttachment_Helmet,
	ChestAttachment,
	BackAttachment,
	ShoulderAttachment_Right,
	ShoulderAttachment_Left,
	ElbowAttachment_Right,
	ElbowAttachment_Left,
	HipsAttachment,
	KneeAttachment_Right,
	KneeAttachment_Left,
	Extra,
	Head_AllElements,
	Head_NoElements,
	Eyebrows,
	FacialHair,
	Torso,
	ArmUpperRight,
	ArmUpperLeft,
	ArmLowerRight,
	ArmLowerLeft,
	HandRight,
	HandLeft,
	Hips,
	LegRight,
	LegLeft,
	Max
};

USTRUCT()
struct FAppearanceData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere)
		FString AssetID;
	
	UPROPERTY(EditAnywhere)
		EAppearancePart MeshPart;
	
	UPROPERTY(EditAnywhere)
		TSoftObjectPtr<USkeletalMesh> Asset;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEON_API UAppearanceComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAppearanceComponent();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//property
private:
protected:
public:

	//function
private:
protected:
public:
};
