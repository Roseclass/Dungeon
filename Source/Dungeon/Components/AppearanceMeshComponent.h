#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "AppearanceMeshComponent.generated.h"

/**
 * 캐릭터의 메시에 붙일 하위 메시 컴포넌트
 * 단순히 현재 어디 파트에 붙어있는지 확인하는 용도
 * 부위가 많아지면 헷갈릴 가능성이 많아져서 블루프린트 단계에서 필요한 부위만 넣어서 사용
 */

enum class EAppearancePart : uint8;

UCLASS()
class DUNGEON_API UAppearanceMeshComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

private:
	EAppearancePart AppearancePart;
public:
	FORCEINLINE EAppearancePart GetAppearancePart() {return AppearancePart;}
};
