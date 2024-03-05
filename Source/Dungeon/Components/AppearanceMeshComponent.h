#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "AppearanceMeshComponent.generated.h"

/**
 * ĳ������ �޽ÿ� ���� ���� �޽� ������Ʈ
 * �ܼ��� ���� ��� ��Ʈ�� �پ��ִ��� Ȯ���ϴ� �뵵
 * ������ �������� �򰥸� ���ɼ��� �������� �������Ʈ �ܰ迡�� �ʿ��� ������ �־ ���
 */

enum class EAppearancePart : uint8;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEON_API UAppearanceMeshComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly)
		EAppearancePart AppearancePart;
public:
	FORCEINLINE EAppearancePart GetAppearancePart() {return AppearancePart;}
};
