// Copyright Epic Games, Inc. All Rights Reserved.

#include "EigenExternal.h"

#include<iostream>  //TODO

//#include <unsupported/Eigen/NonLinearOptimization>

#define LOCTEXT_NAMESPACE "FEigenExternalModule"

void FEigenExternalModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FEigenExternalModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}
THIRD_PARTY_INCLUDES_START

#include <stdio.h>      //TODO
#include <Eigen/Geometry> 
#include <unsupported/Eigen/LevenbergMarquardt>
THIRD_PARTY_INCLUDES_END
// This disables some useless Warnings on MSVC.
// It is intended to be done for this test only.
#include <Eigen/src/Core/util/DisableStupidWarnings.h>

using std::sqrt;

// tolerance for checking number of iterations
#define LM_EVAL_COUNT_TOL 4/3

struct lmder_functor : Eigen::DenseFunctor<double>
{
    lmder_functor(void): DenseFunctor<double>(3,3) {}
    int operator()(const Eigen::VectorXd &x, Eigen::VectorXd &fvec) const
    {
        double yaw = x[0];      //radian
        double pitch = x[1];    //radian
        double t = x[2];

        Eigen::AngleAxisd yawAngle(yaw, Eigen::Vector3d::UnitZ());
        Eigen::AngleAxisd pitchAngle(pitch, Eigen::Vector3d::UnitY());
        
        Eigen::Vector3d MDir =  yawAngle* pitchAngle * Eigen::Vector3d::UnitX();
        // MDir.normalize();
        

        double MSpeed = 1.;
        double TSpeed = 1.;
        
        Eigen::Vector3d TDir{1., 0., 0.};
        // TDir.normalize();
        
        Eigen::Vector3d MLoc{0., 0., 0.};
        Eigen::Vector3d TLoc{1.,2.,3.};
        
        assert(x.size()==4);
        assert(fvec.size()==3);

        fvec = TLoc + TDir*TSpeed*t*t - MLoc - MDir*MSpeed*t*t;
        
        return 0;
    }

    /*
    int df(const Eigen::VectorXd &x, Eigen::MatrixXd &fjac) const
    {
        double tmp1, tmp2, tmp3, tmp4;
        for (int i = 0; i < 14; i++)
        {
            tmp1 = i+1;
            tmp2 = 16 - i - 1;
            tmp3 = (i>=8)? tmp2 : tmp1;
            tmp4 = (x[1]*tmp2 + x[2]*tmp3); tmp4 = tmp4*tmp4;
            fjac(i,0) = -1;
            fjac(i,1) = tmp1*tmp2/tmp4;
            fjac(i,2) = tmp1*tmp3/tmp4;
        }
        return 0;
    }
    */
};



struct lmdif_functor : Eigen::DenseFunctor<double>
{
    lmdif_functor(void) : Eigen::DenseFunctor<double>(3,15) {}
    int operator()(const Eigen::VectorXd &x, Eigen::VectorXd &fvec) const
    {
        int i;
        double tmp1,tmp2,tmp3;
        static const double y[15]={1.4e-1,1.8e-1,2.2e-1,2.5e-1,2.9e-1,3.2e-1,3.5e-1,3.9e-1,
            3.7e-1,5.8e-1,7.3e-1,9.6e-1,1.34e0,2.1e0,4.39e0};

        assert(x.size()==3);
        assert(fvec.size()==15);
        for (i=0; i<15; i++)
        {
            tmp1 = i+1;
            tmp2 = 15 - i;
            tmp3 = tmp1;
            
            if (i >= 8) tmp3 = tmp2;
            fvec[i] = y[i] - (x[0] + tmp1/(x[1]*tmp2 + x[2]*tmp3));
        }
        return 0;
    }
};


FString testLmder1()
{
    int cinfo, info;
  //int n=4;
    const int n=3;
    

  Eigen::VectorXd x;

  /* the following starting values provide a rough fit. */
  x.setConstant(n, 4.);

  // do the computation
  // lmder_functor functor;
  // Eigen::NumericalDiff<lmder_functor> func_with_num_diff(functor);
  // Eigen::LevenbergMarquardt<Eigen::NumericalDiff<lmder_functor> > lm(func_with_num_diff);
  //Eigen::LevenbergMarquardt<lmder_functor> lm(functor);
  //  lm.minimize(x);
  // info = lm.lmder1(x);

    lmder_functor functor;
    Eigen::DenseIndex nfev = -1; // initialize to avoid maybe-uninitialized warning

    Eigen::NumericalDiff<lmder_functor> numDiff(functor);

    // Eigen::Vector4d xx{1,2,3, 4};
    // Eigen::MatrixXd JJ;
    // JJ.setConstant(3,4,0);
    
   // numDiff.df(xx, JJ);
    
    Eigen::LevenbergMarquardt<Eigen::NumericalDiff<lmder_functor> > lm(numDiff);
    lm.setMaxfev(2000);
    info = lm.minimize(x);
    cinfo = lm.info();
    nfev = lm.nfev();
    //Eigen::NumericalDiff<lmdif_functor> numDiff(functor);
    //info = Eigen::LevenbergMarquardt<lmder_functor>::lmdif1(functor, x, &nfev);

    return FString::Printf(TEXT("Prediction: %f, %f, %f; info: %i, nfev: %lld, cinfo: %i "), x[0], x[1], x[2], info, nfev, cinfo);
}

FString FEigenExternalModule::GetLMPrediction()
{
    return testLmder1();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEigenExternalModule, EigenExternal)
