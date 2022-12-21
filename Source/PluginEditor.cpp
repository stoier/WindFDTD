/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PlateAudioProcessorEditor::PlateAudioProcessorEditor (PlateAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
 sig0SliderAttachment(audioProcessor.tree, "Frequency Independent Damping", sig0Slider), sig1SliderAttachment(audioProcessor.tree, "Frequency Dependent Damping", sig1Slider),
lengthXSliderAttachment(audioProcessor.tree, "Plate length X", lengthXSlider),
lengthYSliderAttachment(audioProcessor.tree, "Plate length Y", lengthYSlider),
excXSliderAttachment(audioProcessor.tree, "Excitation pos X", excXSlider),
excYSliderAttachment(audioProcessor.tree, "Excitation pos Y", excYSlider),
lisXSliderAttachment(audioProcessor.tree, "Listening pos X", lisXSlider),
lisYSliderAttachment(audioProcessor.tree, "Listening pos Y", lisYSlider),
thicknessSliderAttachment(audioProcessor.tree, "Plate thickness", thicknessSlider),
excFSliderAttachment(audioProcessor.tree, "Excitation force", excFSlider),
excTSliderAttachment(audioProcessor.tree, "Excitation time", excTSlider), vBSliderAttachment(audioProcessor.tree, "Bow velocity", vBSlider), fBSliderAttachment(audioProcessor.tree, "Bow force", fBSlider), fricSliderAttachment(audioProcessor.tree, "Friction", fricSlider), bAtt1SliderAttachment(audioProcessor.tree, "Bow attack 1", bAtt1Slider), bDec1SliderAttachment(audioProcessor.tree, "Bow decay 1", bDec1Slider), bSus1SliderAttachment(audioProcessor.tree, "Bow sustain 1", bSus1Slider), bRel1SliderAttachment(audioProcessor.tree, "Bow release 1", bRel1Slider), FBEnv1SliderAttachment(audioProcessor.tree, "Bow force env 1", FBEnv1Slider), vBEnv1SliderAttachment(audioProcessor.tree, "Bow velocity env 1", vBEnv1Slider), xPosLFOModSliderAttachment(audioProcessor.tree, "X Pos Mod Depth", xPosLFOModSlider), yPosLFOModSliderAttachment(audioProcessor.tree, "Y Pos Mod Depth", yPosLFOModSlider), lfoRateSliderAttachment(audioProcessor.tree, "LFO Rate", lfoRateSlider), numStringSliderAttachment(audioProcessor.tree, "Number of Strings", numStringSlider), stringTensionDiffSliderAttachment(audioProcessor.tree, "String Tension Difference", stringTensionDiffSlider), stringLengthSliderAttachment(audioProcessor.tree, "String Length", stringLengthSlider), stringRadiusSliderAttachment(audioProcessor.tree, "String Radius", stringRadiusSlider),
    stringTensionSliderAttachment(audioProcessor.tree, "String Tension" , stringTensionSlider),
    stringPosSpreadSliderAttachment(audioProcessor.tree, "String Position Spread" , stringPosSpreadSlider), sSig0SliderAttachment(audioProcessor.tree, "String Damping", sSig0Slider),
cylinderLengthSliderAttachment(audioProcessor.tree, "Cylinder Length", cylinderLengthSlider),
cylinderRadiusSliderAttachment(audioProcessor.tree, "Cylinder Radius", cylinderRadiusSlider),
bellLengthSliderAttachment(audioProcessor.tree, "Bell Length", bellLengthSlider),
bellEndRadiusSliderAttachment(audioProcessor.tree, "Bell Radius", bellEndRadiusSlider)
{
    addAndMakeVisible(hammerComponent);
    addAndMakeVisible(bowComponent);
    
    setSliderAndLabelRotary(sig0Slider, sig0Label, "Freq Ind Damp");
    addAndMakeVisible(sig0Slider);
    addAndMakeVisible(sig0Label);
    
    setSliderAndLabelRotary(sig1Slider, sig1Label, "Freq Dep Damp");
    addAndMakeVisible(sig1Slider);
    addAndMakeVisible(sig1Label);
    
    setSliderAndLabelHorizontal(lengthXSlider, lengthXLabel, "Plate Length X");
    lengthXSlider.setTextValueSuffix(" m");
    addAndMakeVisible(lengthXSlider);
    addAndMakeVisible(lengthXLabel);
    lengthXSlider.addListener(this);
    
    setSliderAndLabelVertical(lengthYSlider, lengthYLabel, "Plate Length Y");
    lengthYSlider.setTextValueSuffix(" m");
    addAndMakeVisible(lengthYSlider);
    addAndMakeVisible(lengthYLabel);
    lengthYSlider.addListener(this);
    
    setSliderAndLabelHorizontal(excXSlider, excXLabel, "Excitation Pos X Ratio");
    addAndMakeVisible(excXSlider);
    addAndMakeVisible(excXLabel);
    excXSlider.addListener(this);
    
    setSliderAndLabelVertical(excYSlider, excYLabel, "Excitation Pos Y Ratio");
    addAndMakeVisible(excYSlider);
    addAndMakeVisible(excYLabel);
    excYSlider.addListener(this);
    
    setSliderAndLabelHorizontal(lisXSlider, lisXLabel, "Listening Pos X Ratio");
    addAndMakeVisible(lisXSlider);
    addAndMakeVisible(lisXLabel);
    
    setSliderAndLabelVertical(lisYSlider, lisYLabel, "Listening Pos Y Ratio");
    addAndMakeVisible(lisYSlider);
    addAndMakeVisible(lisYLabel);
    
    setSliderAndLabelRotary(thicknessSlider, thicknessLabel, "Thickness");
    thicknessSlider.setTextValueSuffix(" mm");
    addAndMakeVisible(thicknessSlider);
    addAndMakeVisible(thicknessLabel);
    
    setSliderAndLabelRotary(excFSlider, excFLabel, "Excitation Force");
    excFSlider.setTextValueSuffix(" N");
    addAndMakeVisible(excFSlider);
    addAndMakeVisible(excFLabel);
    
    setSliderAndLabelRotary(excTSlider, excTLabel, "Excitation Time");
    excTSlider.setTextValueSuffix(" ms");
    addAndMakeVisible(excTSlider);
    addAndMakeVisible(excTLabel);
    
    setSliderAndLabelRotary(vBSlider, vBLabel, "Bow Velocity");
    vBSlider.setTextValueSuffix(" m/s");
    addAndMakeVisible(vBSlider);
    addAndMakeVisible(vBLabel);
    
    setSliderAndLabelRotary(fBSlider, fBLabel, "Bow Force");
    excTSlider.setTextValueSuffix(" N");
    addAndMakeVisible(fBSlider);
    addAndMakeVisible(fBLabel);
    
    setSliderAndLabelRotary(fricSlider, fricLabel, "Friction");
    //fricSlider.setTextValueSuffix("");
    addAndMakeVisible(fricSlider);
    addAndMakeVisible(fricLabel);
    
    setSliderAndLabelVertical(bAtt1Slider, bAtt1Label, "Attack");
    addAndMakeVisible(bAtt1Slider);
    addAndMakeVisible(bAtt1Label);
    
    setSliderAndLabelVertical(bDec1Slider, bDec1Label, "Decay");
    addAndMakeVisible(bDec1Slider);
    addAndMakeVisible(bDec1Label);
    
    setSliderAndLabelVertical(bSus1Slider, bSus1Label, "Sustain");
    addAndMakeVisible(bSus1Slider);
    addAndMakeVisible(bSus1Label);
    
    setSliderAndLabelVertical(bRel1Slider, bRel1Label, "Release");
    addAndMakeVisible(bRel1Slider);
    addAndMakeVisible(bRel1Label);
    
    setSliderAndLabelRotary(FBEnv1Slider, FBEnv1Label, "Force Env Amount");
    addAndMakeVisible(FBEnv1Slider);
    addAndMakeVisible(FBEnv1Label);
    
    setSliderAndLabelRotary(vBEnv1Slider, vBEnv1Label, "Velocity Env Amount");
    addAndMakeVisible(vBEnv1Slider);
    addAndMakeVisible(vBEnv1Label);
    
    setSliderAndLabelRotary(lfoRateSlider, lfoRateLabel, "LFO Rate");
    lfoRateSlider.setTextValueSuffix("Hz");
    addAndMakeVisible(lfoRateSlider);
    addAndMakeVisible(lfoRateLabel);
    
    setSliderAndLabelRotary(xPosLFOModSlider, xPosLFOModLabel, "X Pos Mod");
    addAndMakeVisible(xPosLFOModSlider);
    addAndMakeVisible(xPosLFOModLabel);
    
    setSliderAndLabelRotary(yPosLFOModSlider, yPosLFOModLabel, "Y Pos Mod");
    addAndMakeVisible(yPosLFOModSlider);
    addAndMakeVisible(yPosLFOModLabel);
    
    setSliderAndLabelHorizontal(numStringSlider, numStringLabel, "Number of Strings");
    addAndMakeVisible(numStringSlider);
    addAndMakeVisible(numStringLabel);
    numStringSlider.addListener(this);
    
    setSliderAndLabelRotary(stringLengthSlider, stringLengthLabel, "String Length");
    addAndMakeVisible(stringLengthSlider);
    addAndMakeVisible(stringLengthLabel);
    stringLengthSlider.addListener(this);
    
    setSliderAndLabelRotary(stringTensionDiffSlider, stringTensionDiffLabel, "String Tension Difference");
    addAndMakeVisible(stringTensionDiffSlider);
    addAndMakeVisible(stringTensionDiffLabel);
    
    
    setSliderAndLabelRotary(stringTensionSlider, stringTensionLabel, "String Tension");
    stringTensionSlider.setTextValueSuffix("N");
    addAndMakeVisible(stringTensionSlider);
    addAndMakeVisible(stringTensionLabel);
    
    setSliderAndLabelRotary(stringRadiusSlider, stringRadiusLabel, "String Radius");
    addAndMakeVisible(stringRadiusSlider);
    addAndMakeVisible(stringRadiusLabel);
    stringRadiusSlider.setTextValueSuffix(" mm");
    
    setSliderAndLabelRotary(stringPosSpreadSlider, stringPosSpreadLabel, "String Pos Spread");
    addAndMakeVisible(stringPosSpreadSlider);
    addAndMakeVisible(stringPosSpreadLabel);
    stringPosSpreadSlider.addListener(this);
    
    setSliderAndLabelRotary(sSig0Slider, sSig0Label, "String Damping");
    addAndMakeVisible(sSig0Slider);
    addAndMakeVisible(sSig0Label);
    
    hitButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::purple);
    addAndMakeVisible(hitButton);
    hitButton.onClick = [this] {hitButtonClicked();};
    
    bowExcButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::purple);
    bowExcButton.setRadioGroupId(1);
    bowExcButton.setToggleable(true);
    bowExcButton.setClickingTogglesState(true);
    //bowExcButton.setToggleState(true);
    addAndMakeVisible(bowExcButton);
    bowExcButton.onStateChange = [this]
    {
        if (bowExcButton.getToggleState() == true)
        {
            resized();
            audioProcessor.excTypeId = 1;
        }
    };
    
    malletExcButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::purple);
    malletExcButton.setRadioGroupId(1);
    malletExcButton.setToggleable(true);
    malletExcButton.setClickingTogglesState(true);
    addAndMakeVisible(malletExcButton);
    malletExcButton.onStateChange = [this]
    {
        if (malletExcButton.getToggleState() == true)
        {
            resized();
            audioProcessor.excTypeId = 2;
        }
            
    };
    
    stringConnButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::purple);
    stringConnButton.setRadioGroupId(2);
    stringConnButton.setToggleable(true);
    stringConnButton.setClickingTogglesState(true);
    stringConnButton.setToggleState(true, juce::dontSendNotification);
    addAndMakeVisible(stringConnButton);
    stringConnButton.onStateChange = [this]
    {
        if (stringConnButton.getToggleState() == true)
        {
            resized();
            //audioProcessor.excTypeId = 2;
        }
    };
    
    tubeConnButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::purple);
    tubeConnButton.setRadioGroupId(2);
    tubeConnButton.setToggleable(true);
    tubeConnButton.setClickingTogglesState(true);
    addAndMakeVisible(tubeConnButton);
    tubeConnButton.onStateChange = [this]
    {
        if (tubeConnButton.getToggleState() == true)
        {
            resized();
            //audioProcessor.excTypeId = 2;
        }
    };
    
    
    
    springConnButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::purple);
    springConnButton.setRadioGroupId(3);
    springConnButton.setToggleable(true);
    springConnButton.setClickingTogglesState(true);
    springConnButton.setToggleState(true, juce::dontSendNotification);
    addAndMakeVisible(springConnButton);
    springConnButton.onStateChange = [this]
    {
        if (springConnButton.getToggleState() == true)
        {
            audioProcessor.springConn = true;
        }
        else
        {
            audioProcessor.springConn = false;
        }
    };
    
    rigidConnButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::purple);
    rigidConnButton.setRadioGroupId(3);
    rigidConnButton.setToggleable(true);
    rigidConnButton.setClickingTogglesState(true);
    addAndMakeVisible(rigidConnButton);
    rigidConnButton.onStateChange = [this]
    {
        if (rigidConnButton.getToggleState() == true)
        {
            audioProcessor.springConn = false;
        }
        else
        {
            audioProcessor.springConn = true;
        }
    };
   
    
    /*
    linkFBvB.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::purple);
    linkFBvB.setToggleable(true);
    linkFBvB.setClickingTogglesState(true);
    addAndMakeVisible(linkFBvB);
    linkFBvB.onStateChange = [this]
    {
        if (linkFBvB.getToggleState() == true)
        {
           
        }
    };*/
    
    startBowButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::purple);
    startBowButton.setToggleable(true);
    startBowButton.setClickingTogglesState(true);
    addAndMakeVisible(startBowButton);
    startBowButton.onStateChange = [this]
    {
        if (startBowButton.getToggleState() == true)
        {
            audioProcessor.bowEnd = false;
            audioProcessor.bowStart = true;
        }
        else
        {
            audioProcessor.bowEnd = true;
            audioProcessor.bowStart = false;
        }
            
    };
    
    plateMaterialMenu.addItem("Brass", 1);
    plateMaterialMenu.addItem("Bronze", 2);
    plateMaterialMenu.addItem("Iron", 3);
    plateMaterialMenu.addItem("Aluminium", 4);
    plateMaterialMenu.addItem("Gold", 5);
    plateMaterialMenu.addItem("Silver", 6);
    plateMaterialMenu.addItem("Copper", 7);
    plateMaterialMenu.setSelectedId (audioProcessor.plateMaterialId);
    plateMaterialMenu.setSelectedId(1);
    plateMaterialMenu.onChange = [this]
    {
        switch (plateMaterialMenu.getSelectedId())
        {
            case 1: audioProcessor.plateMaterialId = 1; break;
            case 2: audioProcessor.plateMaterialId = 2; break;
            case 3: audioProcessor.plateMaterialId = 3; break;
            case 4: audioProcessor.plateMaterialId = 4; break;
            case 5: audioProcessor.plateMaterialId = 5; break;
            case 6: audioProcessor.plateMaterialId = 6; break;
            case 7: audioProcessor.plateMaterialId = 7; break;
            default: break;
        };
    };
    addAndMakeVisible(plateMaterialMenu);
    plateMaterialMenuLabel.setText("Plate Material:", juce::dontSendNotification);
    plateMaterialMenuLabel.setFont(18.0f);
    plateMaterialMenuLabel.setJustificationType(juce::Justification::left);
    plateMaterialMenuLabel.attachToComponent(&plateMaterialMenu , false);
    
    setSliderAndLabelHorizontal(cylinderLengthSlider, cylinderLengthLabel, "Cylinder Length");
    cylinderLengthSlider.setTextValueSuffix(" m");
    cylinderLengthSlider.setSkewFactor(0.4);
    addAndMakeVisible(cylinderLengthSlider);
    addAndMakeVisible(cylinderLengthLabel);
    
    setSliderAndLabelVertical(cylinderRadiusSlider, cylinderRadiusLabel, "Cylinder Radius");
    cylinderRadiusSlider.setTextValueSuffix("cm");
    cylinderRadiusSlider.setSkewFactor(0.4);
    addAndMakeVisible(cylinderRadiusSlider);
    addAndMakeVisible(cylinderRadiusLabel);
    
    setSliderAndLabelHorizontal(bellLengthSlider, bellLengthLabel, "Bell Length");
    bellLengthSlider.setTextValueSuffix(" m");
    bellLengthSlider.setSkewFactor(0.4);
    addAndMakeVisible(bellLengthSlider);
    addAndMakeVisible(bellLengthLabel);
   
    setSliderAndLabelVertical(bellEndRadiusSlider, bellEndRadiusLabel, "Bell Radius");
    bellEndRadiusSlider.setTextValueSuffix("cm");
    addAndMakeVisible(bellEndRadiusSlider);
    addAndMakeVisible(bellEndRadiusLabel);
    
    bellGrowthMenu.addItem("Linear", 1);
    bellGrowthMenu.addItem("Exponential", 2);
    bellGrowthMenu.addItem("Logarithmic", 3);
    bellGrowthMenu.setSelectedId (audioProcessor.bellGrowthMenuId);
    bellGrowthMenu.setSelectedId(1);
    bellGrowthMenu.onChange = [this]
    {
        switch (bellGrowthMenu.getSelectedId())
        {
            case 1: audioProcessor.bellGrowthMenuId = 1; break;
            case 2: audioProcessor.bellGrowthMenuId = 2; break;
            case 3: audioProcessor.bellGrowthMenuId = 3; break;
            default: break;
        };
    };
    
    
    addAndMakeVisible(bellGrowthMenu);
    bellGrowthLabel.setText("Bell growth curve", juce::dontSendNotification);
    bellGrowthLabel.setFont(15.0f);
    bellGrowthLabel.setJustificationType(juce::Justification::centred);
    bellGrowthLabel.attachToComponent(&bellGrowthMenu , false);
    
    addAndMakeVisible(connTubeToggle);
    //connTubeToggle  .onClick = [this] { updateToggleState (&connTubeToggle,   "Connect")};
    connTubeToggle.setClickingTogglesState (true);
    connTubeLabel.setText("Connect", juce::dontSendNotification);
    connTubeLabel.setFont(15.0f);
    connTubeLabel.setJustificationType(juce::Justification::centred);
    connTubeLabel.attachToComponent(&connTubeToggle , false);
    addAndMakeVisible(connTubeLabel);
    connTubeToggle.onStateChange = [this]
    {
        if (connTubeToggle.getToggleState() == true)
        {
            audioProcessor.tubeConn = true;
        }
        else
        {
            audioProcessor.tubeConn = false;
        }
    };
    
    setResizable(true, true);
    setResizeLimits(400, 250, 1600, 1000);
    getConstrainer() -> setFixedAspectRatio(1.6);
    
    setSize (1200, 1000);
}

