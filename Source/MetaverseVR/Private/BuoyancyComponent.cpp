// Fill out your copyright notice in the Description page of Project Settings.


#include "BuoyancyComponent.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"

// Sets default values for this component's properties
UBuoyancyComponent::UBuoyancyComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBuoyancyComponent::BeginPlay()
{
	Super::BeginPlay();
	MeshComp = Cast<UStaticMeshComponent>(GetOwner()->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	
}


// Called every frame
void UBuoyancyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (MeshComp) {
		ApplyBuoyancy();
	}

	// ...
}

void UBuoyancyComponent::SetInputs(float forward, float turn)
{
	forwardInput = forward;
	turnInput = turn;
}

void UBuoyancyComponent::ApplyBuoyancy()
{
	if (!MeshComp) return;

	FVector ActorLocation = GetOwner()->GetActorLocation();
	FVector Velocity = MeshComp->GetComponentVelocity();

	float currentHeight = ActorLocation.Z - waterLevel;
	float heightDiffrence = currentHeight;

	// Applying Corrective Force to pull boat towards water level
	FVector correctForce = FVector(0, 0, -heightDiffrence * correctiveForceMultiplier);

	// Calculate damping force based on vertical velocity
	float VerticalVelocity = FVector::DotProduct(Velocity, FVector(0, 0, 1));
	FVector DampingForce = -FVector(0, 0, VerticalVelocity * dampingCoefficient);
	
	// Apply Drag Force to Boat's Speed
	FVector dragForce = -Velocity.GetSafeNormal() * (Velocity.Size() * dragCoefficient);

	FVector totalTorque = FVector::ZeroVector;
	FVector totalForce = FVector::ZeroVector;

	float MaxBuoyantForce = fluidDensity * gravity * 100.0f;

	for (const FBuoyancyPoint& point : buoyancyPoints) {
		
		FVector WorldLocation = ActorLocation + GetOwner()->GetTransform().TransformVector(point.localOffset);
		float submersionDepth = CalculateSubmersionDepth(WorldLocation, point.radius);

		if (submersionDepth > 0.0f) // Only apply buoyancy if the point is submerged
		{
			// Calculate buoyant force proportional to the submerged depth
			float BuoyantForce = fluidDensity * gravity * point.radius * submersionDepth;
			BuoyantForce = FMath::Clamp(BuoyantForce, 0.0f, MaxBuoyantForce);
		

			// Apply the total force (buoyancy + damping)
			FVector Force = FVector(0, 0, BuoyantForce) + DampingForce + correctForce;

			// Log for debugging
			UE_LOG(LogTemp, Warning, TEXT("BuoyantForce: %f, SubmersionDepth: %f, DampingForce: %f, CorrectiveForce: %f, DragForce: %f"),
				BuoyantForce, submersionDepth, DampingForce.Z, correctForce.Z, dragForce.Z);

			// Apply the force at the current buoyancy point
			MeshComp->AddForceAtLocation(Force, WorldLocation);

			FVector Torque = FVector::CrossProduct(WorldLocation - ActorLocation, Force);
			totalTorque += Torque;
			totalForce += Force;
		}
	}

	MeshComp->AddForce(totalForce);

	MeshComp->AddTorqueInRadians(totalTorque);

	FVector angularVelocity = MeshComp->GetPhysicsAngularVelocityInDegrees();
	FVector angularDampingForce = -angularVelocity * angularDampingCoefficient;
	MeshComp->AddTorqueInRadians(angularDampingForce);
}

float UBuoyancyComponent::CalculateSubmersionDepth(const FVector& WorldLocation, float Radius)
{
	// Calculate the distance from the water level to the point in question
	float DistanceToSurface = waterLevel - WorldLocation.Z;

	// If the point is above the water surface, it's not submerged
	if (DistanceToSurface < -Radius) return 0.0f;

	// If the point is fully submerged, return 1
	if (DistanceToSurface >= Radius) return 1.0f;

	// Otherwise, calculate submersion depth as a fraction of the sphere's radius
	return (DistanceToSurface + Radius) / (2.0f * Radius);
}

