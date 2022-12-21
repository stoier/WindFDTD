/*
  ==============================================================================

    ThinPlate.cpp
    Created: 21 Sep 2022 3:33:46pm
    Author:  Benjamin Støier

  ==============================================================================
*/

#include "ThinPlate.h"
#include <math.h>

//==============================================================================
ThinPlate::ThinPlate (double kIn) : k (kIn) // <- This is an initialiser list. It initialises the member variable 'k' (in the "private" section in OneDWave.h), using the argument of the constructor 'kIn'.
{
    firstHit = false;
    tubeConn= false;
    tol = 1e-7;
    Lx= 0.5;
    Ly = 0.5; //side length (y)
    c = 343; //speed of sound
    sigma0 = 1; //frequency independent damping
    sigma1 = 0.005; //frequency dependent damping
    rho = 7700; //material density
    H = 5 * pow(10,-3); //Thickness
    E = 2 * pow(10,11); //Young's modulus
    nu = 0.3; //Poisson's ratio
    
    K1 = 1e8;
    K3 = 1e10;
    R = 0.01;
    
    D = E*pow(H,3)/(12*(1-pow(nu,2))); // stifness coefficient
    kappa = sqrt(D/(rho*H)); // stifness paramater
    
    h = 2*sqrt(k*(sigma1+sqrt(pow(kappa,2)+pow(sigma1,2))));
    Nx = floor(Lx/h); //grid steps (x)
    Ny = floor(Ly/h); //grid steps (y)
    h = std::min (Lx / Nx, Ly / Ny); //redefine grid spacing (using the smallest dimension)

    N = (Nx+1)*(Ny+1);
    mu = kappa*k/pow(h,2);
    muSq = mu * mu;
    S = 2 * sigma1 * k /pow(h,2);
    maxForce = 10; //maximum force
    excTime = 0.001;
    t0 = 0;
    t = 0;
    excXpos = 0.3;
    excYpos = 0.3;
    hx = Lx/Nx;
    hy = Ly/Ny;
    excXidx = floor(excXpos/hx);
    excYidx = floor(excYpos/hy);
    alphaX = excXpos/hx-excXidx;
    alphaY = excYpos/hy-excYidx;
    isBowing = false;
    bowEnd = true;
    lisXpos = 0.3;
    lisYpos = 0.3;
    n=0;
    J=0;
    tubeOut = 0;
    LS=0.2;
    rS= 0.001;
    sPosSpread=50;
    rhoS = 7850;
    ES = 2e11;
    sigma0S = 0.2;
    sigma1S = 0.005;
    TavgS = 1200;
    numStrings= 7;
    initParameters();
}

ThinPlate::~ThinPlate()
{
}

void ThinPlate::getSampleRate(double fsToSet)
{
    fs = fsToSet;
}