PlateAudioProcessorEditor::~PlateAudioProcessorEditor()
{
}

void PlateAudioProcessorEditor::hitButtonClicked()
{
    audioProcessor.hit = true;
}


void PlateAudioProcessorEditor::setSliderAndLabelRotary(juce::Slider& slider, juce::Label& label, const juce::String &Text)
{
    slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    slider.setColour(0x1001311, juce::Colours::purple);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    label.setText(Text, juce::dontSendNotification);
    label.setFont(15.0f);
    label.setJustificationType(juce::Justification::centred);
    label.attachToComponent(&slider , false);
}


void PlateAudioProcessorEditor::setSliderAndLabelHorizontal(juce::Slider& slider, juce::Label& label, const juce::String &Text)
{
    slider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    slider.setColour(0x1001310, juce::Colours::purple);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    label.setText(Text, juce::dontSendNotification);
    label.setFont(15.0f);
    label.setJustificationType(juce::Justification::centred);
    label.attachToComponent(&slider , false);
}


void PlateAudioProcessorEditor::setSliderAndLabelVertical(juce::Slider& slider, juce::Label& label, const juce::String &Text)
{
    slider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    slider.setColour(0x1001310, juce::Colours::purple);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    label.setText(Text, juce::dontSendNotification);
    label.setFont(15.0f);
    label.setJustificationType(juce::Justification::centred);
    label.attachToComponent(&slider , false);
}


