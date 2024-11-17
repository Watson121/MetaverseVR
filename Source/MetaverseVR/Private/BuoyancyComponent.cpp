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
		ApplyBuoyancyAndMovement();
	}

	// ...
}

void UBuoyancyComponent::SetInputs(float forward, float turn)
{
	forwardInput = forward;
	turnInput = turn;
}

void UBuoyancyComponent::ApplyBuoyancyAndMovement()
{

	if (!MeshComp) {
		return;
	}

	// Caculating Speed and Direction
	ApplyForwardMovement();

	FVector rightDirection = GetOwner()->GetActorRightVector();



	// Applying Turning Force
	float turnTorque = turnInput * turnTorqueMultiplier;

	// Applying these forces
	
	MeshComp->AddTorqueInRadians(FVector(0, 0, turnTorque), TEXT("None"), true);

	// Calculate buoyancy forces based on submerged points
	float maxBuoyantForce = CalculateMaxBuoyantForce();
	ApplyBuoyancy(maxBuoyantForce);
	ApplyDamping();

	// Apply Corrective Force and Damping
	FVector correctiveForce = ApplyCorrectiveForce(GetOwner()->GetActorLocation(), MeshComp->GetComponentVelocity());
	MeshComp->AddForce(correctiveForce);

	// Apply Angular Damping to Reduce Rocking
	FVector angularVelocity = MeshComp->GetPhysicsAngularVelocityInDegrees();
	FVector angularDampingForce = -angularVelocity * angularDampingCoefficient;
	MeshComp->AddTorqueInDegrees(angularDampingForce);

	
}

void UBuoyancyComponent::ApplyForwardMovement()
{
	FVector forwardDirection = GetOwner()->GetActorForwardVector();
	FVector currentVelocity = MeshComp->GetComponentVelocity();
	float currentForwardSpeed = FVector::DotProduct(currentVelocity, forwardDirection);

	UE_LOG(LogTemp, Warning, TEXT("Forward Speed: %f"), currentForwardSpeed);

	if (FMath::Abs(forwardInput) != 0.0f) {

		if (FMath::Abs(currentForwardSpeed) > maxForwardSpeed) {
			forwardInput = (maxForwardSpeed / FMath::Max(1.0f, FMath::Abs(currentForwardSpeed))) * forwardInput;
		}

		// Applying Forward Force
		FVector forwardForce = forwardDirection * forwardInput * forwardForceMultiplier;

		MeshComp->AddForce(forwardForce, TEXT("None"), false);
	}
	else if (FMath::Abs(forwardInput) == 0.0f) {
		FVector decelerationForce = -currentVelocity.GetSafeNormal() * 200.0f * forwardForceMultiplier;
		MeshComp->AddForce(decelerationForce, TEXT("None"), false);
	}
	
}

void UBuoyancyComponent::ApplyTurningMovement()
{
}

void UBuoyancyComponent::ApplyBuoyancy(float MaxBuoyantForce)
{
	
	FVector actorLocation = GetOwner()->GetActorLocation();
	FVector velocity = MeshComp->GetComponentVelocity();

	for (const FBuoyancyPoint& point : buoyancyPoints) {
		
		FVector worldLocation = actorLocation + GetOwner()->GetTransform().TransformVector(point.localOffset);

		// Calculate Submersion Depth
		float submersionDepth = CalculateSubmersionDepth(worldLocation, point.radius);

		if (submersionDepth > 0.0f) {

			// Calculate Buoyant Force
			float buoyantForce = fluidDensity * gravity * point.radius * submersionDepth;
			buoyantForce = FMath::Clamp(buoyantForce, 0.0f, MaxBuoyantForce); // Limiting Force

			FVector force = FVector(0, 0, buoyantForce);
			MeshComp->AddForceAtLocation(force, worldLocation); // Applying Buoyancy Force at this point 

		}

	}
}

float UBuoyancyComponent::CalculateMaxBuoyantForce()
{
	return fluidDensity * gravity * submergedVolume;
}

FVector UBuoyancyComponent::ApplyCorrectiveForce(FVector ActorLocation, FVector Velocity)
{
	float currentHeight = ActorLocation.Z - waterLevel;
	float heightDifference = currentHeight;

	FVector correctiveForce = FVector(0, 0, -heightDifference * correctiveForceMultiplier);
	return correctiveForce;
}

void UBuoyancyComponent::ApplyDamping()
{
	FVector Velocity = MeshComp->GetComponentVelocity();
	FVector VerticalVelocity = FVector(0, 0, FVector::DotProduct(Velocity, FVector(0, 0, 1)));

	// Damping force to reduce vertical oscillations
	FVector DampingForce = -VerticalVelocity * dampingCoefficient;
	MeshComp->AddForce(DampingForce); // Apply damping in the Z direction to stop bouncing

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