void ThinPlate::initParameters()
{
    D = E*pow(H,3)/(12*(1-pow(nu,2))); // stifness coefficient
    kappa = sqrt(D/(rho*H)); // stifness paramater
    h = 2*sqrt(k*(sigma1+sqrt(pow(kappa,2)+pow(sigma1,2))));
    Nx = floor(Lx/h); //grid steps (x)
    Ny = floor(Ly/h); //grid steps (y)
    h = std::min (Lx / Nx, Ly / Ny); //redefine grid spacing (using the smallest dimension)
    N = (Nx+1)*(Ny+1);
    
    mu = kappa*k/pow(h,2);
    muSq = mu * mu;
    S = 2 * sigma1 * k /pow(h,2);
    
    hx = Lx/Nx;
    hy = Ly/Ny;
    
    excXpos = excXposRatio*Lx;
    excYpos = excYposRatio*Ly;
    excXidx = floor(excXpos/hx);
    excYidx = floor(excYpos/hy);
    alphaX = excXpos/hx-excXidx;
    alphaY = excYpos/hy-excYidx;
    
    J=0;
    n=0;
    
    
    uStates = std::vector<std::vector<std::vector<double>>> (3, std::vector<std::vector<double>>(Nx, std::vector<double>(Ny, 0)));
    uNext = &uStates[0][0]; //Initialise time step u^n+1
    u = &uStates[1][0]; //Initialise time step u^n
    uPrev = &uStates[2][0]; //Initialise time step u^n-1
    //auto NSMaxP =  std::max_element(std::begin(NS),std::end(NS));

    if (0 < numStrings)
    {
        stringConn = true;
    }
    else
    {
        stringConn = false;
    }
    
    if (stringConn == true)
    {
        eta.resize(numStrings);
        etaPrev.resize(numStrings);
        etaNext.resize(numStrings);
        rPlus.resize(numStrings);
        rMinus.resize(numStrings);
        connF.resize(numStrings);
        connXPos.resize(numStrings);
        connXPos2.resize(numStrings);
        lcS.resize(numStrings);
        lcP.resize(numStrings);
        lcS2.resize(numStrings);
        lcP2.resize(numStrings);
        NS.resize(numStrings);
        cSSq.resize(numStrings);
        TS.resize(numStrings);
        hS.resize(numStrings);
        lambdaSSq.resize(numStrings);
        muSSq.resize(numStrings);
        uS1.resize(numStrings);
        uS2.resize(numStrings);
        uS3.resize(numStrings);
        stringConnTerm.resize(numStrings);
        
        IS = M_PI * rS*rS*rS*rS / 4;
        AS = M_PI * rS*rS;
        kappaSSq = ES * IS / (rhoS * AS);
        As = 1 + sigma0S * k;
        
        for (int nS = 0; nS < numStrings; ++nS)
        {
            connSPos = 0.1 * LS;
            connSPos2 = LS - connSPos;
            if (nS == 0)
            {
                connXPos[nS] = Lx * 0.5;
                connYPos =  Ly * 0.5 + LS * 0.4;
                //connXPos2[nS] = connXPos[nS];
                connYPos2 = Ly * 0.5 - LS * 0.4;
                TS[nS] = TavgS;
                
            }
            else if (nS % 2)
            {
                connXPos[nS] = Lx * 0.5 + Lx * 0.5 * nS/numStrings * sPosSpread/100;
                connYPos = Ly * 0.5 + LS * 0.4;
                //connXPos2[nS] = connXPos[nS];
                connYPos2 = Ly * 0.5 - LS * 0.4;
                TS[nS] = TavgS + TavgS*TDiffS/(200*nS);
            }
            else
            {
                connXPos[nS] = Lx * 0.5 - Lx * 0.5 * (nS-1)/numStrings * sPosSpread/100;
                connYPos = Ly * 0.5 + LS * 0.4;
                //connXPos2[nS] = connXPos[nS];
                connYPos2 = Ly * 0.5 - LS * 0.4;
                TS[nS] = TavgS - TavgS*TDiffS/(200*(nS-1));
            }
            //TS[nS] = TavgS;
            cSSq[nS] = TS[nS] / (rhoS * AS);
            hS[nS] = sqrt((cSSq[nS]  * k*k + 4 * sigma1S * k + sqrt((cSSq[nS]  * k*k + 4 * sigma1S * k)*(cSSq[nS]  * k*k + 4 * sigma1S * k) + 16 * kappaSSq * k*k))/2);
            NS[nS]  = floor(LS/hS[nS]);
            hS[nS] = LS / NS[nS];
            lambdaSSq[nS] = cSSq[nS]*k*k/(hS[nS]*hS[nS]);
            muSSq[nS] = kappaSSq*k*k/(hS[nS]*hS[nS]*hS[nS]*hS[nS]);
            uS1[nS]  = 2-2*lambdaSSq[nS]-6*muSSq[nS]-4*sigma1S*k/(hS[nS]*hS[nS]);
            uS2[nS] = lambdaSSq[nS]+4*muSSq[nS]+2*sigma1S*k/(hS[nS]*hS[nS]);
            uS3[nS] = -1+sigma0S*k + 4*sigma1S*k/(hS[nS]*hS[nS]);
            
            stringConnTerm[nS] = k * k / (rhoS * AS * hS[nS] * (1.0 + sigma0S * k));
            
            //alphaConnS[nS] = connSPos[nS]/hS[nS] - lcS(i)
            lcS[nS]  = floor(connSPos/hS[nS]);
            lcP[nS] = floor(connXPos[nS]/hx);
            mcP = floor(connYPos/hy);
            
            lcS2[nS]  = floor(connSPos2/hS[nS]);
            //lcP2[nS] = floor(connXPos2[nS]/hx);
            mcP2 = floor(connYPos2/hy);
        }
        NSMax = NS[0];
        uStringStates =std::vector<std::vector<std::vector<double>>> (3, std::vector<std::vector<double>>(numStrings, std::vector<double>(NSMax+1, 0)));
        
        uStringNext = &uStringStates[0][0]; //Initialise time step u^n+1
        uString = &uStringStates[1][0]; //Initialise time step u^n
        uStringPrev = &uStringStates[2][0]; //Initialise time step u^n-1
        
    }
    
    if (tubeConn == true)
    {
        cT = 343;
        rhoT = 1.225;
        LT = cLT + bLT;
        hT = cT *k;
        nCT = floor(cLT/h);
        nBT = floor(bLT/h);
        NT = nCT + nBT;
        hT = LT/NT;
        lambdaT = cT*k/hT;
        sC.clear();
        sC.resize(nCT+1,0);
        sB.clear();
        sB.resize(nBT,0);
        ST.clear();
        ST.resize(NT+1,0);
        calculateBoreShape();
        R1 = rhoT*cT;
        R2 = 0.505* rhoT*cT;
        
        Lr = 0.613*rhoT*sqrt(ST[NT]/juce::MathConstants<double>::pi);
        Cr = 1.111* sqrt(ST[NT])/(rhoT*cT*cT*sqrt(juce::MathConstants<double>::pi));
        zeta1=(2*R2*k)/(2*R1*R2*Cr+k*(R1+R2));
        zeta2 = (2*R1*R2*Cr-k*(R1+R2))/(2*R1*R2*Cr+k*(R1+R2));
        zeta3 = k/(2*Lr)+zeta1/(2*R2)+(Cr*zeta1)/k;
        zeta4 = (zeta2+1)/(2*R2)+(Cr*zeta2-Cr)/k;
        vInt = 0;
        pInt = 0;
        
        vStates.reserve(2 * NT);
        vStates = std::vector<std::vector<double>> (2,
                                            std::vector<double>(NT, 0));
        pStates.reserve(3 * (NT+1));
        pStates = std::vector<std::vector<double>> (3,
                                            std::vector<double>(NT+1, 0));
        
        // Initialise vector of pointers to the states
        v.resize (2, nullptr);
        p.resize (3, nullptr);
        
        for (int i = 0; i < 2; ++i)
            v[i] = &vStates[i][0];
        
        for (int i = 0; i < 3; ++i)
            p[i] = &pStates[i][0];
        
        tubeConn = k* k / (rhoT * ST[0] * hT);
        lcPT = floor(Nx*0.5);
        mcPT = floor(Ny*0.5);
        lcT = 0;
    }
    
    plateConnTerm = (k*k)/(rho*H*h*h*(1+sigma0*k));
    
    
}

