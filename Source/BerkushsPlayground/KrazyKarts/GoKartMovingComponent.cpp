// 2024'te R. Berk Sozgen tarafindan gelistirilmeye baslanmistir.


#include "GoKartMovingComponent.h"

#include "GameFramework/GameStateBase.h"

// Sets default values for this component's properties
UGoKartMovingComponent::UGoKartMovingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	UWorld* MyOwnerWorld = (GetOwner() ? GetOwner()->GetWorld() : nullptr);

	if (MyOwnerWorld!= nullptr) RegisterComponentWithWorld(MyOwnerWorld);

	// ...
}


// Called when the game starts
void UGoKartMovingComponent::BeginPlay()
{
	Super::BeginPlay();
	
	GameState = GetWorld()->GetGameState();
	if (GameState==nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Game State is null"));
	}

	bIsOwnerLocallyControlled = Cast<APawn>(GetOwner())->IsLocallyControlled();
}


// Called every frame
void UGoKartMovingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwnerRole() == ROLE_AutonomousProxy || bIsOwnerLocallyControlled)
	{
		LastMove = CreateMove(DeltaTime);
		SimulateMove(LastMove);
	}
}

void UGoKartMovingComponent::SimulateMove(const FGoKartMove& Move)
{
	FVector Force =GetOwner()->GetActorForwardVector() * MaxDrivingForce * Move.Throttle;

	Force+=GetAirResistance();
	Force+=GetRollingResistance();
	
	FVector Acceleration = Force / Mass;

	Velocity = Velocity + Acceleration * Move.DeltaTime;
	
	ApplyRotation(Move.DeltaTime, Move.SteeringThrow);

	UpdateLocationFromVelocity(Move.DeltaTime);
}

FGoKartMove UGoKartMovingComponent::CreateMove(float DeltaTime)
{
	FGoKartMove Move;
	Move.DeltaTime = DeltaTime;
	Move.SteeringThrow = SteeringThrow;
	Move.Throttle = Throttle;
	Move.Time = GameState->GetServerWorldTimeSeconds();

	return Move;
}

FVector UGoKartMovingComponent::GetRollingResistance()
{
	float AccelerationDueToGravity = - GetWorld()->GetGravityZ() / 100.f;
	float NormalForce = Mass * AccelerationDueToGravity;
	return - Velocity.GetSafeNormal() * RollingResistanceCoefficient * NormalForce;
}

void UGoKartMovingComponent::ApplyRotation(float DeltaTime, float _SteeringThrow)
{
	float DeltaLocation = FVector::DotProduct(GetOwner()->GetActorForwardVector(), Velocity) * DeltaTime;
	float RotationAngle = DeltaLocation / MinTurningRadius * _SteeringThrow;
	FQuat RotationDelta(GetOwner()->GetActorUpVector(), RotationAngle);

	Velocity = RotationDelta.RotateVector(Velocity);
	
	GetOwner()->AddActorWorldRotation(RotationDelta);
}

FVector UGoKartMovingComponent::GetAirResistance()
{
	return - Velocity.GetSafeNormal() * Velocity.SizeSquared() * DragCoefficient;
}

void UGoKartMovingComponent::UpdateLocationFromVelocity(float DeltaTime)
{
	FVector Translation = Velocity * 100.f * DeltaTime;

	FHitResult Hit;
	GetOwner()->AddActorWorldOffset(Translation, true, &Hit);
	if (Hit.IsValidBlockingHit())
	{
		Velocity = FVector::Zero();
	}
}