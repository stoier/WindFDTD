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
WindFDTDVoice::WindFDTDVoice (double kIn) : k (kIn)
{
    //******
    
    cL = 1;
    cR = 0.02;
    bL = 0.2;
    bR = 0.1;
    omegaR = 1643.84;
    initParameters();
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
    //Get sample rate
    fs = getSampleRate();
    
    //Get midi note number
    noteNumber = midiNoteNumber;
    
    omegaR = juce::MidiMessage::getMidiNoteInHertz(noteNumber) * 2 * MathConstants<double>::pi;
    
    // Start ADSR
    pressureAdsr.noteOn();
    
    // Reinitialise parameters
    initParameters();

}

void WindFDTDVoice::stopNote (float velocity, bool allowTailOff)
{
    
    pressureAdsr.noteOff();
    
    if(! pressureAdsr.isActive())
    {
        clearCurrentNote();
        noteNumber = 0;
    }
}

void WindFDTDVoice::updateParameters(const double boreScaleToSet, const double cylinderRadiusToSet, const double cylinderBellRatioToSet, const double bellRadiusToSet, const int bellGrowthToSet, const double reedMassToSet, const double reedWidthToSet, const double pressureToSet, const double attackToSet, const double decayToSet, const double sustainToSet, const double releaseToSet, const double pressureMultToSet, const double vibRateToSet, const double vibAmountToSet)
{
    boreScale = boreScaleToSet;
    cBRatio = cylinderBellRatioToSet;
    cR = cylinderRadiusToSet/100;
    bR = bellRadiusToSet/100;
    shape = bellGrowthToSet;
    pMouth = pressureToSet;
    M = reedMassToSet/1000;
    w = reedWidthToSet/1000;
    pressureAttack = attackToSet/1000;
    pressureDecay = decayToSet/1000;
    pressureSustain = sustainToSet;
    pressureRelease = releaseToSet/1000;
    
    pressureMultiply = pressureMultToSet;
    
    vibRate = vibRateToSet;
    vibAmount = vibAmountToSet;
    
    

    
}

void WindFDTDVoice::pitchWheelMoved (int newPitchEheelValue)
{
    
}
void WindFDTDVoice::controllerMoved (int controllerNumber, int newControllerValue)
{
    
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
            currentSample = limit((float) Out/30, -0.16, 0.16);
            //DBG(currentSample);
            
            for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                outputBuffer.addSample (i, startSample, currentSample);
            
            
            currentAngleVib += angleDeltaVib;
            ++startSample;
            updateStates();
        }
    }
}

void WindFDTDVoice::calculateScheme()
{
    
    //Get number of cycles pr. sample the number of cycles pr. sample for the vibrato
    auto cyclesPerSampleVib= vibRate / fs;
    
    //Update the amount that the phase angle of the vibrato needs to increment for each sample
    angleDeltaVib = cyclesPerSampleVib * 2.0 * juce::MathConstants<double>::pi;
    
    currentVib= std::sin(currentAngleVib)*vibAmount;
    
    
    if (vibAmount != 0)
    {
        auto vibMod = 440 *std::powf(2, ((noteNumber+currentVib) - 69) / 12);
        omegaR =  vibMod * 2 * MathConstants<double>::pi;;
        a1 = 2/k + pow(omegaR,2) * k + sigmaR;
        alphaR = 2+pow(omegaR,2)*pow(k,2)+ sigmaR * k;
        betaR = sigmaR * k  - 2 - pow(omegaR,2)*pow(k,2);
    }
    
    for (int l = 0; l <= N-1; l++)
    {
        v[0][l] = v[1][l]-((lambda/(rho*c)))*(p[1][l+1]-p[1][l]);
    }
    
    
    a3 = (2/pow(k,2))*(y[1]-y[2])-pow(omegaR,2)*y[2];
    b1 = S[0]* v[0][0] + ((S[0]*h)/(rho*pow(c,2) * k))*((pMouth*pressureAdsr.getNextSample()*pressureMultiply) - p[1][0]);
    c1 = w * ((y[1]+H_0+abs(y[1]+H_0))/2) * sqrt(2/rho);
    c3 = b1 - (a3 * sR)/a1;

    deltaP = sgn(c3) * pow(((-c1 + sqrt(pow(c1,2)+4*c2*abs(c3)))/(2*c2)),2);
    
    y[0] = (4 * y[1] + betaR * y[2] + xiR * deltaP)/alphaR;
    uB = w*((y[1]+H_0+abs(y[1]+H_0))/2)*sgn(deltaP)*sqrt(2*abs(deltaP)/rho);
    uR = (sR / (2*k)) * (y[0]-y[2]);
    
    for(int l = 1; l <= N-1; l++)
    {
        sMinus = 0.5 * (S[l]+S[l-1]);
        sPlus = 0.5 * (S[l]+S[l+1]);
        p[0][l] = p[1][l]-((rho*c*lambda)/((sPlus+sMinus)/2))*(v[0][l]*sPlus-v[0][l-1]*sMinus);
    }
    
    p[0][0] = p[1][0] - (rho*c*lambda)/S[0] *(-2*(uB+uR)+2*S[0]*v[0][0]);
    p[0][N] = (1-rho*c*lambda*zeta3)/(1+rho*c*lambda*zeta3)*p[1][N]-((2*rho*c*lambda)/(1+rho*c*lambda*zeta3))*(vInt+zeta4*pInt-(0.5*(S[N]+S[N-1])*v[0][N-1])/S[N]);
        
    vInt=vInt+k/Lr*0.5*(p[0][N]+p[1][N]);
    pInt= zeta1 * 0.5*(p[0][N]+p[1][N]) + zeta2 * pInt;
    
    Out = p[1][N];
    
    calculateOutputFilter();
    
}