void ThinPlate::updateParameters(const double sig0ToSet, const double sig1ToSet, const double LxToSet, const double LyToSet, const double excXToSet, const double excYToSet, const double lisXToSet, const double lisYToSet, const double thicknessToSet, const double excFToSet, const double excTToSet, const double vBToSet, const double FBToSet, const double aToSet, const int excTypeId, const double  bAtt1ToSet, const double bDec1ToSet, const double  bSus1ToSet, const double bRel1ToSet, const double FBEnv1ToSet, const double vBEnv1ToSet, const double lfoRateToSet, const double xPosModToSet, const double yPosModToSet, const int numStringsToSet, const double sLenToSet, const double sPosSpreadToSet, const double sAvgTenToSet, const double sTenDiffToSet, const double sRadToSet, const double sSig0ToSet, const double cylinderLengthToSet, const double cylinderRadiusToSet, const double bellLengthToSet, const double bellRadiusToSet, const int bellGrowth, bool tubeConnToSet, bool springConnToSet)
{
    sigma0 = sig0ToSet;
    sigma1 = sig1ToSet;
    Lx = LxToSet;
    Ly = LyToSet;
    excXposRatio = excXToSet;
    excYposRatio = excYToSet;
    lisXpos = lisXToSet;
    lisYpos = lisYToSet;
    lfoRate= lfoRateToSet;
    xPosMod = xPosModToSet/1000;
    yPosMod = yPosModToSet/1000;
    H = thicknessToSet*0.001f;
    auto prevNumStrings = numStrings;
    numStrings = numStringsToSet;
    LS = sLenToSet*Ly;
    TavgS = sAvgTenToSet;
    TDiffS = sTenDiffToSet;
    rS= sRadToSet*0.001f;
    sPosSpread = sPosSpreadToSet;
    sigma0S = sSig0ToSet;
    cLT = cylinderLengthToSet;
    bLT = bellLengthToSet;
    cRT = cylinderRadiusToSet;
    bRT = bellRadiusToSet;
    bCT = bellGrowth;
    springConn = springConnToSet;
    
    if (excTypeId == 1)
    {
        vB = vBToSet;
        FB = FBToSet;
        a = aToSet * 0.00000000001;
        FBEnv1 = FBEnv1ToSet;
        vBEnv1 = vBEnv1ToSet;
        bAtt1 = bAtt1ToSet;
        bDec1 = bDec1ToSet;
        bSus1 = bSus1ToSet;
        bRel1 = bRel1ToSet;
        setADSR(fs);
        excType = Bow;
    }
    else
    {
        maxForce = excFToSet;
        excTime = excTToSet*0.001f;
        excType = Mallet;
    }

    if (tubeConnToSet == true)
    {
        tubeConn = true;
        initParameters();
    }
    else
    {
        tubeConn = false;
    }
    if (numStrings > prevNumStrings)
    {
        if (tubeConn != true)
        {
            initParameters();
        }
    }
    
}

