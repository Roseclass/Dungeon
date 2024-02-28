#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Objects/Weapon.h"
#include "ItemObject.generated.h"

/**
 * 
 */

class UMaterialInstance;
class UInventoryComponent;

UCLASS()
class DUNGEON_API UItemObject : public UObject
{
	GENERATED_BODY()
	//property
private:
	UPROPERTY()UInventoryComponent* InventoryComp;
	UPROPERTY()UMaterialInstance* Icon;
	UPROPERTY()UMaterialInstance* IconRotated;
	AWeapon* Weapon;
	int32 DimensionX;
	int32 DimensionY;
	bool bRotated;

protected:
public:

	//function
private:
protected:
public:
	void Init(int32 InX, int32 InY, UMaterialInstance* InIcon, UMaterialInstance* InIconRotated, AWeapon* InItem);
	void Rotate();
	void GetDimensions(int32& X, int32& Y);
	void ChangeVisibility(EItemMode InMode = EItemMode::Max);

	FORCEINLINE UMaterialInstance* GetIcon() {return bRotated ? IconRotated : Icon;}
	FORCEINLINE AWeapon* GetWeapon() {return Weapon;}
	FORCEINLINE bool IsRotated(){return bRotated;}
	FORCEINLINE UInventoryComponent* GetInventoryComp() { return InventoryComp; }
	FORCEINLINE void SetInventoryComp(UInventoryComponent* InComp) { InventoryComp = InComp; }
};
