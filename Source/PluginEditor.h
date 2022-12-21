/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class WindFDTDpluginAudioProcessorEditor  : public juce::AudioProcessorEditor//, public KeyListener//, public Slider::Listener
{
public:
    WindFDTDpluginAudioProcessorEditor (WindFDTDpluginAudioProcessor&);
    ~WindFDTDpluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
        
    void buttonStateChanged (Button* button);

    void setSliderAndLabelRotary(juce::Slider& slider, juce::Label& label, const String &Text);

    void setSliderAndLabelHorizontal(juce::Slider& slider, juce::Label& label, const String &Text);

    void setSliderAndLabelVertical(juce::Slider& slider, juce::Label& label, const String &Text);
    

private:

    WindFDTDpluginAudioProcessor& audioProcessor;
    
    Slider cylinderLengthSlider, cylinderRadiusSlider, bellLengthSlider, bellEndRadiusSlider, pressureSlider, attackSlider, decaySlider, sustainSlider, releaseSlider, vibRateSlider, vibAmountSlider;
    Label cylinderLengthLabel, cylinderRadiusLabel, bellLengthLabel, bellEndRadiusLabel, pressureLabel, attackLabel, decayLabel, sustainLabel, releaseLabel, vibRateLabel, vibAmountLabel, bellGrowthLabel, pressureMultLabel;
    
    ComboBox bellGrowthMenu, pressureMultiply;
    
    
    
    Slider reedMassSlider, reedWidthSlider;
    Label reedMassLabel, reedWidthLabel;
    
    using APVTS = juce::AudioProcessorValueTreeState;
    using sliderAttachment = APVTS::SliderAttachment;
    
    sliderAttachment cylinderLengthSliderAttachment, cylinderRadiusSliderAttachment, bellLengthSliderAttachment, bellEndRadiusSliderAttachment, reedMassSliderAttachment, reedWidthSliderAttachment, pressureSliderAttachment, attackSliderAttachment, decaySliderAttachment, sustainSliderAttachment, releaseSliderAttachment, vibRateSliderAttachment, vibAmountSliderAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WindFDTDpluginAudioProcessorEditor)
};