void ThinPlate::updatePlateMaterial(int plateMaterialToSet)
{
    plateMaterial=plateMaterialToSet;
    if (plateMaterial == 1) //brass
    {
        rho=8530;
        E = 110 * pow(10, 9);
        nu = 0.34;
    }
    else if (plateMaterial == 2) //bronze
    {
        rho = 8770;
        E = 103 * pow(10, 9);
        nu = 0.34;
    }
    else if (plateMaterial == 3) //iron
    {
        rho = 7874;
        E = 211 * pow(10, 9);
        nu = 0.26;
    }
    else if (plateMaterial == 4) // aluminium
    {
        rho = 2700;
        E = 70 * pow(10, 9);
        nu = 0.33;
    }
    else if (plateMaterial == 5) //gold
    {
        rho = 19300;
        E = 79 * pow(10, 9);
        nu = 0.42;
    }
    else if (plateMaterial == 6) //silver
    {
        rho = 10490;
        E = 83 * pow(10, 9);
        nu = 0.37;
    }
    else if (plateMaterial == 7) //copper
    {
        rho = 8920;
        E = 120 * pow(10, 9);
        nu = 0.36;
    }
}

void ThinPlate::plateHit()
{
    firstHit = true;
    n = 0;
    t = 0;
    for (int nS = 0; nS < numStrings; ++nS)
        connF[nS] = 0;
}

void ThinPlate::startBow()
{
    if (bowEnd == true)
    {
        adsr1.noteOn();
        isBowing = true;
        bowEnd = false;
    }
}

void ThinPlate::endBow()
{
    adsr1.noteOff();
    isBowing = false;
    bowEnd = true;
}

