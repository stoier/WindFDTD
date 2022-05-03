/*
  ==============================================================================

    WindFDTDVoice.cpp
    Created: 6 Apr 2022 3:53:09pm
    Author:  Benjamin Støier

  ==============================================================================
*/

#include <JuceHeader.h>
#include "WindFDTDVoice.h"


//==============================================================================
WindFDTDVoice::WindFDTDVoice (double kIn) : k (kIn) // <- This is an initialiser list. It initialises the member variable 'k' (in the "private" section in OneDWave.h), using the argument of the constructor 'kIn'.
{
    //******
    
    cL = 1;
    cR = 0.02;
    bL = 0.2;
    bR = 0.1;
    c=343;
    k=1/fs;
    h = c* k;
    N = floor(cL/h);
    h = cL/N;
    lambda = c*k/h;
    rho = 1.225;
    H_0 = 2.9*pow(10,-4);
    w = 0.005;
    sR = 1.46*pow(10,-5);
    R = 0.5;
    omegaR = 600;
    M = 5.37 * pow(10,-5);
    sigmaR = 5;
    alphaR = 2+pow(omegaR,2)*pow(k,2)+ sigmaR * k;
    betaR = sigmaR * k  - 2 - pow(omegaR,2)*pow(k,2);
    xiR = (2* sR * pow(k,2))/M;
    uB= 0;
    uR= 0;
    deltaP = 0;
    
    a1 = 2/k + pow(omegaR,2) * k + sigmaR;
    a2 = sR / M;
    a3 = 0;

    b1 = 0;
    b2 = (S * h) / (rho * pow(c,2) * k);
    
    c1 = 0;
    c2 = b2+(a2*sR)/a1;
    c3 = 0;
    
    R1 = rho*c;
    R2 = 0.505* rho*c;
    Lr = 0.613*rho*sqrt(S/MathConstants<double>::pi);
    Cr = 1.111* sqrt(S)/(rho*pow(c,2)*sqrt(MathConstants<double>::pi));
    
    zeta1=(2*R2*k)/(2*R1*R2*Cr+k*(R1+R2));
    zeta2 = (2*R1*R2*Cr-k*(R1+R2))/(2*R1*R2*Cr+k*(R1+R2));
    zeta3 = k/(2*Lr)+zeta1/(2*R2)+(Cr*zeta1)/k;
    zeta4 = (zeta2+1)/(2*R2)+(Cr*zeta2-Cr)/k;


    
    //******
    
        
    // Initialise vectors containing the state of the system
    //std::vector<double> y(3, 0);
    //std::vector<double> y(3, 0);
    y = std::vector<double>(3,0);
    
    sC = std::vector<double>(nC,0);
    sB = std::vector<double>(nB,0);
    
    vStates = std::vector<std::vector<double>> (2,
                                        std::vector<double>(N, 0));
    
    pStates = std::vector<std::vector<double>> (2,
                                        std::vector<double>(N+1, 0));
    
    // Initialise vector of pointers to the states
//    y.resize (3, nullptr);
    v.resize (2, nullptr);
    p.resize (2, nullptr);
    
    // Make set memory addresses to first index of the state vectors.
    //for (int i = 0; i < 3; ++i)
    //    y[i] = &yStates[i][0];
    
    y[0] = -H_0;
    y[1] = -H_0;
    y[2] = -H_0;
    
    //std::vector<double> y(3);

    
    
    for (int i = 0; i < 2; ++i)
        v[i] = &vStates[i][0];
    
    for (int i = 0; i < 2; ++i)
        p[i] = &pStates[i][0];

}

WindFDTDVoice::~WindFDTDVoice()
{
}

bool WindFDTDVoice::canPlaySound (juce::SynthesiserSound* sound)
{
    return dynamic_cast<WindFDTDSound*> (sound) != nullptr;
}

