#include "Objects/ItemObject.h"
#include "Global.h"

void UItemObject::Init(int32 InX, int32 InY, UMaterialInstance* InIcon, UMaterialInstance* InIconRotated, AEqquipment* InEqquipment, EItemType InType)
{
	DimensionX = InX;
	DimensionY = InY;
	Icon = InIcon;
	IconRotated = InIconRotated;
	
	Eqquipment = InEqquipment;

	Type = InType;
}

void UItemObject::Rotate()
{
	bRotated = !bRotated;
}

void UItemObject::GetDimensions(int32& X, int32& Y)
{
	if (bRotated)
	{
		X = DimensionY;
		Y = DimensionX;
	}
	else
	{
		X = DimensionX;
		Y = DimensionY;
	}
}