void ThinPlate::calculateScheme()
{
    //Get number of cycles pr. sample the number of cycles pr. sample for the lfo
    auto cyclesPerSampleLFO= lfoRate / fs;
    
    //Update the amount that the phase angle of the vibrato needs to increment for each sample
    angleDeltaLFO = cyclesPerSampleLFO * 2.0 * juce::MathConstants<double>::pi;
    
    currentXMod = std::sin(currentAngleLFO)*xPosMod;
    currentYMod = std::sin(currentAngleLFO)*yPosMod;
    currentAngleLFO += angleDeltaLFO;
    
    switch (excType) {
        case Mallet:
            if (n < floor(excTime*fs))
            {
                malletForce= maxForce/2*(1-std::cos((2*juce::MathConstants<double>::pi*(t-t0))/(excTime)));
                t = t+k;
            }
            else
            {
                malletForce = 0;
            }
            excitation = malletForce;
            n++;
            break;
            
        case Bow:
           if (isBowing == true)
           {
               excXpos = excXposRatio*Lx;
               excYpos = excYposRatio*Ly;
               if (0 < xPosMod)
               {
                   excXpos = excXpos + (currentXMod *  excXpos);
               }
               if (0 < yPosMod)
               {
                   excYpos = excYpos + (currentYMod *  excYpos);
               }
               excXidx = floor(excXpos/hx);
               excYidx = floor(excYpos/hy);
               alphaX = excXpos/hx-excXidx;
               alphaY = excYpos/hy-excYidx;
                
               nextAdsr1 = adsr1.getNextSample();
               b = ((2/k) + 2*sigma0)*(vB*nextAdsr1) - ((2/(k*k)*interpolation(u,excXidx,excYidx,alphaX, alphaY)-interpolation(uPrev,excXidx,excYidx,alphaX, alphaY)) + ((kappa*kappa)/(h*h*h*h))*(interpolation(u,excXidx+2,excYidx,alphaX, alphaY)+interpolation(u,excXidx-2,excYidx,alphaX, alphaY)+interpolation(u,excXidx,excYidx+2,alphaX, alphaY)+interpolation(u,excXidx,excYidx-2,alphaX, alphaY))
                + 2 * (interpolation(u,excXidx+1,excYidx+1,alphaX, alphaY)+interpolation(u,excXidx+1,excYidx-1,alphaX, alphaY)+interpolation(u,excXidx-1,excYidx+1,alphaX, alphaY)+interpolation(u,excXidx-1,excYidx-1,alphaX, alphaY)-8*(interpolation(u,excXidx+1,excYidx,alphaX, alphaY)+interpolation(u,excXidx-1,excYidx,alphaX, alphaY)+interpolation(u,excXidx,excYidx+1,alphaX, alphaY)+interpolation(u,excXidx,excYidx-1,alphaX, alphaY))+20*interpolation(u,excXidx,excYidx,alphaX, alphaY))
                - 2*sigma1/(k*h*h)*(interpolation(u,excXidx+1,excYidx,alphaX, alphaY)+interpolation(u,excXidx-1,excYidx,alphaX, alphaY)+interpolation(u,excXidx,excYidx+1,alphaX, alphaY)+interpolation(u,excXidx,excYidx-1,alphaX, alphaY)-interpolation(u,excXidx+1,excYidx,alphaX, alphaY)-interpolation(u,excXidx-1,excYidx,alphaX, alphaY)-interpolation(u,excXidx,excYidx+1,alphaX, alphaY)-interpolation(u,excXidx,excYidx-1,alphaX, alphaY)-4*(interpolation(u,excXidx,excYidx,alphaX, alphaY)-interpolation(uPrev,excXidx,excYidx,alphaX, alphaY))));
                eps = 1;
                int i = 0;
                while (eps > tol && i < 100)
                {
                    vRel =  vRelPrev - (((2/k+2*sigma0)*vRelPrev+(FB*nextAdsr1)*sqrt(2*a)*vRelPrev*exp(-a*vRelPrev*vRelPrev+0.5)+b)/(2/k + 2*sigma0+(FB*nextAdsr1)*sqrt(2*a)*(1-2*a*vRel*vRel)*exp(-a*(vRel*vRel+0.5))));
                    eps = std::abs(vRel-vRelPrev);
                    vRelPrev = vRel;
                    ++i;
                }
                excitation = sqrt(2*a)*vRel*exp(-a*vRel*vRel+0.5)*(FB*nextAdsr1);
           }
            break;
    }
    
    for (int l = 2; l < Nx-2; ++l) // clamped boundaries
    {
        for (int m = 2; m < Ny-2; ++m) // clamped boundaries
        {
            //Implement spreading 1tb order 2D spreading operator
            if (l == excXidx && m == excYidx)
                J = ((1-alphaX)*(1-alphaY))/(hx*hy);
            else if (l == excXidx && m == excYidx+1)
                J = ((1-alphaX)*alphaY)/(hx*hy);
            else if (l == excXidx+1 && m == excYidx)
                J = (alphaX*(1-alphaY))/(hx*hy);
            else if (l == excXidx+1 && m == excYidx+1)
                J = (alphaX*alphaY)/(hx*hy);
            else
                J = 0;
            //calculate next plate state
            uNext[l][m] =
            (2-20*muSq-4*S)*u[l][m]
            + (8*muSq+S) * (u[l+1][m] + u[l-1][m] + u[l][m+1] + u[l][m-1])
            - 2*muSq * (u[l+1][m+1] + u[l-1][m+1] + u[l+1][m-1] + u[l-1][m-1])
            - muSq * (u[l+2][m] + u[l-2][m] + u[l][m+2] + u[l][m-2])
            + (sigma0*k-1+4*S) * uPrev[l][m]
            - S * (uPrev[l+1][m] + uPrev[l-1][m] + uPrev[l][m+1] + u[l][m-1])
            + J * excitation;
        }
    }
    if (stringConn == true)
    {
        stringOut = 0;
        for (int nS = 0; nS < numStrings; ++nS)
        {
            for (int l = 2; l < NS[nS]-1; l++)
            {
                uStringNext[nS][l] =
                uS1[nS] * uString[nS][l]
                + uS2[nS] * (uString[nS][l+1]+uString[nS][l-1]) - muSSq[nS] * (uString[nS][l+2]+uString[nS][l-2])
                + uS3[nS] * uStringPrev[nS][l]-2*sigma1S*k / (hS[nS]*hS[nS])*(uStringPrev[nS][l+1]+uStringPrev[nS][l-1])/As;
                
            }

            etaNext[nS] = uStringNext[nS][lcS[nS]]-uNext[lcP[nS]][mcP];
            eta[nS] = uString[nS][lcS[nS]]-u[lcP[nS]][mcP];
            etaPrev[nS] = uStringPrev[nS][lcS[nS]]-uPrev[lcP[nS]][mcP];
            rPlus[nS] = 0.5 * K1 + 0.5 * K3 * eta[nS] * eta[nS] + 0.5 * fs * R;
            rMinus[nS] = 0.5 * K1 + 0.5 * K3 * eta[nS] * eta[nS] + 0.5 * fs * R;
            if (springConn == true)
            {
                connF[nS]  = (etaNext[nS] + rMinus[nS] / rPlus[nS] * etaPrev[nS]) / (1/rPlus[nS] + plateConnTerm + stringConnTerm[nS]);
            }
            else
            {
                connF[nS]  = etaNext[nS]/(plateConnTerm + stringConnTerm[nS]);
            }
            uStringNext[nS][lcS[nS]]= uStringNext[nS][lcS[nS]]-connF[nS]*stringConnTerm[nS];
            uNext[lcP[nS]][mcP] = uNext[lcP[nS]][mcP] + connF[nS]*plateConnTerm;
            
            etaNext[nS] = uStringNext[nS][lcS2[nS]]-uNext[lcP[nS]][mcP2];
            eta[nS] = uString[nS][lcS2[nS]]-u[lcP[nS]][mcP];
            etaPrev[nS] = uStringPrev[nS][lcS2[nS]]-uPrev[lcP[nS]][mcP2];
            rPlus[nS] = 0.5 * K1 + 0.5 * K3 * eta[nS] * eta[nS] + 0.5 * fs * R;
            rMinus[nS] = 0.5 * K1 + 0.5 * K3 * eta[nS] * eta[nS] + 0.5 * fs * R;
            if (springConn == true)
            {
                connF[nS]  = (etaNext[nS] + rMinus[nS] / rPlus[nS] * etaPrev[nS]) / (1/rPlus[nS] + plateConnTerm + stringConnTerm[nS]);
            }
            else
            {
                connF[nS]  = etaNext[nS]/(plateConnTerm + stringConnTerm[nS]);
            }
            uStringNext[nS][lcS2[nS]]= uStringNext[nS][lcS2[nS]]-connF[nS]*stringConnTerm[nS];
            uNext[lcP[nS]][mcP2] = uNext[lcP[nS]][mcP2] + connF[nS]*plateConnTerm;
            stringOutIdx = floor(NS[nS]*0.5);
            stringOut = stringOut + uString[nS][stringOutIdx];
            //updateStringStates();
        }
        //compensate for extra volume
        stringOut = stringOut/numStrings;
        

    }
    
    if (tubeConn == true)
    {
        for (int l = 0; l <= NT-1; l++)
        {
            v[0][l] = v[1][l]-((lambdaT/(rhoT*cT)))*(p[1][l+1]-p[1][l]);
        }
        for(int l = 1; l <= NT-1; l++)
        {
            sMinus = 0.5 * (ST[l]+ST[l-1]);
            sPlus = 0.5 * (ST[l]+ST[l+1]);
            p[0][l] = p[1][l]-((rhoT*cT*lambdaT)/((sPlus+sMinus)/2))*(v[0][l]*sPlus-v[0][l-1]*sMinus);
        }
        p[0][NT] = (1-rhoT*cT*lambdaT*zeta3)/(1+rhoT*cT*lambdaT*zeta3)*p[1][NT]-((2*rhoT*cT*lambdaT)/(1+rhoT*cT*lambdaT*zeta3))*(vInt+zeta4*pInt-(0.5*(ST[NT]+ST[NT-1])*v[0][NT-1])/ST[NT]);
            
        vInt=vInt+k/Lr*0.5*(p[0][NT]+p[1][NT]);
        pInt= zeta1 * 0.5*(p[0][NT]+p[1][NT]) + zeta2 * pInt;
        
        etaNextT = p[0][lcT] - uNext[lcPT][mcPT];
        etaT = p[1][lcT]-u[lcPT][mcPT];
        etaPrevT = p[2][lcT]-uPrev[lcPT][mcPT];
        rPlusT = 0.5 * K1 + 0.5 * K3 * etaT * etaT + 0.5 * fs * R;
        rMinusT = 0.5 * K1 + 0.5 * K3 * etaT * etaT - 0.5 * fs * R;
        if (springConn == true)
        {
            connFT = (etaNextT  + rMinusT / rPlusT * etaPrevT) / (1/rPlusT + plateConnTerm + tubeConnTerm);
        }
        else
        {
            connFT = etaNextT / (plateConnTerm + tubeConnTerm);
        }
        p[0][lcT] = p[0][lcT] - connFT * tubeConnTerm;
        uNext[lcPT][mcPT] = uNext[lcPT][mcPT] + connFT * plateConnTerm;
        tubeOut = p[1][NT-1];
    }
    updateStates();
}

