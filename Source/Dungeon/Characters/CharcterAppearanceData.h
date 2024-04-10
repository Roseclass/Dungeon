#pragma once

#include "CoreMinimal.h"
#include "CharcterAppearanceData.generated.h"

#define HeadCoverings_BaseHair_MAX 11
#define HeadCoverings_NoFacialHair_MAX 4
#define HeadCoverings_NoHair_MAX 13
#define Hair_MAX 29
#define HeadAttachment_Hair_MAX 1
#define HeadAttachment_Helmet_MAX 13
#define ChestAttachment_MAX 1
#define BackAttachment_MAX 16
#define ShoulderAttachment_Right_MAX 22
#define ShoulderAttachment_Left_MAX 22
#define ElbowAttachment_Right_MAX 7
#define ElbowAttachment_Left_MAX 7
#define HipsAttachment_MAX 13
#define KneeAttachment_Right_MAX 12
#define KneeAttachment_Left_MAX 12
#define Extra_MAX 4
#define Head_AllElements_MAX 23
#define Head_NoElements_MAX 13
#define Eyebrows_MAX 11
#define FacialHair_MAX 18
#define Torso_MAX 29
#define ArmUpperRight_MAX 21
#define ArmUpperLeft_MAX 21
#define ArmLowerRight_MAX 19
#define ArmLowerLeft_MAX 19
#define HandRight_MAX 18
#define HandLeft_MAX 18
#define Hips_MAX 29
#define LegRight_MAX 20
#define LegLeft_MAX 20

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

USTRUCT(BlueprintType)
struct FAppearanceData
{
	GENERATED_BODY()
private:
friend class UAppearanceObject;
protected:
	UPROPERTY(EditAnywhere)
		EAppearancePart PartType;

	UPROPERTY(EditAnywhere)
		FLinearColor PartColor;
	
	UPROPERTY(EditAnywhere, meta = (EditCondition = "PartType == EAppearancePart::HeadCoverings_BaseHair", EditConditionHides))
		int32 HeadCoverings_BaseHair;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "PartType == EAppearancePart::HeadCoverings_NoFacialHair", EditConditionHides))
		int32 HeadCoverings_NoFacialHair;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "PartType == EAppearancePart::HeadCoverings_NoHair", EditConditionHides))
		int32 HeadCoverings_NoHair;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "PartType == EAppearancePart::Hair", EditConditionHides))
		int32 Hair;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "PartType == EAppearancePart::HeadAttachment_Hair", EditConditionHides))
		int32 HeadAttachment_Hair;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "PartType == EAppearancePart::HeadAttachment_Helmet", EditConditionHides))
		int32 HeadAttachment_Helmet;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "PartType == EAppearancePart::ChestAttachment", EditConditionHides))
		int32 ChestAttachment;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "PartType == EAppearancePart::BackAttachment", EditConditionHides))
		int32 BackAttachment;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "PartType == EAppearancePart::ShoulderAttachment_Right", EditConditionHides))
		int32 ShoulderAttachment_Right;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "PartType == EAppearancePart::ShoulderAttachment_Left", EditConditionHides))
		int32 ShoulderAttachment_Left;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "PartType == EAppearancePart::ElbowAttachment_Right", EditConditionHides))
		int32 ElbowAttachment_Right;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "PartType == EAppearancePart::ElbowAttachment_Left", EditConditionHides))
		int32 ElbowAttachment_Left;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "PartType == EAppearancePart::HipsAttachment", EditConditionHides))
		int32 HipsAttachment;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "PartType == EAppearancePart::KneeAttachment_Right", EditConditionHides))
		int32 KneeAttachment_Right;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "PartType == EAppearancePart::KneeAttachment_Left", EditConditionHides))
		int32 KneeAttachment_Left;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "PartType == EAppearancePart::Extra", EditConditionHides))
		int32 Extra;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "PartType == EAppearancePart::Head_AllElements", EditConditionHides))
		int32 Head_AllElements;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "PartType == EAppearancePart::Head_NoElements", EditConditionHides))
		int32 Head_NoElements;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "PartType == EAppearancePart::Eyebrows", EditConditionHides))
		int32 Eyebrows;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "PartType == EAppearancePart::FacialHair", EditConditionHides))
		int32 FacialHair;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "PartType == EAppearancePart::Torso", EditConditionHides))
		int32 Torso;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "PartType == EAppearancePart::ArmUpperRight", EditConditionHides))
		int32 ArmUpperRight;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "PartType == EAppearancePart::ArmUpperLeft", EditConditionHides))
		int32 ArmUpperLeft;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "PartType == EAppearancePart::ArmLowerRight", EditConditionHides))
		int32 ArmLowerRight;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "PartType == EAppearancePart::ArmLowerLeft", EditConditionHides))
		int32 ArmLowerLeft;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "PartType == EAppearancePart::HandRight", EditConditionHides))
		int32 HandRight;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "PartType == EAppearancePart::HandLeft", EditConditionHides))
		int32 HandLeft;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "PartType == EAppearancePart::Hips", EditConditionHides))
		int32 Hips;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "PartType == EAppearancePart::LegRight", EditConditionHides))
		int32 LegRight;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "PartType == EAppearancePart::LegLeft", EditConditionHides))
		int32 LegLeft;

