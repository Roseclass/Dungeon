#include "Characters/AppearanceObject.h"
#include "Global.h"

#if WITH_EDITOR
void UAppearanceObject::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	AppearanceData.HeadCoverings_BaseHair = FMath::Clamp(AppearanceData.HeadCoverings_BaseHair, 0, HeadCoverings_BaseHair_MAX);
	AppearanceData.HeadCoverings_NoFacialHair = FMath::Clamp(AppearanceData.HeadCoverings_NoFacialHair, 0, HeadCoverings_NoFacialHair_MAX);
	AppearanceData.HeadCoverings_NoHair = FMath::Clamp(AppearanceData.HeadCoverings_NoHair, 0, HeadCoverings_NoHair_MAX);
	AppearanceData.Hair = FMath::Clamp(AppearanceData.Hair, 0, Hair_MAX);
	AppearanceData.HeadAttachment_Hair = FMath::Clamp(AppearanceData.HeadAttachment_Hair, 0, HeadAttachment_Hair_MAX);
	AppearanceData.HeadAttachment_Helmet = FMath::Clamp(AppearanceData.HeadAttachment_Helmet, 0, HeadAttachment_Helmet_MAX);
	AppearanceData.ChestAttachment = FMath::Clamp(AppearanceData.ChestAttachment, 0, ChestAttachment_MAX);
	AppearanceData.BackAttachment = FMath::Clamp(AppearanceData.BackAttachment, 0, BackAttachment_MAX);
	AppearanceData.ShoulderAttachment_Right = FMath::Clamp(AppearanceData.ShoulderAttachment_Right, 0, ShoulderAttachment_Right_MAX);
	AppearanceData.ShoulderAttachment_Left = FMath::Clamp(AppearanceData.ShoulderAttachment_Left, 0, ShoulderAttachment_Left_MAX);
	AppearanceData.ElbowAttachment_Right = FMath::Clamp(AppearanceData.ElbowAttachment_Right, 0, ElbowAttachment_Right_MAX);
	AppearanceData.ElbowAttachment_Left = FMath::Clamp(AppearanceData.ElbowAttachment_Left, 0, ElbowAttachment_Left_MAX);
	AppearanceData.HipsAttachment = FMath::Clamp(AppearanceData.HipsAttachment, 0, HipsAttachment_MAX);
	AppearanceData.KneeAttachment_Right = FMath::Clamp(AppearanceData.KneeAttachment_Right, 0, KneeAttachment_Right_MAX);
	AppearanceData.KneeAttachment_Left = FMath::Clamp(AppearanceData.KneeAttachment_Left, 0, KneeAttachment_Left_MAX);
	AppearanceData.Extra = FMath::Clamp(AppearanceData.Extra, 0, Extra_MAX);
	AppearanceData.Head_AllElements = FMath::Clamp(AppearanceData.Head_AllElements, 0, Head_AllElements_MAX);
	AppearanceData.Head_NoElements = FMath::Clamp(AppearanceData.Head_NoElements, 0, Head_NoElements_MAX);
	AppearanceData.Eyebrows = FMath::Clamp(AppearanceData.Eyebrows, 0, Eyebrows_MAX);
	AppearanceData.FacialHair = FMath::Clamp(AppearanceData.FacialHair, 0, FacialHair_MAX);
	AppearanceData.Torso = FMath::Clamp(AppearanceData.Torso, 0, Torso_MAX);
	AppearanceData.ArmUpperRight = FMath::Clamp(AppearanceData.ArmUpperRight, 0, ArmUpperRight_MAX);
	AppearanceData.ArmUpperLeft = FMath::Clamp(AppearanceData.ArmUpperLeft, 0, ArmUpperLeft_MAX);
	AppearanceData.ArmLowerRight = FMath::Clamp(AppearanceData.ArmLowerRight, 0, ArmLowerRight_MAX);
	AppearanceData.ArmLowerLeft = FMath::Clamp(AppearanceData.ArmLowerLeft, 0, ArmLowerLeft_MAX);
	AppearanceData.HandRight = FMath::Clamp(AppearanceData.HandRight, 0, HandRight_MAX);
	AppearanceData.HandLeft = FMath::Clamp(AppearanceData.HandLeft, 0, HandLeft_MAX);
	AppearanceData.Hips = FMath::Clamp(AppearanceData.Hips, 0, Hips_MAX);
	AppearanceData.LegRight = FMath::Clamp(AppearanceData.LegRight, 0, LegRight_MAX);
	AppearanceData.LegLeft = FMath::Clamp(AppearanceData.LegLeft, 0, LegLeft_MAX);
}
#endif