void WindFDTDVoice::startNote (int midiNoteNumber, float velocity,
                juce::SynthesiserSound *sound, int currentPitchWheelPosition)
{
    //Apply note velocity to output level of the voice
    //level = velocity * 0.1;
    
    //Get midi note number
    noteNumber = midiNoteNumber;
    
    //Get sample rate
    fs = getSampleRate();
    
    omegaR = juce::MidiMessage::getMidiNoteInHertz(noteNumber) * 2 * MathConstants<double>::pi;
    
    //Update patameters depending on reed frequency
    alphaR = 2+pow(omegaR,2)*pow(k,2)+ sigmaR * k;
    betaR = sigmaR * k  - 2 - pow(omegaR,2)*pow(k,2);
    xiR = (2* sR * pow(k,2))/M;
    a1 = 2/k + pow(omegaR,2) * k + sigmaR;
    a2 = sR / M;
    c2 = b2+(a2*sR)/a1;
    
}

void WindFDTDVoice::stopNote (float velocity, bool allowTailOff)
{
    
    //Clear note if amplitude envelope is not active
    //if (allowTailOff)
    //        {
    //            //if (tailOff == 0.0)
    //           //    tailOff = 1.0;
    //        }
    //        else
    //        {
    clearCurrentNote();
    noteNumber = 0;
    
    //reset parameters
    uB= 0;
    uR= 0;
    deltaP = 0;
    a3 = 0;
    b1 = 0;
    c1 = 0;
    c3 = 0;
    y[0] = -H_0;
    y[1] = -H_0;
    y[2] = -H_0;
                
    //        }
}

void WindFDTDVoice::updateParameters(const double cylinderLengthToSet, const double reedMassToSet, const double reedWidthToSet)
{
    //update cylinder length and convert cm to m
    cL = cylinderLengthToSet/100;
    M = reedMassToSet/1000;
    w = reedWidthToSet/100;
    //update parameters depinding on cylinder length
    N = floor(cL/h);
    h = cL/N;
    lambda = c*k/h;
    xiR = (2* sR * pow(k,2))/M;
    a2 = sR / M;
    b2 = (S * h) / (rho * pow(c,2) * k);
    c2 = b2+(a2*sR)/a1;
    
}

void WindFDTDVoice::pitchWheelMoved (int newPitchEheelValue)
{
    
}
void WindFDTDVoice::controllerMoved (int controllerNumber, int newControllerValue)
{
    
}

void WindFDTDVoice::getAudioInput(double input)
{
    audioInput = input;
}

float WindFDTDVoice::limit (float val, float min, float max)
{
    if (val < min)
        return min;
    else if (val > max)
        return max;
    else
        return val;
}

void WindFDTDVoice::renderNextBlock(juce::AudioSampleBuffer &outputBuffer, int startSample, int numSamples)
{
    if (noteNumber != 0)
    {
        while (--numSamples >= 0)
        {
            calculateScheme();
            currentSample = limit((float) Out, -0.125, 0.125);
            //DBG(currentSample);
            
            for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                outputBuffer.addSample (i, startSample, currentSample);
            
            
            ++startSample;
            updateStates();
        }
        
    }
}

void WindFDTDVoice::calculateScheme()
{
    // Here is where you'll have to implement your update equation in a for loop (ranging from l = 1 to l < N).

    //******
    for (int l = 0; l <= N-1; l++)
    {
        v[0][l] = v[1][l]-((lambda/(rho*c)))*(p[1][l+1]-p[1][l]);
    }
    
    
    a3 = (2/pow(k,2))*(y[1]-y[2])-pow(omegaR,2)*y[2];
    b1 = S* v[0][0] + ((S*h)/(rho*pow(c,2) * k))*(pMouth - p[1][0]);
    c1 = w * ((y[1]+H_0+abs(y[1]+H_0))/2) * sqrt(2/rho);
    c3 = b1 - (a3 * sR)/a1;

    deltaP = sgn(c3) * pow(((-c1 + sqrt(pow(c1,2)+4*c2*abs(c3)))/(2*c2)),2);
    
    y[0] = (4 * y[1] + betaR * y[2] + xiR * deltaP)/alphaR;
    uB = w*((y[1]+H_0+abs(y[1]+H_0))/2)*sgn(deltaP)*sqrt(2*abs(deltaP)/rho);
    uR = (sR / (2*k)) * (y[0]-y[2]);
    
    for(int l = 1; l <= N-1; l++)
    {
        p[0][l] = p[1][l]-((rho*c*lambda)/S)*(v[0][l]*S-v[0][l-1]*S);
    }
    
    p[0][0] = p[1][0] - (rho*c*lambda)/S *(-2*(uB+uR)+2*S*v[0][0]);
    p[0][N] = (1-rho*c*lambda*zeta3)/(1+rho*c*lambda*zeta3)*p[1][N]-((2*rho*c*lambda)/(1+rho*c*lambda*zeta3))*(vInt+zeta4*pInt-(0.5*(S+S)*v[0][N-1])/S);
        
    vInt=vInt+k/Lr*0.5*(p[0][N]+p[1][N]);
    pInt= zeta1 * 0.5*(p[0][N]+p[1][N]) + zeta2 * pInt;
    
    Out = p[1][N];
    
    //calculateOutputFilter();
    
}