void WindFDTDVoice::calculateBoreShape()
{
    for (int i = 0; i <= nC; i++)
    {
        sC[i] = MathConstants<double>::pi * pow(cR,2);
    }
    // if bell is linear
    if(shape == 1)
    {
        auto r = cR;
        auto rGrowth = (bR - cR) / nB;
        for (int i = 1; i<= nB; i++)
        {
            r = r + rGrowth;
            sB[i-1] = juce::MathConstants<double>::pi*pow(r,2);
        }
    }
    // if bell is exponential
    else if(shape == 2)
    {
        auto r = cR;
        auto rGrowth = exp(log(bR/cR)/nB);
        for (int i = 1; i<= nB; i++)
        {
            r = cR * pow(rGrowth,i);
            sB[i-1] = juce::MathConstants<double>::pi*pow(r,2);
        }
    }
    // if bell is logarithmic
    else if(shape == 3)
    {
        auto r = cR;
        auto rGrowth = (bR-cR)/log(nB);
        for (int i = 1; i<= nB; i++)
        {
            r = cR + rGrowth * log(i);
            sB[i-1] = juce::MathConstants<double>::pi*pow(r,2);
        }
    }
    sC.insert( sC.end(), sB.begin(), sB.end() );
    for (int i = 0; i <= N; i++)
       S[i] = sC[i];
}

void WindFDTDVoice::calculateOutputFilter()
{
    // Calculate filter cutoff divided by sample rate
    auto fC = sqrt(pow(c,2)*juce::MathConstants<double>::pi/S[N]);
    auto wF = 2.0*juce::MathConstants<double>::pi*fC/fs;
    
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
    double* pTmp = p[1];
    p[1] = p[0];
    p[0] = pTmp;
    
    double* vTmp = v[1];
    v[1] = v[0];
    v[0] = vTmp;
    
    y[2] = y[1];
    y[1] = y[0];
}

void WindFDTDVoice::getLength(double boreScale, double cylinderBellRatio)
{
    L = (boreScale * 2 * MathConstants<double>::pi * c)/(omegaR*rho);
    cL = L * cylinderBellRatio;
    bL = L * (1-cylinderBellRatio);
}

void WindFDTDVoice::initParameters()
{
    c=343;
    rho = 1.225;
    getLength(boreScale, cBRatio);
    k=1/fs;
    h = c* k;
    nC = floor(cL/h);
    nB = floor(bL/h);
    nMax = floor(10/h);
    N = nC + nB;
    h = L/N;
    lambda = c*k/h;
    H_0 = 2.9*pow(10,-4);
    w = 0.005;
    sR = 1.46*pow(10,-5);
    R = 0.5;
    sigmaR = 5;
    alphaR = 2+pow(omegaR,2)*pow(k,2)+ sigmaR * k;
    betaR = sigmaR * k  - 2 - pow(omegaR,2)*pow(k,2);
    xiR = (2* sR * pow(k,2))/M;
    uB= 0;
    uR= 0;
    sC.clear();
    sC.resize(nC+1,0);
    sB.clear();
    sB.resize(nB,0);
    S.clear();
    S.resize(N+1,0);

    calculateBoreShape();
    deltaP = 0;
    
    a1 = 2/k + pow(omegaR,2) * k + sigmaR;
    a2 = sR / M;
    a3 = 0;

    b1 = 0;
    b2 = (S[0] * h) / (rho * pow(c,2) * k);
    
    c1 = 0;
    c2 = b2+(a2*sR)/a1;
    c3 = 0;
    
    R1 = rho*c;
    R2 = 0.505* rho*c;
    Lr = 0.613*rho*sqrt(S[N]/MathConstants<double>::pi);
    Cr = 1.111* sqrt(S[N])/(rho*pow(c,2)*sqrt(MathConstants<double>::pi));
    
    zeta1=(2*R2*k)/(2*R1*R2*Cr+k*(R1+R2));
    zeta2 = (2*R1*R2*Cr-k*(R1+R2))/(2*R1*R2*Cr+k*(R1+R2));
    zeta3 = k/(2*Lr)+zeta1/(2*R2)+(Cr*zeta1)/k;
    zeta4 = (zeta2+1)/(2*R2)+(Cr*zeta2-Cr)/k;

    vInt = 0;
    pInt = 0;
    
    y = std::vector<double>(3,0);
    
    vStates.reserve(2 * N);
    vStates = std::vector<std::vector<double>> (2,
                                        std::vector<double>(N, 0));
    pStates.reserve(2 * (N+1));
    pStates = std::vector<std::vector<double>> (2,
                                        std::vector<double>(N+1, 0));
    
    // Initialise vector of pointers to the states
    v.resize (2, nullptr);
    p.resize (2, nullptr);
    
    
    y[0] = 0;
    y[1] = 0;
    y[2] = 0;
        
    for (int i = 0; i < 2; ++i)
        v[i] = &vStates[i][0];
    
    for (int i = 0; i < 2; ++i)
        p[i] = &pStates[i][0];

}

void WindFDTDVoice::setADSR(double sampleRate)
{
    pressureAdsr.setSampleRate(sampleRate);
    pressureAdsrParams.attack = pressureAttack;
    pressureAdsrParams.decay = pressureDecay;
    pressureAdsrParams.sustain = pressureSustain;
    pressureAdsrParams.release = pressureRelease;
    pressureAdsr.setParameters(pressureAdsrParams);
}
