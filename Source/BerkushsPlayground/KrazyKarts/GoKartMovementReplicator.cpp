// 2024'te R. Berk Sozgen tarafindan gelistirilmeye baslanmistir.


#include "GoKartMovementReplicator.h"

#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UGoKartMovementReplicator::UGoKartMovementReplicator()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	UWorld* MyOwnerWorld = (GetOwner() ? GetOwner()->GetWorld() : nullptr);

	if (MyOwnerWorld!= nullptr) RegisterComponentWithWorld(MyOwnerWorld);

	//SetIsReplicated(true); //Build tool agliyor
}


// Called when the game starts
void UGoKartMovementReplicator::BeginPlay()
{
	Super::BeginPlay();

	MovingComponent = GetOwner()->FindComponentByClass<UGoKartMovingComponent>();
	bIsOwnerLocallyControlled = Cast<APawn>(GetOwner())->IsLocallyControlled();
}

// Called every frame
void UGoKartMovementReplicator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (MovingComponent == nullptr) return;

	FGoKartMove LastMove = MovingComponent->GetLastMove();
	
	//They are client's own pawn /*Bunlari switch yapisina cevircem*/
	if (GetOwnerRole() == ROLE_AutonomousProxy)
	{
		UnacknowledgedMoves.Add(LastMove);
		Server_SendMove(LastMove);
	}
	//We are the server and in control of the pawn
	if (bIsOwnerLocallyControlled)//GetRemoteRole() == ROLE_SimulatedProxy) Unreal editorde ve buildden sonra bunun farkini yapamiyormus //iyice 31 burasi IsLocallyControlled gelmiyor
	{
		UpdateServerState(LastMove);
	}
	//They are other clients pawm
	if (GetOwnerRole() == ROLE_SimulatedProxy)
	{
		ClientTick(DeltaTime);
	}
}

void UGoKartMovementReplicator::UpdateServerState(const FGoKartMove& Move)
{
	ServerState.LastMove = Move;
	ServerState.Transform = GetOwner()->GetActorTransform();
	ServerState.Velocity = MovingComponent->GetVelocity();
}

void UGoKartMovementReplicator::ClientTick(float DeltaTime)
{
	ClientTimeSinceUpdate += DeltaTime;

	if (ClientTimeBetweenLastUpdates < KINDA_SMALL_NUMBER) return;
	if (MovingComponent == nullptr) return;

	float LerpRatio = ClientTimeSinceUpdate / ClientTimeBetweenLastUpdates;

	FHermiteCubicSpline Spline = CreateSpline();
	InterpolateLocation(Spline, LerpRatio);
	InterpolateVelocity(Spline, LerpRatio);
	InterpolateRotation(LerpRatio);
}

FHermiteCubicSpline UGoKartMovementReplicator::CreateSpline()
{
	FHermiteCubicSpline Spline;
	Spline.TargetLocation = ServerState.Transform.GetLocation();
	Spline.StartLocation = ClientStartTransform.GetLocation();
	Spline.StartDerivative = ClientStartVelocity * VelocityToDerivative(); //egim demek
	Spline.TargetDerivative = ServerState.Velocity * VelocityToDerivative(); //Hedef egim, kabaca hiz vektoru.
	return Spline;
}

void UGoKartMovementReplicator::InterpolateLocation(const FHermiteCubicSpline& Spline, float LerpRatio)
{
	FVector NewLocation = Spline.InterpolateLocation(LerpRatio);
	if (MeshOffsetRoot!=nullptr)
	{
		MeshOffsetRoot->SetWorldLocation(NewLocation);
	}
	/* //Bunun da orijinal hali bu, kalani sil collisionlari duzeltmek istersen
	FVector NewLocation = Spline.InterpolateLocation(LerpRatio);
	GetOwner()->SetActorLocation(NewLocation);
	*/
}

void UGoKartMovementReplicator::InterpolateVelocity(const FHermiteCubicSpline& Spline, float LerpRatio)
{
	FVector NewDerivative = Spline.InterpolateDerivative(LerpRatio);
	FVector NewVelocity = NewDerivative / VelocityToDerivative();
	MovingComponent->SetVelocity(NewVelocity);
}

