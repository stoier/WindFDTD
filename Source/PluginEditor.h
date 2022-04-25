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
    
    //void sliderValueChanged (Slider* slider) override;
    
    void buttonStateChanged (Button* button);
    
    //void setSliderAndLabelRotary(juce::Slider& slider, juce::Label& label, double minLim, double maxLim, double step, double defaultValue, const String &Text);
    void setSliderAndLabelRotary(juce::Slider& slider, juce::Label& label, const String &Text);
    
    //void setSliderAndLabelHorizontal(juce::Slider& slider, juce::Label& label, double minLim, double maxLim, double step, double defaultValue, const String &Text);
    void setSliderAndLabelHorizontal(juce::Slider& slider, juce::Label& label, const String &Text);
    //void setSliderAndLabelVertical(juce::Slider& slider, juce::Label& label, double minLim, double maxLim, double step, double defaultValue, const String &Text);
    void setSliderAndLabelVertical(juce::Slider& slider, juce::Label& label, const String &Text);

    //bool keyPressed (const KeyPress& key, Component* originatingComponent) override;
private:
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    WindFDTDpluginAudioProcessor& audioProcessor;
    
    Slider cylinderLengthSlider, cylinderRadiusSlider, bellLengthSlider, bellEndRadiusSlider;
    Label cylinderLengthLabel, cylinderRadiusLabel, bellLengthLabel, bellEndRadiusLabel;
    
    ComboBox bellGrowthMenu;
    
    Slider reedMassSlider, reedWidthSlider;
    Label reedMassLabel, reedWidthLabel;
    
    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;
    
    Attachment cylinderLengthSliderAttachment, cylinderRadiusSliderAttachment, bellLengthSliderAttachment, bellEndRadiusSliderAttachment, reedMassSliderAttachment, reedWidthSliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WindFDTDpluginAudioProcessorEditor)
};
