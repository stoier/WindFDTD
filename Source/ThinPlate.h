/*
  ==============================================================================

    ThinPlate.h
    Created: 21 Sep 2022 3:33:46pm
    Author:  Benjamin Støier

  ==============================================================================
*/


#pragma once

#include <JuceHeader.h>



class ThinPlate  : public juce::Component
{
public:
    ThinPlate (double k); // initialise the model with the time step
    ~ThinPlate() override;
    
void initParameters();

void updateParameters(const double sig0ToSet, const double sig1ToSet, const double LxToSet, const double LyToSet, const double excXToSet, const double excYToSet, const double lisXToSet, const double lisYToSet, const double thicknessToSet, const double excFToSet, const double excTToSet, const double vBToSet, const double fBToSet, const double aToSet, const int excTypeId, const double  bAtt1ToSet, const double bDec1ToSet, const double  bSus1ToSet, const double bRel1ToSet, const double FBEnv1ToSet, const double vBEnv1ToSet, const double lfoRateToSet, const double xPosModToSet, const double yPosModToSet, const int numStringsToSet, const double sLenToSet, const double sPosSpreadToSet, const double sAvgTenToSet, const double sTenDiffToSet, const double sRadToSet, const double sSig0ToSet, const double cylinderLengthToSet, const double cylinderRadiusToSet, const double bellLengthToSet, const double bellRadiusToSet, const int bellGrowth, bool tubeConnToSet, bool springConnToSet);
    
void updatePlateMaterial(int plateMaterialToSet);
    
void getSampleRate(double fsToSet);

void calculateScheme();
    
void plateHit();
    
void startBow();
    
void endBow();

void updateStates();

//void updateStringStates();

float getOutput()
{
  
    switch (excType) {
        case Mallet:
            return (u[static_cast <int> (floor(0.5*Nx))][static_cast <int> (floor(0.5*Ny))]+stringOut+tubeOut*0.00001f)*0.000001;
            break;
        case Bow:
            return (u[static_cast <int> (floor(0.5*Nx))][static_cast <int> (floor(0.5*Ny))]+stringOut+tubeOut*0.00001f)*0.0001;
            break;
    }
    
};
    
float interpolation(std::vector<double>* u, int excXidx, int excYidx, double alphaX, double alphaY)
{
    if (excXidx < 1)
    {
        excXidx = 1;
    }
    if (excYidx < 1)
    {
        excYidx = 1;
    }
    
    if (excXidx > Nx-2)
    {
        excXidx = Nx-2;
    }
    if (excYidx > Ny-2)
    {
        excYidx = Ny-2;
    }
    return (1.0 - alphaX) * (1.0 - alphaY) * u[excXidx][excYidx]+(1.0-alphaX)*alphaY*u[excXidx][excYidx+1]+alphaX*(1-alphaY)*u[excXidx+1][excYidx]+alphaX*alphaY*u[excXidx+1][excYidx+1];
}
  
void setADSR(double sampleRate);
    
void calculateBoreShape();
    
private:
    enum ExcitationType
    {
        Mallet,
        Bow
    };
    
    juce::ADSR adsr1;
    juce::ADSR::Parameters adsr1Params;

    // Variables
    double k; // Time step (in s)
    double c; // Wave speed (in m/s)
    double h; // Grid spacing (in m)
    double Lx; // Length of x dimension(in m)
    double Ly; // Length of y dimension(in m)
    
    double T60;
    double sigma0; // frequency independent damping
    double sigma1; //frequency dependent damping
    double rho; //material density
    double H; //plate thickness
    double E; //Young's modulus
    double nu; //Poisson's ratio
    
    double D; // stifness coefficient
    double kappa; //stifness parameter
    double mu;
    double muSq;
    double S;
    double maxForce;
    double excTime;
    double excXpos;
    double excYpos;
    double excXposRatio;
    double excYposRatio;
    double lisXpos;
    double lisYpos;
    double hx;
    double hy;
    double lambda;
    int excXidx;
    int excYidx;
    double b;
    double a;
    double vB, FB, vRel, vRelPrev, eps, tol;
    