void UGoKartMovementReplicator::InterpolateRotation(float LerpRatio)
{
	FQuat TargetRotation = ServerState.Transform.GetRotation();
	FQuat StartRotation = ClientStartTransform.GetRotation();
	FQuat NewRotation = FQuat::Slerp(StartRotation, TargetRotation, LerpRatio);

	if (MeshOffsetRoot!=nullptr)
	{
		MeshOffsetRoot->SetWorldRotation(NewRotation);
	}
	//GetOwner()->SetActorRotation(NewRotation); //Bunun da orijinal hali bu duzeltmek icin ustteki if blogunu sil
}

float UGoKartMovementReplicator::VelocityToDerivative()
{
	return ClientTimeBetweenLastUpdates * 100; //bu sadece kod toplu gorunsun diye, 100 ile carpma nedeni m=cm cevrimi
}

void UGoKartMovementReplicator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UGoKartMovementReplicator, ServerState);
}

void UGoKartMovementReplicator::OnRep_ServerState()
{
	switch (GetOwnerRole())
	{
		case ROLE_AutonomousProxy:
			AutonomousProxy_OnRep_ServerState();
			break;
		case ROLE_SimulatedProxy:
			SimulatedProxy_OnRep_ServerState();
			break;
		default:
			break;
	}
}

void UGoKartMovementReplicator::AutonomousProxy_OnRep_ServerState()
{
	if (MovingComponent == nullptr) return;
	
	GetOwner()->SetActorTransform(ServerState.Transform);
	MovingComponent->SetVelocity(ServerState.Velocity);

	ClearAcknowledgeMoves(ServerState.LastMove);

	for (const FGoKartMove& Move : UnacknowledgedMoves)
	{
		MovingComponent->SimulateMove(Move);
	}
}

void UGoKartMovementReplicator::SimulatedProxy_OnRep_ServerState()
{
	if (MovingComponent == nullptr) return;
	
	ClientTimeBetweenLastUpdates = ClientTimeSinceUpdate;
	ClientTimeSinceUpdate = 0;

	if (MeshOffsetRoot!=nullptr)
	{
		ClientStartTransform.SetLocation(MeshOffsetRoot->GetComponentLocation());
		ClientStartTransform.SetRotation(MeshOffsetRoot->GetComponentQuat());
	}
	ClientStartVelocity = MovingComponent->GetVelocity();
/* //Orijinal hali bu, alttakini ve bi ustteki iften sonraki kismi sil duzeltmek icin
	ClientStartTransform = GetOwner()->GetActorTransform();
	ClientStartVelocity = MovingComponent->GetVelocity();
*/

	GetOwner()->SetActorTransform(ServerState.Transform); //its for mesh offset, actually i can delete it
}

void UGoKartMovementReplicator::ClearAcknowledgeMoves(FGoKartMove LastMove)
{
	TArray<FGoKartMove> NewMoves;

	for (const FGoKartMove& Move : UnacknowledgedMoves)
	{
		if (Move.Time > LastMove.Time)
		{
			NewMoves.Add(Move);
		}
	}

	UnacknowledgedMoves = NewMoves;
}

void UGoKartMovementReplicator::Server_SendMove_Implementation(FGoKartMove Move)
{
	if (MovingComponent == nullptr) return;

	ClientSimulatedTime+=Move.DeltaTime;
	
	MovingComponent->SimulateMove(Move);

	UpdateServerState(Move);
}

bool UGoKartMovementReplicator::Server_SendMove_Validate(FGoKartMove Move)
{
	float ProposedTime = ClientSimulatedTime + Move.DeltaTime;
	bool ClientNotRunningAhead = ProposedTime < GetWorld()->TimeSeconds; //GameStateden server time almak mantikli olabilir
	if (!ClientNotRunningAhead)
	{
		UE_LOG(LogTemp, Error, TEXT("Client Running Ahead!"));
		return false;
	}
	if (FMath::Abs(Move.Throttle) > 1.f || FMath::Abs(Move.SteeringThrow) > 1.f)
	{
		UE_LOG(LogTemp, Error, TEXT("Recieved Invalid Move!"));
		return false;
	}
	return true;
}