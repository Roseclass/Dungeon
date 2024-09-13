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

enum class EItemType : uint8;

UCLASS()
class DUNGEON_API UItemObject : public UObject
{
	GENERATED_BODY()
	//property
private:
	EItemType Type;

	UPROPERTY()UInventoryComponent* InventoryComp;
	UPROPERTY()UMaterialInstance* Icon;
	UPROPERTY()UMaterialInstance* IconRotated;
	AEqquipment* Eqquipment;
	int32 DimensionX;
	int32 DimensionY;
	bool bRotated;

protected:
public:

	//function
private:
protected:
public:
	void Init(int32 InX, int32 InY, UMaterialInstance* InIcon, UMaterialInstance* InIconRotated, AEqquipment* InEqquipment, EItemType InType);
	void Rotate();
	void GetDimensions(int32& X, int32& Y);

	FORCEINLINE EItemType GetType() const {return Type;}
	FORCEINLINE UMaterialInstance* GetIcon() const {return bRotated ? IconRotated : Icon;}
	FORCEINLINE AEqquipment* GetEqquipment() const {return Eqquipment;}
	FORCEINLINE bool IsRotated() const{return bRotated;}
	FORCEINLINE UInventoryComponent* GetInventoryComp() const { return InventoryComp; }
	FORCEINLINE void SetInventoryComp(UInventoryComponent* InComp) { InventoryComp = InComp; }
};
