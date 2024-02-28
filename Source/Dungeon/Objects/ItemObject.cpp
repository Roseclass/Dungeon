#include "Objects/ItemObject.h"
#include "Global.h"

void UItemObject::Init(int32 InX, int32 InY, UMaterialInstance* InIcon, UMaterialInstance* InIconRotated, AWeapon* InItem)
{
	DimensionX = InX;
	DimensionY = InY;
	Icon = InIcon;
	IconRotated = InIconRotated;
	Weapon = InItem;
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

void UItemObject::ChangeVisibility(EItemMode InMode)
{
	if (Weapon)Weapon->ChangeVisibility(InMode);
}
