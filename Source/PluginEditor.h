/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ThinPlate.h"


//==============================================================================
/**
*/

class HammerComponent   : public juce::Component
{
public:
    HammerComponent() {}
    
    void paint (juce::Graphics& g) override
    {
        g.drawImage(hammer, 0, 0, getWidth(), getHeight(), 0, 0, hammer.getWidth(), hammer.getHeight());
    }

    juce::Image hammer = juce::ImageCache::getFromMemory (BinaryData::Hammer_png, BinaryData::Hammer_pngSize);
};

class BowComponent   : public juce::Component
{
public:
    BowComponent() {}
    
    void paint (juce::Graphics& g) override
    {
        g.drawImage(bow, 0, 0, getWidth(), getHeight(), 0, 0, bow.getWidth(), bow.getHeight());

    }
    juce::Image bow = juce::ImageCache::getFromMemory (BinaryData::Bow_png, BinaryData::Bow_pngSize);

};

class PlateAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Slider::Listener
{
public:
    PlateAudioProcessorEditor (PlateAudioProcessor&);
    ~PlateAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void setSliderAndLabelRotary(juce::Slider& slider, juce::Label& label, const juce::String &Text);

    void setSliderAndLabelHorizontal(juce::Slider& slider, juce::Label& label, const juce::String &Text);

    void setSliderAndLabelVertical(juce::Slider& slider, juce::Label& label, const juce::String &Text);
    
    /*
    void mouseDown(const juce::MouseEvent& mE) override;
    
    void mouseDrag(const juce::MouseEvent& mE) override;
    
    void mouseUp(const juce::MouseEvent& mE) override;
    */
    
    void sliderValueChanged (juce::Slider* slider) override
    {
        repaint();
    }
    void hitButtonClicked();
    
    void bowExcButtonClicked();
    
    void malletExcButtonClicked();
    
private:

    PlateAudioProcessor& audioProcessor;
    
    using APVTS = juce::AudioProcessorValueTreeState;
    using sliderAttachment = APVTS::SliderAttachment;
    
    juce::Slider sig0Slider, sig1Slider, lengthXSlider, lengthYSlider, excXSlider, excYSlider, lisXSlider, lisYSlider, excFSlider, excTSlider, thicknessSlider, vBSlider, fBSlider, fricSlider, bAtt1Slider, bDec1Slider, bSus1Slider, bRel1Slider, FBEnv1Slider, vBEnv1Slider, xPosLFOModSlider, yPosLFOModSlider, lfoRateSlider, numStringSlider, stringTensionDiffSlider, stringLengthSlider, stringRadiusSlider, stringTensionSlider, stringPosSpreadSlider, stringPosOffSetSlider, sSig0Slider, cylinderLengthSlider, cylinderRadiusSlider, bellLengthSlider, bellEndRadiusSlider;
    
    juce::Label sig0Label, sig1Label, lengthXLabel, lengthYLabel, excXLabel, excYLabel, lisXLabel, lisYLabel, excFLabel, excTLabel, thicknessLabel, plateMaterialMenuLabel, vBLabel, fBLabel, fricLabel, bAtt1Label, bDec1Label, bSus1Label, bRel1Label, FBEnv1Label, vBEnv1Label, xPosLFOModLabel, yPosLFOModLabel, lfoRateLabel, numStringLabel, stringTensionDiffLabel, stringLengthLabel, stringRadiusLabel, stringTensionLabel, stringPosSpreadLabel, stringPosOffSetLabel, sSig0Label, cylinderLengthLabel, cylinderRadiusLabel, bellLengthLabel, bellEndRadiusLabel, bellGrowthLabel, connTubeLabel;
    
    juce::ComboBox plateMaterialMenu, bellGrowthMenu;
    
    sliderAttachment sig0SliderAttachment, sig1SliderAttachment, lengthXSliderAttachment, lengthYSliderAttachment, excXSliderAttachment, excYSliderAttachment, lisXSliderAttachment, lisYSliderAttachment, thicknessSliderAttachment, excFSliderAttachment, excTSliderAttachment, vBSliderAttachment, fBSliderAttachment, fricSliderAttachment, bAtt1SliderAttachment, bDec1SliderAttachment, bSus1SliderAttachment, bRel1SliderAttachment, FBEnv1SliderAttachment, vBEnv1SliderAttachment, xPosLFOModSliderAttachment, yPosLFOModSliderAttachment, lfoRateSliderAttachment, numStringSliderAttachment, stringTensionDiffSliderAttachment, stringLengthSliderAttachment, stringRadiusSliderAttachment, stringTensionSliderAttachment, stringPosSpreadSliderAttachment, sSig0SliderAttachment, cylinderLengthSliderAttachment, cylinderRadiusSliderAttachment, bellLengthSliderAttachment, bellEndRadiusSliderAttachment;
    
    juce::TextButton hitButton{"Hit plate"}, bowExcButton{"Bow"}, malletExcButton{"Mallet"}, linkFBvB{"Link"}, startBowButton{"Start bowing"}, stringConnButton{"Connect strings"}, tubeConnButton{"Connect tube"}, rigidConnButton{"Rigid Connection"}, springConnButton{"Spring Connection"};

    juce::ToggleButton connTubeToggle;
    
    bool hammerDrag = false;
    bool bowDrag = false;
    
    int plateGUIWidth, plateGUIHeight, plateGUIX, plateGUIY, numString;
    
    //juce::Image hammer = juce::ImageCache::getFromMemory (BinaryData::Hammer_png, BinaryData::Hammer_pngSize);
    //juce::Image bow = juce::ImageCache::getFromMemory (BinaryData::Bow_png, BinaryData::Bow_pngSize);
    
    juce::ComponentDragger componentDragger;
    
    HammerComponent hammerComponent;
    BowComponent bowComponent;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlateAudioProcessorEditor)
};

