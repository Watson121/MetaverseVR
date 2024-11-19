// Fill out your copyright notice in the Description page of Project Settings.


#include "BoatControllerComponent.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"

// Sets default values for this component's properties
UBoatControllerComponent::UBoatControllerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBoatControllerComponent::BeginPlay()
{
	Super::BeginPlay();
	MeshComp = Cast<UStaticMeshComponent>(GetOwner()->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	
}


// Called every frame
void UBoatControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (MeshComp) {
		ApplyBuoyancyAndMovement();
	}

	// ...
}

void UBoatControllerComponent::SetInputs(float forward, float turn)
{
	forwardInput = forward;
	turnInput = turn;
}

void UBoatControllerComponent::ApplyBuoyancyAndMovement()
{

	if (!MeshComp) {
		return;
	}


	FVector currentVelocity = MeshComp->GetComponentVelocity();
	FVector rightDirection = GetOwner()->GetActorRightVector();
	FVector forwardDirection = GetOwner()->GetActorForwardVector();

	// Applying Forward Movement
	ApplyForwardMovement(forwardDirection, currentVelocity);

	// Calculate buoyancy forces based on submerged points
	float maxBuoyantForce = CalculateMaxBuoyantForce();
	ApplyBuoyancy(maxBuoyantForce);

	// Applying Damping
	ApplyDamping();

	// Apply Corrective Force and Damping
	FVector correctiveForce = ApplyCorrectiveForce(GetOwner()->GetActorLocation(), MeshComp->GetComponentVelocity());
	MeshComp->AddForce(correctiveForce);

	// Apply Angular Damping to Reduce Rocking
	FVector angularVelocity = MeshComp->GetPhysicsAngularVelocityInDegrees();
	FVector angularDampingForce = -angularVelocity * angularDampingCoefficient;
	MeshComp->AddTorqueInDegrees(angularDampingForce);

	// Applying Turning Movement
	ApplyTurningMovement(currentVelocity);

	// Applying Laterial Damping
	ApplyLaterialDamping(currentVelocity, forwardDirection, rightDirection);
}

void UBoatControllerComponent::ApplyForwardMovement(const FVector& forwardDirection, const FVector& velocity)
{

	float currentForwardSpeed = FVector::DotProduct(velocity, forwardDirection);

	UE_LOG(LogTemp, Warning, TEXT("Forward Speed: %f"), currentForwardSpeed);

	if (FMath::Abs(forwardInput) != 0.0f) {

		if (FMath::Abs(currentForwardSpeed) > maxForwardSpeed) {
			forwardInput = (maxForwardSpeed / FMath::Max(1.0f, FMath::Abs(currentForwardSpeed))) * forwardInput;
		}

		// Applying Forward Force
		FVector forwardForce = forwardDirection * forwardInput * (forwardInput == 1.0f ? forwardForceMultiplier : reverseForceMultiplier);

		MeshComp->AddForce(forwardForce, TEXT("None"), true);
	}
	else if (FMath::Abs(forwardInput) == 0.0f) {
		FVector decelerationForce = -velocity.GetSafeNormal() * 200.0f * forwardForceMultiplier;
		MeshComp->AddForce(decelerationForce, TEXT("None"), false);
	}
	
}

void UBoatControllerComponent::ApplyTurningMovement(const FVector& velocity)
{
	if (velocity.Size() == 0) {
		return;
	}

	// Calculating Turning Torque
	float turnTorque = turnInput * turnTorqueMultiplier;
	
	// Clamping Angular Velocity to a max turning speed
	FVector angularVelocity = MeshComp->GetPhysicsAngularVelocityInRadians();

	// Applying the Turning Torque
	if (FMath::Abs(turnInput) != 0.0f) {
		MeshComp->AddTorqueInRadians(FVector(0, 0, turnTorque), TEXT("None"), true);
	}
	else {
		FVector decelartionTorque = -angularVelocity.GetSafeNormal() * 10.0f * turnTorqueMultiplier;
		MeshComp->AddTorqueInDegrees(decelartionTorque, TEXT("None"), true);
	}

	if (FMath::Abs(angularVelocity.Z) > maxTurningSpeed) {

		//Clamping Angular Velocity
		angularVelocity.Z = FMath::Clamp(angularVelocity.Z, -maxTurningSpeed, maxTurningSpeed);
		MeshComp->SetPhysicsAngularVelocityInRadians(angularVelocity);

	}
}

void UBoatControllerComponent::ApplyBuoyancy(float MaxBuoyantForce)
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

float UBoatControllerComponent::CalculateMaxBuoyantForce()
{
	return fluidDensity * gravity * submergedVolume;
}

FVector UBoatControllerComponent::ApplyCorrectiveForce(FVector ActorLocation, FVector Velocity)
{
	float currentHeight = ActorLocation.Z - waterLevel;
	float heightDifference = currentHeight;

	FVector correctiveForce = FVector(0, 0, -heightDifference * correctiveForceMultiplier);
	return correctiveForce;
}

void UBoatControllerComponent::ApplyDamping()
{
	FVector Velocity = MeshComp->GetComponentVelocity();
	FVector VerticalVelocity = FVector(0, 0, FVector::DotProduct(Velocity, FVector(0, 0, 1)));

	// Damping force to reduce vertical oscillations
	FVector DampingForce = -VerticalVelocity * dampingCoefficient;
	MeshComp->AddForce(DampingForce); // Apply damping in the Z direction to stop bouncing

}

void UBoatControllerComponent::ApplyLaterialDamping(const FVector& velocity, const FVector& forwardDirection, const FVector& rightDirection)
{

	float lateralSpeed = FVector::DotProduct(velocity, rightDirection);

	FVector lateralDampingForce = -lateralSpeed * rightDirection * laterialDampingMultiplier;

	MeshComp->AddForce(lateralDampingForce);

}

float UBoatControllerComponent::CalculateSubmersionDepth(const FVector& WorldLocation, float Radius)
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

