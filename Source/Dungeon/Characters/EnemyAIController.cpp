#include "Characters/EnemyAIController.h"
#include "Global.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

#include "Characters/DungeonCharacter.h"
#include "Characters/Enemy.h"
#include "Components/BehaviorComponent.h"

AEnemyAIController::AEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	CHelpers::CreateActorComponent(this, &Behavior, "Behavior");
	CHelpers::CreateActorComponent(this, &Perception, "Perception");

	Sight = CreateDefaultSubobject<UAISenseConfig_Sight>("Sight");
	Sight->SightRadius = 600.0f;
	Sight->LoseSightRadius = 800.0f;
	Sight->PeripheralVisionAngleDegrees = 90.0f;
	Sight->SetMaxAge(2.0f);
	Sight->DetectionByAffiliation.bDetectEnemies = true;
	Sight->DetectionByAffiliation.bDetectFriendlies = false;
	Sight->DetectionByAffiliation.bDetectNeutrals = false;

	Perception->ConfigureSense(*Sight);
	Perception->SetDominantSense(*Sight->GetSenseImplementation());
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	OwnerEnemy = Cast<AEnemy>(InPawn);
	UBlackboardComponent* bb = Blackboard.Get();
	UseBlackboard(OwnerEnemy->GetBehaviorTree()->BlackboardAsset, bb);

	SetGenericTeamId(OwnerEnemy->GetGenericTeamId());
	Perception->OnPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnPerceptionUpdated);

	RunBehaviorTree(OwnerEnemy->GetBehaviorTree());

	Behavior->SetBlackboard(Blackboard);
}

void AEnemyAIController::OnUnPossess()
{
	Super::OnUnPossess();

	Perception->OnPerceptionUpdated.Clear();
}

float AEnemyAIController::GetSightRadius()
{
	return Sight->SightRadius;
}

void AEnemyAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	// create player array object
	TArray<AActor*> actors;
	Perception->GetCurrentlyPerceivedActors(nullptr, actors);

	TArray<ADungeonCharacter*> players;
	for (AActor* actor : actors)
	{
		ADungeonCharacter* player = Cast<ADungeonCharacter>(actor);
		if (!player)continue;
		players.Add(player);
	}

	// already exsist?
	UBlackBoardPlayerArrayObject* obj = Cast<UBlackBoardPlayerArrayObject>(Blackboard->GetValueAsObject(Behavior->GetPerceptedPlayersKey()));
	if (!obj)obj = NewObject<UBlackBoardPlayerArrayObject>();

	// check failed
	if (!obj)
	{
		CLog::Print("AEnemyAIController::OnPerceptionUpdated NewObject Failed", -1, 10, FColor::Red);
		return;
	}

	// set obj
	obj->SetPlayers(players);
	Blackboard->SetValueAsObject(Behavior->GetPerceptedPlayersKey(), obj);
}

void AEnemyAIController::StopLogic(FString Reason)
{
	GetBrainComponent()->StopLogic(Reason);
}

void AEnemyAIController::RestartLogic()
{
	GetBrainComponent()->RestartLogic();
}
