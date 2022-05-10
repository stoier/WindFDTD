/*
  ==============================================================================

    WindFDTDVoice.h
    Created: 6 Apr 2022 3:53:09pm
    Author:  Benjamin Støier

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/

struct WindFDTDSound   : public juce::SynthesiserSound
{
public:
    bool appliesToNote    (int) override        { return true; }
    bool appliesToChannel (int) override        { return true; }
};

//==============================================================================
struct WindFDTDVoice  : public juce::SynthesiserVoice
{
public:
    WindFDTDVoice (double k); // initialise the model with the time step
    ~WindFDTDVoice() override;
    
    bool canPlaySound (juce::SynthesiserSound* sound) override;
    void startNote (int midiNoteNumber, float velocity,
                    juce::SynthesiserSound *sound, int currentPitchWheelPosition) override;

    void stopNote (float velocity, bool allowTailOff) override;
    void pitchWheelMoved (int newPitchEheelValue) override;
    void controllerMoved (int controllerNumber, int newControllerValue) override;
    void renderNextBlock (juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override;
    

    void getAudioInput(double input);
    // Function containing the update equation.
    void calculateScheme();
    
    void calculateBoreShape();
    
    void calculateOutputFilter();
    
    // Update the states by performing a pointer switch.
    void updateStates();
    
    //void updateParameters(const double cylinderLengthToSet, const double cylinderRadiusToSet, const double bellLengthToSet, const double bellRadiusToSet, const int bellGrowth, const double reedMassToSet, const double reedWidthToSet, const double pressure);
    
    void updateParameters(const double boreScaleToSet, const double cylinderRadiusToSet, const double bellCylinderRatio, const double bellRadiusToSet, const int bellGrowth, const double reedMassToSet, const double reedWidthToSet, const double pressure, const double attack, const double decay, const double sustain, const double release, const double pressureMultToSet, const double vibRateToSet, const double vibAmountToSet);
    
    void initParameters();
    
    void getLength(double boreScale, double cylinderBellRatio);
    
    void setADSR(double sampleRate);
    
    // Function to visualise the state of the system.
    //Path visualiseState (Graphics& g);
    
    // Function to excite the system.
    //void excite (double excitationLoc);
    
    // Get the output at a specified ratio of the length of the system
    //float getOutput (float outRatio) { return u[1][(int)floor(outRatio * N)]; };
    
    // Limiter to keep the output signal between -1 and 1. Arguments are: input value, minimum value and maximum value
    float limit (float val, float min, float max);
    
    int sgn(double val) {
       return (0 < val) - (val < 0);
   }
private:
    
    // Variables
    double k; // Time step (in s)
    double c; // Wave speed (in m/s)
    double h; // Grid spacing (in m)
    double cL; // Length of cyllinder (in m)
    double bL; // Length of bell (in m)
    double L; // Total length (in m)
    double cR; // Radius of cyllinder (in m)
    double bR; // Radius of bell (in m)
    double bC; // Curve of bell (1 = conical, 2 = exponential, 3 = logarithmic)
    double lambda; // Courant number squared to be used in the update equation
    double rho; // Air density (in Kg/m^3)
    double H_0; // Equilibrium seperation (in m)
    double w; // Effective lip-reed width (in m)
    double sR; //
    double K; // Lip stiffness
    double M; // Mass of lip reed
    double R; // Damping coefficient
    double omegaR; // Angular frequency of lip reed
    double sigmaR; // Loss parameter
    double alphaR; //
    double betaR; //
    double xiR; //
    double uB; //
    double uR; //
    double deltaP; // Pressure difference between mouth and left boundary of the acoustic tube
    double a1; //
    double a2; //
    double a3; //
    double b1; //
    double b2; //
    double c1;
    double c2;
    double c3;
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
    double level = 0;//
    double audioInput = 0;
    double nMax;
    double boreScale = 2.4;
    double cBRatio = 0.75;
    
    int dCnt = 0;

    double pMouth = 0;
    double Out = 0;
    double currentSample = 0.0;
    
    double pressureMultiply = 1;
    
    
    //double S = 1;
    double sMinus;
    double sPlus;
    double fs = getSampleRate();
    int noteNumber =0;
    
    int N; // number of intervals (number of grid points is N+1)
    
    int nC = 0;
    int nB = 0;
    //int boreCnt = 0;
    
    int shape=1;
    
    double lastInSampleOne = 0.0;
    double lastInSampleTwo = 0.0;
    double lastOutSampleOne = 0.0;
    double lastOutSampleTwo = 0.0;
    
    double lastInSampleOne24db = 0.0;
    double lastInSampleTwo24db = 0.0;
    double lastOutSampleOne24db = 0.0;
    double lastOutSampleTwo24db = 0.0;
    
    double pressureAttack = 0.01;
    double pressureDecay = 0.0;
    double pressureSustain = 1;
    double pressureRelease = 0.2;
    
    juce::ADSR pressureAdsr;
    juce::ADSR::Parameters pressureAdsrParams;
    
    double currentAngleVib = 0;
    double angleDeltaVib = 0;
    double vibRate = 0.1;
    double vibAmount = 0;
    double currentVib = 0;

    // A vector of 3 vectors saving the states of the system at n+1, n and n-1
    //std::vector<std::vector<double>> yStates;
    
    std::vector<std::vector<double>> pStates;
    
    std::vector<std::vector<double>> vStates;
    
    // Pointers to the 3 vectors in uStates. The first index indicates the time index (0 -> n+1, 1 -> n, 2 -> n-1) and the second index indicates the spatial index.
    /*
     Examples:
         u[0][3] -> u_{3}^{n+1}
         u[1][7] -> u_{7}^{n}
         u[2][50] -> u_{50}^{n-1}
     */
    std::vector<double> y;
    
    std::vector<double*> p;
    
    std::vector<double*> v;
    
    std::vector<double> sC;
    
    std::vector<double> sB;
    
    std::vector<double> S;
    //double S;
        
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WindFDTDVoice)
};
