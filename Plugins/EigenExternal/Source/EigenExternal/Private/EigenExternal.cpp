// Copyright Dominik Forkert. All Rights Reserved.

#include "EigenExternal.h"

#define LOCTEXT_NAMESPACE "FEigenExternalModule"

THIRD_PARTY_INCLUDES_START
#include <Eigen/Geometry> 
#include <unsupported/Eigen/LevenbergMarquardt>
THIRD_PARTY_INCLUDES_END

void FEigenExternalModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FEigenExternalModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

static Eigen::Vector3d FVectorToVector3d(const FVector& InVector)
{
    return Eigen::Vector3d{InVector.X, InVector.Y, InVector.Z};
}

static FVector VectorXdToFVector(const Eigen::VectorXd& InVector)
{
    return FVector{InVector.x(), InVector.y(), InVector.z()};
}

using FLMResidualFunction = FEigenExternalModule::FLMTargetPredictor::FLMResidualFunction;
/**
     * @brief  Defines the functor to be minimized by the LM-algorithm;
     * the functor uses double instead of float to prevent numerical instability
     */
struct TargetLocationFunctor : public Eigen::DenseFunctor<double>
{
    explicit TargetLocationFunctor(FLMResidualFunction& InSimulateProjectileLocation) : 
        DenseFunctor<double>(3, 3), LMResidualFunction{MoveTemp(InSimulateProjectileLocation)} {}

    // TFunction which expresses the residual in terms of UE-objects
    FLMResidualFunction LMResidualFunction;

    // Beta is the adjustable vector of the minimization problem
    // norm(ResidualVector)^2 is the error to be minimized
    int operator()(const Eigen::VectorXd& Beta, Eigen::VectorXd& ResidualVector) const
    {
        assert(x.size()==4);
        assert(fvec.size()==3);
       
        ResidualVector = FVectorToVector3d(LMResidualFunction(VectorXdToFVector(Beta.normalized()), Beta.norm()));

        return 0;
    }
};

/**
 * @brief Wrapper struct to hide Eigen's functor classes/structs from the interface header
 */
struct FLMWrapper
{
    explicit FLMWrapper(FLMResidualFunction& InComputeProjectileVelocityFuncRef) :
        DiffFunctor{TargetLocationFunctor{InComputeProjectileVelocityFuncRef}} {}

    // TargetLocationFunctor must be initialized with a valid InComputeProjectileVelocityFuncRef
    Eigen::NumericalDiff<TargetLocationFunctor> DiffFunctor;
    Eigen::LevenbergMarquardt<Eigen::NumericalDiff<TargetLocationFunctor>> Solver{DiffFunctor};
};

FEigenExternalModule::FLMTargetPredictor::FLMTargetPredictor() = default;
FEigenExternalModule::FLMTargetPredictor::~FLMTargetPredictor() = default;

void FEigenExternalModule::FLMTargetPredictor::InitializeLMTargetPredictor(
    FLMResidualFunction& InComputeProjectileVelocityFunc,
    const uint32 MaxFunctionEvaluations,
    const double ResidualErrorTolerance
    )
{
    LMWrapper = MakeUnique<FLMWrapper>(InComputeProjectileVelocityFunc);
    LMWrapper->Solver.setMaxfev(MaxFunctionEvaluations);
    LMWrapper->Solver.setFtol(ResidualErrorTolerance);
}

TPair<FVector, double> FEigenExternalModule::FLMTargetPredictor::LMPredictTargetLocation(
    const FVector& CurrentProjectileLocation,
    const double CurrentProjectileSpeed,
    const FVector& PredictedTargetLocationGuess) const
{
    check(LMWrapper.IsValid());
    
    // guess for the starting value of Beta: 
    const FVector ProjectileToTargetGuess{PredictedTargetLocationGuess - CurrentProjectileLocation};
    // guess for the time it takes the projectile to reach the target
    const double TimeGuess{ProjectileToTargetGuess.Length() / CurrentProjectileSpeed};

    Eigen::VectorXd Beta{FVectorToVector3d(ProjectileToTargetGuess.GetUnsafeNormal() * TimeGuess)};
    
    const int Status = LMWrapper->Solver.minimize(Beta);
    
    FVector PredictedUnitDirFromProjectileToTarget{VectorXdToFVector(Beta.normalized())};
    double PredictedTime{Beta.norm()};

#if WITH_EDITOR
    const int ComputationInfo = LMWrapper->Solver.info();
    const int Iterations = LMWrapper->Solver.iterations();
    const int NumberOfFunctionEvaluations = LMWrapper->Solver.nfev();
    GEngine->AddOnScreenDebugMessage(42, 1.f, FColor::White,
        FString::Printf(
            TEXT("LM-algorithm: PredictedTime: %f; Status: %i; Iterations: %i; NumberOfFunctionEvaluations: %i; ComputationInfo: %i "),
            PredictedTime, Status, Iterations, NumberOfFunctionEvaluations, ComputationInfo
            )
    );
#endif
    
    return {PredictedUnitDirFromProjectileToTarget, PredictedTime};
}

#undef LOCTEXT_NAMESPACE
IMPLEMENT_MODULE(FEigenExternalModule, EigenExternal)