public:
	EAppearancePart GetPartType() const
	{
		return PartType;
	}

	int32 GetDataIndex() const
	{
		int32 result = 0;
		switch (PartType)
		{
		case EAppearancePart::HeadCoverings_BaseHair:result = HeadCoverings_BaseHair;break;
		case EAppearancePart::HeadCoverings_NoFacialHair:result = HeadCoverings_NoFacialHair;break;
		case EAppearancePart::HeadCoverings_NoHair:result = HeadCoverings_NoHair;break;
		case EAppearancePart::Hair:result = result;break;
		case EAppearancePart::HeadAttachment_Hair:result = HeadAttachment_Hair;break;
		case EAppearancePart::HeadAttachment_Helmet:result = HeadAttachment_Helmet;break;
		case EAppearancePart::ChestAttachment:result = ChestAttachment;break;
		case EAppearancePart::BackAttachment:result = BackAttachment;break;
		case EAppearancePart::ShoulderAttachment_Right:result = ShoulderAttachment_Right;break;
		case EAppearancePart::ShoulderAttachment_Left:result = ShoulderAttachment_Left;break;
		case EAppearancePart::ElbowAttachment_Right:result = ElbowAttachment_Right;break;
		case EAppearancePart::ElbowAttachment_Left:result = ElbowAttachment_Left;break;
		case EAppearancePart::HipsAttachment:result = HipsAttachment;break;
		case EAppearancePart::KneeAttachment_Right:result = KneeAttachment_Right;break;
		case EAppearancePart::KneeAttachment_Left:result = KneeAttachment_Left;break;
		case EAppearancePart::Extra:result = Extra;break;
		case EAppearancePart::Head_AllElements:result = Head_AllElements;break;
		case EAppearancePart::Head_NoElements:result = Head_NoElements;break;
		case EAppearancePart::Eyebrows:result = Eyebrows;break;
		case EAppearancePart::FacialHair:result = FacialHair;break;
		case EAppearancePart::Torso:result = Torso;break;
		case EAppearancePart::ArmUpperRight:result = ArmUpperRight;break;
		case EAppearancePart::ArmUpperLeft:result = ArmUpperLeft;break;
		case EAppearancePart::ArmLowerRight:result = ArmLowerRight;break;
		case EAppearancePart::ArmLowerLeft:result = ArmLowerLeft;break;
		case EAppearancePart::HandRight:result = HandRight;break;
		case EAppearancePart::HandLeft:result = HandLeft;break;
		case EAppearancePart::Hips:result = Hips;break;
		case EAppearancePart::LegRight:result = LegRight;break;
		case EAppearancePart::LegLeft:result = LegLeft;break;
		case EAppearancePart::Max:break;
		default:break;
		}
		return result;
	}
};