void ThinPlate::updateStates()
{
    std::vector<double>* uTmp = uPrev;
    uPrev = u;
    u = uNext;
    uNext = uTmp;
    
   
    std::vector<double>* uStringTmp = uStringPrev;
    uStringPrev = uString;
    uString = uStringNext;
    uStringNext = uStringTmp;
    
    if (tubeConn == true)
    {
        double* pTmp = p[2];
        p[2] = p[1];
        p[1] = p[0];
        p[0] = pTmp;
    
        double* vTmp = v[1];
        v[1] = v[0];
        v[0] = vTmp;
    }
}


void ThinPlate::setADSR(double sampleRate)
{
    adsr1.setSampleRate(sampleRate);
    adsr1Params.attack = bAtt1;
    adsr1Params.decay = bDec1;
    adsr1Params.sustain = bSus1;
    adsr1Params.release = bRel1;
    adsr1.setParameters(adsr1Params);
    
}


void ThinPlate::calculateBoreShape()
{
    for (int i = 0; i <= nCT; i++)
    {
        sC[i] = juce::MathConstants<double>::pi * cRT*cRT;
    }
    // if bell is linear
    if(shape == 1)
    {
        auto r = cRT;
        auto rGrowth = (bRT - cRT) / nBT;
        for (int i = 1; i<= nBT; i++)
        {
            r = r + rGrowth;
            sB[i-1] = juce::MathConstants<double>::pi*r*r;
        }
    }
    // if bell is exponential
    else if(shape == 2)
    {
        auto r = cRT;
        auto rGrowth = exp(log(bRT/cRT)/nBT);
        for (int i = 1; i<= nBT; i++)
        {
            r = cRT * pow(rGrowth,i);
            sB[i-1] = juce::MathConstants<double>::pi*r*r;
        }
    }
    // if bell is logarithmic
    else if(shape == 3)
    {
        auto r = cRT;
        auto rGrowth = (bRT-cRT)/log(nBT);
        for (int i = 1; i<= nBT; i++)
        {
            r = cRT + rGrowth * log(i);
            sB[i-1] = juce::MathConstants<double>::pi*r*r;
        }
    }
    sC.insert( sC.end(), sB.begin(), sB.end() );
    
    for (int i = 0; i <= NT; i++)
        ST[i] = sC[i];
}