//==============================================================================
void PlateAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.fillAll (juce::Colours::black);
    g.setColour (juce::Colours::white);
    g.setFont(20.f);
    
    auto excitationArea = getLocalBounds();
    auto connectionArea = excitationArea.removeFromRight(excitationArea.getWidth()*0.2);
    auto plateArea = excitationArea.removeFromRight(excitationArea.getWidth()*0.4);
    auto dampingArea = plateArea.removeFromTop(plateArea.getHeight()*0.25);
    auto dampingAreaHeigth = dampingArea.getHeight();
    auto plateXYArea = plateArea.removeFromTop(dampingAreaHeigth*1.4);
    plateArea.removeFromLeft(plateArea.getWidth()*0.15);
    juce::Rectangle<int> plateRect (plateArea.getX(),
                                    plateArea.getY(),
                                           lengthXSlider.getValue()*plateArea.getHeight()*0.9,
                                           lengthYSlider.getValue()*plateArea.getHeight()*0.9);
    plateGUIWidth = plateRect.getWidth();
    plateGUIHeight = plateRect.getHeight();
    plateGUIX = plateRect.getX();
    plateGUIY = plateRect.getY();
    
    auto stringLength = plateGUIHeight*stringLengthSlider.getValue();
    numString = numStringSlider.getValue();
    for (int nS=0; nS < numStringSlider.getValue(); ++nS)
    {
        //if (numString % 2)
        //{
            if (nS == 0)
            {
                g.drawLine(plateGUIX + plateGUIWidth * 0.5 , plateGUIY + plateGUIHeight * 0.5 + stringLength * 0.5, plateGUIX + plateGUIWidth * 0.5 , plateGUIY + plateGUIHeight * 0.5 - stringLength * 0.5);
            }
            // If odd number
            else if (nS % 2)
            {
                auto platePosXOdd = plateGUIX + plateGUIWidth * 0.5 + plateGUIWidth * 0.5 * nS/numStringSlider.getValue() * stringPosSpreadSlider.getValue()/100;
                g.drawLine(platePosXOdd, plateGUIY + plateGUIHeight * 0.5 + stringLength * 0.5, platePosXOdd , plateGUIY + plateGUIHeight * 0.5 - stringLength * 0.5);
            }
            // If even number
            else
            {
                auto platePosXEven = plateGUIX + plateGUIWidth * 0.5 - plateGUIWidth * 0.5 * (nS-1)/numStringSlider.getValue() * stringPosSpreadSlider.getValue()/100;
                g.drawLine(platePosXEven, plateGUIY + plateGUIHeight * 0.5 + stringLength * 0.5, platePosXEven , plateGUIY + plateGUIHeight * 0.5 - stringLength * 0.5);
            }
        //}
        /*
        else
        {
            if (nS % 2 || nS == 0)
            {
                auto platePosXOdd =plateGUIX + plateGUIWidth * 0.5 + plateGUIWidth * 0.5 * (nS+1)/numStringSlider.getValue() * stringPosSpreadSlider.getValue()/100;
                g.drawLine(platePosXOdd, plateGUIY + plateGUIHeight * 0.5 + stringLength * 0.5, platePosXOdd , plateGUIY + plateGUIHeight * 0.5 - stringLength * 0.5);
            }
            // If even number
            else
            {
                auto platePosXEven = plateGUIX + plateGUIWidth * 0.5 - plateGUIWidth * 0.5 * (nS)/numStringSlider.getValue() * stringPosSpreadSlider.getValue()/100;
                g.drawLine(platePosXEven, plateGUIY + plateGUIHeight * 0.5 + stringLength * 0.5, platePosXEven , plateGUIY + plateGUIHeight * 0.5 - stringLength * 0.5);
            }
        }
        */
    }
        

    if (malletExcButton.getToggleState() == true)
    {
        bowComponent.setVisible(false);
        hammerComponent.setVisible(true);
        if (hammerDrag == false)
        {
            hammerComponent.setBounds(plateRect.getX()+excXSlider.getValue()*plateRect.getWidth(), plateRect.getY()+excYSlider.getValue()*plateRect.getHeight(), plateArea.getWidth()*0.18, plateArea.getWidth()*0.18);
        }
        
    }
    if (bowExcButton.getToggleState() == true)
    {
        hammerComponent.setVisible(false);
        bowComponent.setVisible(true);
        if (bowDrag == false)
        {
            bowComponent.setBounds(plateRect.getX()+excXSlider.getValue()*plateRect.getWidth(), plateRect.getY()+excYSlider.getValue()*plateRect.getHeight(), plateArea.getWidth(), plateArea.getWidth());
        }
    }
    
    
        /*
    if (malletExcButton.getToggleState() == true)
    {
        g.drawImage(hammer, plateRect.getX()+excXSlider.getValue()*plateRect.getWidth(), plateRect.getY()+excYSlider.getValue()*plateRect.getHeight(), plateArea.getWidth()*0.18, plateArea.getWidth()*0.18, 0, 0, hammer.getWidth(), hammer.getHeight());
    }
    if (bowExcButton.getToggleState() == true)
    {
        g.drawImage(bow, plateRect.getX()+excXSlider.getValue()*plateRect.getWidth(), plateRect.getY()+excYSlider.getValue()*plateRect.getHeight(), plateArea.getWidth(), plateArea.getWidth(), 0, 0, bow.getWidth(), bow.getHeight());
    }
     */
    g.drawRect (plateRect, 2);
}
/*
void PlateAudioProcessorEditor::mouseDown(const juce::MouseEvent& mE)
{
    if (malletExcButton.getToggleState() == true)
    {
        componentDragger.startDraggingComponent(&hammerComponent, mE);
        hammerDrag = true;
        //excXSlider.setValue((hammerComponent.getX()-plateGUIX)/plateGUIWidth);
    }
    if (bowExcButton.getToggleState() == true)
    {
        componentDragger.startDraggingComponent(&bowComponent, mE);
        bowDrag = true;
    }
}

void PlateAudioProcessorEditor::mouseDrag(const juce::MouseEvent& mE)
{
    if (malletExcButton.getToggleState() == true)
    {
        componentDragger.dragComponent(&hammerComponent, mE, nullptr);
        hammerDrag = true;
        //excXSlider.setValue((hammerComponent.getX()-plateGUIX)/plateGUIWidth);
    }
    if (bowExcButton.getToggleState() == true)
    {
        componentDragger.dragComponent(&bowComponent, mE, nullptr);
        bowDrag = true;
    }
}

void PlateAudioProcessorEditor::mouseUp(const juce::MouseEvent& mE)
{
    hammerDrag = false;
    bowDrag = false;
}*/