void WindFDTDVoice::calculateBoreShape()
{
    // if bell is linear
    if(shape == 1)
    {
        auto r = cR;
        auto rGrowth = (bR - cR) / nB;
        for (int i = 0; i<= nB; i++)
        {
            r = r + rGrowth;
            sB[i] = juce::MathConstants<double>::pi*pow(r,2);
        }
    }
    // if bell is exponential
    else if(shape == 2)
    {
        auto r = cR;
        auto rGrowth = exp(log(bR-cR)/nB);
        for (int i = 0; i<= nB; i++)
        {
            r = cR * pow(rGrowth,i);
            sB[i] = juce::MathConstants<double>::pi*pow(r,2);
        }
    }
    // if bell is logarithmic
    else if(shape == 3)
    {
        auto r = cR;
        auto rGrowth = (bR-cR)/log(nB);
        for (int i = 0; i<= nB; i++)
        {
            r = cR + rGrowth * log(i);
            sB[i] = juce::MathConstants<double>::pi*pow(r,2);
        }
    }
}

void WindFDTDVoice::calculateOutputFilter()
{
    // Calculate filter cutoff divided by sample rate
    auto wF = (2.0*juce::MathConstants<double>::pi*(pow(c,2)*juce::MathConstants<double>::pi/S))/ fs;
    
    //Calculate alpha and a coefficients for filter
    auto alphaF = std::sin(wF)/(2.0*0.707);
    auto fA0 = 1.0 + alphaF;
    auto fA1 = (-2.0*std::cos(wF))/fA0;
    auto fA2= (1.0-alphaF)/fA0;
    
    //Calculate filter coefficients for biquad lowpass filter
    auto fB0 = ((1.0-std::cos(wF))/2.0)/fA0;
    auto fB1 = (1.0-std::cos(wF))/fA0;
    auto fB2 = ((1.0-std::cos(wF))/2.0)/fA0;

    auto filterIn = Out;
    auto filterOut= fB0*filterIn+fB1*lastInSampleOne+fB2*lastInSampleTwo-fA1*lastOutSampleOne-fA2*lastOutSampleTwo;
    
    lastInSampleTwo = lastInSampleOne;
    lastInSampleOne = filterIn;
    lastOutSampleTwo = lastOutSampleOne;
    lastOutSampleOne = filterOut;
    
    auto filterOut24dB = fB0*filterOut+fB1*lastInSampleOne24db+fB2*lastInSampleTwo24db-fA1*lastOutSampleOne24db-fA2*lastOutSampleTwo24db;
    
    lastInSampleTwo24db = lastInSampleOne24db;
    lastInSampleOne24db = filterOut;
    lastOutSampleTwo24db = lastOutSampleOne24db;
    lastOutSampleOne24db = filterOut24dB;
    
    Out = filterOut24dB;
    
}

void WindFDTDVoice::updateStates()
{
    // Here is where you'll have to implement the pointer switch.
    
    //******
    double* pTmp = p[1];
    p[1] = p[0];
    p[0] = pTmp;
    
    double* vTmp = v[1];
    v[1] = v[0];
    v[0] = vTmp;
    //double* yTmp = y[2];
    y[2] = y[1];
    y[1] = y[0];
    //y[0] = yTmp;
    //******
}
 