    double alphaX;
    double alphaY;
    double J;
    double malletForce;
    
    double angleDeltaLFO;
    double currentXMod;
    double currentYMod;
    double currentAngleLFO;
    double lfoRate;
    
    double xPosMod;
    double yPosMod;
    
    int n;
    int plateMaterial;
    
    int numStrings;
    
    double K1, K3, R; // Connection parameters
    std::vector<double> connXPos, connXPos2; //Connection posisistion (contenios domain)
    double connYPos, connYPos2;
    std::vector<int> lcP, lcP2, lcS, lcS2; //Connection posistions (discrete domain)
    double mcP, mcP2;
    double connSPos, connSPos2;
    std::vector<double> alphaConnS, alphaConnX, alphaConnY, alphaConnS2, alphaConnX2, alphaConnY2;
    std::vector<double> connF; // connection force
    std::vector<double> eta, etaPrev, etaNext; //Connection distance
    std::vector<double> rPlus, rMinus;
    std::vector<double> stringConnTerm;
    double plateConnTerm, tubeConnTerm;
    
    bool stringConn, tubeConn;
    
    int Nx; // grid steps (x)
    int Ny; // grid steps (y)
    int N; // grid steps (total)
    
    ExcitationType excType;
    
    double fs; //sample rate
    double t;
    double t0;
    
    double FBEnv1, vBEnv1;
    double bAtt1, bDec1, bSus1, bRel1;
    double nextAdsr1;
    bool isBowing, bowEnd;
    
    double excitation;
    std::vector<double>* uNext;
    std::vector<double>* u; // state pointers
    std::vector<double>* uPrev;
    std::vector<std::vector<std::vector<double>>> uStates;
    
    //String parameters
    double rhoS; //Density
    double rS; //Radius
    double AS; // Cross-sectional area
    double ES; // Young's modulus
    double IS; //Area moment of inertia
    double sigma0S, sigma1S; // frequency-independent damping and frequency-dependent damping
    double kappaSSq; // Stiffness term
    double As;
    std::vector<double> TS;
    double TavgS;
    double TDiffS;
    //double cSSq;
    double LS; //Length
    double sPosSpread;
    //double NS;
    //double hS;
    //double lambdaSSq;
    //double muSSq;
    //std::vector<double> LS;
    std::vector<double> NS;
    double NSMax;
    std::vector<double> cSSq;
    std::vector<double> hS;
    std::vector<double> lambdaSSq;
    std::vector<double> muSSq;
    std::vector<double> uS1, uS2, uS3;
    double stringOut;
    int stringOutIdx;
    
    std::vector<double>* uStringPrev;
    std::vector<double>* uString;
    std::vector<double>* uStringNext;
    std::vector<std::vector<std::vector<double>>> uStringStates;
    bool firstHit;

    
    // Tube parameters:
    double cLT; // Length of cyllinder (in m)
    double bLT; // Length of bell (in m)
    double LT;
    double cRT; // Radius of cyllinder (in m)
    double bRT; // Radius of bell (in m)
    double bCT; // Curve of bell (1 = conical, 2 = exponential, 3 = logarithmic)
    double lambdaT; // Courant number squared to be used in the update equation
    int nCT;
    int nBT;
    int NT;
    double hT;
    double cT;
    double rhoT;
    double R1;
    double R2;
    double Lr;
    double Cr;
    double zeta1; //
    double zeta2; //
    double zeta3; //
    double zeta4; //
    double vInt; //
    double pInt; //
    std::vector<double> sC;
    std::vector<double> sB;
    std::vector<double> ST;
    double sMinus;
    double sPlus;
    int shape=1;
    std::vector<double*> p;
    std::vector<double*> v;
    std::vector<std::vector<double>> pStates;
    std::vector<std::vector<double>> vStates;
    double connFT; // connection force
    double etaT, etaPrevT, etaNextT; //Connection distance
    double rPlusT, rMinusT;
    double connXPosT, connYPosT, connTPos;
    int lcPT, mcPT, lcT;
    double tubeOut;
    
    bool springConn;
    

    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ThinPlate)
};