void PlateAudioProcessorEditor::resized()
{
    auto excitationArea = getLocalBounds();
    auto connectionArea = excitationArea.removeFromRight(excitationArea.getWidth()*0.2);
    auto plateArea = excitationArea.removeFromRight(excitationArea.getWidth()*0.4);
    auto dampingArea = plateArea.removeFromTop(plateArea.getHeight()*0.25);
    auto dampingAreaHeigth = dampingArea.getHeight();
    auto plateMaterialArea = connectionArea.removeFromBottom(connectionArea.getHeight()*0.15);
    dampingArea.removeFromTop(dampingAreaHeigth*0.3);
    auto rotaryWidth = dampingArea.getWidth()*0.33;
    sig0Slider.setBounds(dampingArea.removeFromLeft(rotaryWidth));
    sig1Slider.setBounds(dampingArea.removeFromLeft(rotaryWidth));
    auto thicknessArea = dampingArea.removeFromRight(dampingArea.getWidth());
    auto plateXYArea = plateArea.removeFromTop(dampingAreaHeigth*1.2);
    plateXYArea.removeFromTop(dampingAreaHeigth*0.3);
    thicknessSlider.setBounds(thicknessArea.removeFromTop(dampingAreaHeigth));
    lengthXSlider.setBounds(plateXYArea.removeFromLeft(plateXYArea.getWidth()*0.5));
    lengthYSlider.setBounds(plateXYArea);
    if (stringConnButton.getToggleState() == true)
    {
        numStringSlider.setVisible(true);
        stringLengthSlider.setVisible(true);
        stringPosSpreadSlider.setVisible(true);
        stringTensionSlider.setVisible(true);
        stringTensionDiffSlider.setVisible(true);
        sSig0Slider.setVisible(true);
        stringRadiusSlider.setVisible(true);
        cylinderLengthSlider.setVisible(false);
        bellLengthSlider.setVisible(false);
        cylinderRadiusSlider.setVisible(false);
        bellEndRadiusSlider.setVisible(false);
        bellGrowthMenu.setVisible(false);
        connTubeToggle.setVisible(false);

        connectionArea.removeFromTop(connectionArea.getHeight()*0.05);
        numStringSlider.setBounds(connectionArea.removeFromTop(connectionArea.getHeight()*0.2));
        auto stringLengthArea = connectionArea.removeFromTop(connectionArea.getHeight()*0.33);
        stringLengthArea.removeFromTop(stringLengthArea.getHeight()*0.2);
        stringLengthSlider.setBounds(stringLengthArea.removeFromLeft(stringLengthArea.getWidth()*0.5));
        stringPosSpreadSlider.setBounds(stringLengthArea);
        auto tensionArea = connectionArea.removeFromTop(connectionArea.getHeight()*0.5);
        tensionArea.removeFromTop(tensionArea.getHeight()*0.2);
        stringTensionSlider.setBounds(tensionArea.removeFromLeft(tensionArea.getWidth()*0.5));
        stringTensionDiffSlider.setBounds(tensionArea);
        connectionArea.removeFromTop(connectionArea.getHeight()*0.2);
        sSig0Slider.setBounds(connectionArea.removeFromLeft(connectionArea.getWidth()*0.5));
        stringRadiusSlider.setBounds(connectionArea);
        //auto listeningArea = plateArea.removeFromTop(dampingAreaHeigth*1.2);
        //listeningArea.removeFromTop(dampingAreaHeigth*0.3);
        //lisXSlider.setBounds(listeningArea.removeFromLeft(listeningArea.getWidth()*0.5));
        //lisYSlider.setBounds(listeningArea);
        //plateArea.removeFromTop(dampingAreaHeigth*0.3);
    }
    if (tubeConnButton.getToggleState() == true)
    {
        numStringSlider.setVisible(false);
        stringLengthSlider.setVisible(false);
        stringPosSpreadSlider.setVisible(false);
        stringTensionSlider.setVisible(false);
        stringTensionDiffSlider.setVisible(false);
        sSig0Slider.setVisible(false);
        stringRadiusSlider.setVisible(false);
        cylinderLengthSlider.setVisible(true);
        bellLengthSlider.setVisible(true);
        cylinderRadiusSlider.setVisible(true);
        bellEndRadiusSlider.setVisible(true);
        bellGrowthMenu.setVisible(true);
        connTubeToggle.setVisible(true);
        
        //connectionArea.removeFromTop(connectionArea.getHeight()*0.05);
        auto boreLengthArea = connectionArea.removeFromTop(connectionArea.getHeight()*0.33);
        boreLengthArea.removeFromTop(boreLengthArea.getHeight()*0.2);
        cylinderLengthSlider.setBounds(boreLengthArea.removeFromLeft(boreLengthArea.getWidth()*0.5));
        bellLengthSlider.setBounds(boreLengthArea);
        auto tubeRadiusArea = connectionArea.removeFromTop(connectionArea.getHeight()*0.5);
        tubeRadiusArea.removeFromTop(tubeRadiusArea.getHeight()*0.2);
        cylinderRadiusSlider.setBounds(tubeRadiusArea.removeFromLeft(tubeRadiusArea.getWidth()*0.5));
        bellEndRadiusSlider.setBounds(tubeRadiusArea);
        connectionArea.removeFromTop(connectionArea.getHeight()*0.2);
        bellGrowthMenu.setBounds(connectionArea.removeFromRight(connectionArea.getWidth()*0.75));

        connTubeToggle.setBounds(connectionArea.removeFromTop(connectionArea.getHeight()*0.5));
        
    }
    plateMaterialArea.removeFromTop(plateMaterialArea.getHeight()*0.2);
    plateMaterialMenu.setBounds(plateMaterialArea.removeFromRight(plateMaterialArea.getWidth()*0.5));
    springConnButton.setBounds(plateMaterialArea.removeFromTop(plateMaterialArea.getHeight()*0.25));
    rigidConnButton.setBounds(plateMaterialArea.removeFromTop(plateMaterialArea.getHeight()*0.33));
    stringConnButton.setBounds(plateMaterialArea.removeFromTop(plateMaterialArea.getHeight()*0.5));
    tubeConnButton.setBounds(plateMaterialArea);
    
    
    auto excitationTypeArea = excitationArea.removeFromTop(excitationArea.getHeight()*0.1);
    bowExcButton.setBounds(excitationTypeArea.removeFromLeft(excitationArea.getWidth()*0.5));
    malletExcButton.setBounds(excitationTypeArea);
    if (bowExcButton.getToggleState() == true)
    {
        hitButton.setVisible(false);
        excTSlider.setVisible(false);
        excFSlider.setVisible(false);
        vBSlider.setVisible(true);
        fBSlider.setVisible(true);
        startBowButton.setVisible(true);
        fricSlider.setVisible(true);
        bAtt1Slider.setVisible(true);
        bDec1Slider.setVisible(true);
        bSus1Slider.setVisible(true);
        bRel1Slider.setVisible(true);
        FBEnv1Slider.setVisible(true);
        vBEnv1Slider.setVisible(true);
        lfoRateSlider.setVisible(true);
        xPosLFOModSlider.setVisible(true);
        yPosLFOModSlider.setVisible(true);
        repaint();

        auto ffvBArea = excitationArea.removeFromTop(dampingAreaHeigth);
        ffvBArea.removeFromTop(dampingAreaHeigth*0.3);
        vBSlider.setBounds(ffvBArea.removeFromLeft(excitationArea.getWidth()*0.33));
        fBSlider.setBounds(ffvBArea.removeFromLeft(excitationArea.getWidth()*0.5));
        fricSlider.setBounds(ffvBArea);
        
        auto ADSR11Area = excitationArea.removeFromBottom(excitationArea.getHeight()*0.5);
        ADSR11Area.removeFromTop(dampingAreaHeigth*0.3);
        auto LFOArea = ADSR11Area.removeFromRight(ADSR11Area.getWidth()*0.5);
        auto envSliderWidth = 0.25 * ADSR11Area.getWidth();
        bAtt1Slider.setBounds(ADSR11Area.removeFromLeft(envSliderWidth));
        bDec1Slider.setBounds(ADSR11Area.removeFromLeft(envSliderWidth));
        bSus1Slider.setBounds(ADSR11Area.removeFromLeft(envSliderWidth));
        bRel1Slider.setBounds(ADSR11Area.removeFromLeft(envSliderWidth));
        auto lfoSliderWidth = 0.33 * LFOArea.getWidth();
        lfoRateSlider.setBounds(LFOArea.removeFromLeft(lfoSliderWidth));
        xPosLFOModSlider.setBounds(LFOArea.removeFromLeft(lfoSliderWidth));
        yPosLFOModSlider.setBounds(LFOArea.removeFromLeft(lfoSliderWidth));
        //FBEnv1Slider.setBounds(ADSR12Area.removeFromLeft(ADSR12Area.getWidth()*0.5));
        //vBEnv1Slider.setBounds(ADSR12Area);
        excitationArea.removeFromTop(dampingAreaHeigth*0.3);
        auto startBowArea = excitationArea.removeFromRight(excitationArea.getWidth()*0.33);
        startBowButton.setBounds(startBowArea);
        excXSlider.setBounds(excitationArea.removeFromLeft(excitationArea.getWidth()*0.5));
        excYSlider.setBounds(excitationArea);
    }
    
    if (malletExcButton.getToggleState() == true)
    {
        vBSlider.setVisible(false);
        fBSlider.setVisible(false);
        fricSlider.setVisible(false);
        bAtt1Slider.setVisible(false);
        bDec1Slider.setVisible(false);
        bSus1Slider.setVisible(false);
        bRel1Slider.setVisible(false);
        FBEnv1Slider.setVisible(false);
        vBEnv1Slider.setVisible(false);
        lfoRateSlider.setVisible(false);
        xPosLFOModSlider.setVisible(false);
        yPosLFOModSlider.setVisible(false);
        startBowButton.setVisible(false);
        hitButton.setVisible(true);
        excTSlider.setVisible(true);
        excFSlider.setVisible(true);
        repaint();
        auto excFTArea = excitationArea.removeFromTop(dampingAreaHeigth);
        excFTArea.removeFromTop(dampingAreaHeigth*0.3);
        excFSlider.setBounds(excFTArea.removeFromLeft(excitationArea.getWidth()*0.5));
        excTSlider.setBounds(excFTArea);
        auto hitArea = excitationArea.removeFromTop(dampingAreaHeigth);
        hitArea.removeFromTop(dampingAreaHeigth*0.3);
        hitButton.setBounds(hitArea);
        excitationArea.removeFromTop(dampingAreaHeigth*0.3);
        excXSlider.setBounds(excitationArea.removeFromLeft(excitationArea.getWidth()*0.5));
        excYSlider.setBounds(excitationArea);
    }

}
