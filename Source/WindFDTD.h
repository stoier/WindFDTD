/*
  ==============================================================================

    WindFDTD.h
    Created: 5 Apr 2022 10:21:16pm
    Author:  Benjamin Støier

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class WindFDTD  : public juce::Component
{
public:
    WindFDTD (double k); // initialise the model with the time step
    ~WindFDTD() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    // Function containing the update equation.
    void calculateScheme();
    
    // Update the states by performing a pointer switch.
    void updateStates();
    
    // Function to visualise the state of the system.
    Path visualiseState (Graphics& g);
    
    // Function to excite the system.
    //void excite (double excitationLoc);
    
    // Get the output at a specified ratio of the length of the system
    float getOutput (float outRatio) { return u[1][(int)floor(outRatio * N)]; };
    
private:
    
    // Variables
    double k; // Time step (in s)
    double c; // Wave speed (in m/s)
    double h; // Grid spacing (in m)
    double cL; // Length of cyllinder (in m)
    double bL; // Length of bell (in m)
    double cR; // Radius of cyllinder (in m)
    double bR; // Radius of bell (in m)
    double bC; // Curve of bell (1 = conical, 2 = exponential, 3 = logarithmic)
    double lambdaSq; // Courant number squared to be used in the update equation
    double rho; //
    double H_0; //
    double w; //
    double sR; //
    double K; //
    double M; //
    double R; //
    double omega; //
    double sigmaR; //
    double alphaR; //
    double betaR; //
    double xiR; //
    double uB; //
    double uR; //
    double deltaP; //
    double a1; //
    double a2; //
    double a3; //
    double b1; //
    double b2; //
    double zeta1; //
    double zeta2; //
    double zeta3; //
    double zeta4; //
    double vInt; //
    double pInt; //

    
    int N; // number of intervals (number of grid points is N+1)

    // A vector of 3 vectors saving the states of the system at n+1, n and n-1
    std::vector<std::vector<double>> uStates;
    
    // Pointers to the 3 vectors in uStates. The first index indicates the time index (0 -> n+1, 1 -> n, 2 -> n-1) and the second index indicates the spatial index.
    /*
     Examples:
         u[0][3] -> u_{3}^{n+1}
         u[1][7] -> u_{7}^{n}
         u[2][50] -> u_{50}^{n-1}
     */
    std::vector<double*> u;
        
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WindFDTD)
};
