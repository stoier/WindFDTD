/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "WindFDTDVoice.h"

//==============================================================================
WindFDTDpluginAudioProcessorEditor::WindFDTDpluginAudioProcessorEditor (WindFDTDpluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
cylinderLengthSliderAttachment(audioProcessor.tree, "Bore Scale", cylinderLengthSlider),
cylinderRadiusSliderAttachment(audioProcessor.tree, "Cylinder Radius", cylinderRadiusSlider),
bellLengthSliderAttachment(audioProcessor.tree, "Cylinder Bell Ratio", bellLengthSlider),
bellEndRadiusSliderAttachment(audioProcessor.tree, "Bell Radius", bellEndRadiusSlider),
reedMassSliderAttachment(audioProcessor.tree, "Reed Mass", reedMassSlider),
reedWidthSliderAttachment(audioProcessor.tree, "Reed Width", reedWidthSlider),
pressureSliderAttachment(audioProcessor.tree, "Pressure", pressureSlider),
attackSliderAttachment(audioProcessor.tree, "Attack", attackSlider), decaySliderAttachment(audioProcessor.tree, "Decay", decaySlider), sustainSliderAttachment(audioProcessor.tree, "Sustain", sustainSlider), releaseSliderAttachment(audioProcessor.tree, "Release", releaseSlider), vibRateSliderAttachment(audioProcessor.tree, "Vibrato Rate", vibRateSlider), vibAmountSliderAttachment(audioProcessor.tree, "Vibrato Amount", vibAmountSlider)
{
    setSliderAndLabelHorizontal(cylinderLengthSlider, cylinderLengthLabel, "Bore scale");
    cylinderLengthSlider.setSkewFactor(0.4);
    addAndMakeVisible(cylinderLengthSlider);
    addAndMakeVisible(cylinderLengthLabel);
    
    setSliderAndLabelVertical(cylinderRadiusSlider, cylinderRadiusLabel, "Cylinder Radius");
    cylinderRadiusSlider.setTextValueSuffix("cm");
    cylinderRadiusSlider.setSkewFactor(0.4);
    addAndMakeVisible(cylinderRadiusSlider);
    addAndMakeVisible(cylinderRadiusLabel);
    
    setSliderAndLabelHorizontal(bellLengthSlider, bellLengthLabel, "Cylinder Bell Ratio");
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
    bellGrowthLabel.setText("Bell growth curve", dontSendNotification);
    bellGrowthLabel.setFont(15.0f);
    bellGrowthLabel.setJustificationType(juce::Justification::centred);
    bellGrowthLabel.attachToComponent(&bellGrowthMenu , false);
    
    
    pressureMultiply.addItem("x 1", 1);
    pressureMultiply.addItem("x 2", 2);
    pressureMultiply.addItem("x 5", 3);
    pressureMultiply.addItem("x 10", 4);
    pressureMultiply.addItem("x 100", 5);
    pressureMultiply.setSelectedId (audioProcessor.pressureMultMenuId);
    pressureMultiply.setSelectedId(1);
    pressureMultiply.onChange = [this]
    {
        switch (pressureMultiply.getSelectedId())
        {
            case 1: audioProcessor.pressureMultMenuId = 1; break;
            case 2: audioProcessor.pressureMultMenuId = 2; break;
            case 3: audioProcessor.pressureMultMenuId = 5; break;
            case 4: audioProcessor.pressureMultMenuId = 10; break;
            case 5: audioProcessor.pressureMultMenuId = 100; break;
            default: break;
        };
    };
    addAndMakeVisible(pressureMultiply);
    pressureMultLabel.setText("Pressure multiplier", dontSendNotification);
    pressureMultLabel.setFont(15.0f);
    pressureMultLabel.setJustificationType(juce::Justification::centred);
    pressureMultLabel.attachToComponent(&pressureMultiply , false);
    
    
    setSliderAndLabelRotary(reedMassSlider, reedMassLabel, "Reed mass");
    reedMassSlider.setTextValueSuffix(" g");
    addAndMakeVisible(reedMassSlider);
    addAndMakeVisible(reedMassLabel);
    
    setSliderAndLabelRotary(reedWidthSlider, reedWidthLabel, "Reed width");
    reedWidthSlider.setTextValueSuffix(" mm");
    addAndMakeVisible(reedWidthSlider);
    addAndMakeVisible(reedWidthLabel);
    
    setSliderAndLabelRotary(pressureSlider, pressureLabel, "Mouth pressure");
    pressureSlider.setTextValueSuffix(" Pa");
    addAndMakeVisible(pressureSlider);
    addAndMakeVisible(pressureLabel);
    
    setSliderAndLabelVertical(attackSlider, attackLabel, "Attack");
    attackSlider.setTextValueSuffix(" ms");
    addAndMakeVisible(attackSlider);
    addAndMakeVisible(attackLabel);
    
    setSliderAndLabelVertical(decaySlider, decayLabel, "Decay");
    decaySlider.setTextValueSuffix(" ms");
    addAndMakeVisible(decaySlider);
    addAndMakeVisible(decayLabel);
    
    setSliderAndLabelVertical(sustainSlider, sustainLabel, "Sustain");
    addAndMakeVisible(sustainSlider);
    addAndMakeVisible(sustainLabel);
    
    setSliderAndLabelVertical(releaseSlider, releaseLabel, "Release");
    releaseSlider.setTextValueSuffix(" ms");
    addAndMakeVisible(releaseSlider);
    addAndMakeVisible(releaseLabel);
    
    setSliderAndLabelRotary(vibRateSlider, vibRateLabel, "Vibrato rate");
    vibRateSlider.setTextValueSuffix(" Hz");
    addAndMakeVisible(vibRateSlider);
    addAndMakeVisible(vibRateLabel);
    
    setSliderAndLabelRotary(vibAmountSlider, vibAmountLabel, "Vibrato amount");
    vibAmountSlider.setTextValueSuffix(" St");
    addAndMakeVisible(vibAmountSlider);
    addAndMakeVisible(vibAmountLabel);
    
    
    setResizable(true, true);
    setResizeLimits(300, 250, 1200, 1000);
    getConstrainer() -> setFixedAspectRatio(1.2);
    
    
    
    setSize (600, 500);
}

WindFDTDpluginAudioProcessorEditor::~WindFDTDpluginAudioProcessorEditor()
{
}

void WindFDTDpluginAudioProcessorEditor::setSliderAndLabelRotary(juce::Slider& slider, juce::Label& label, const String &Text)
{
    slider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    slider.setColour(0x1001311, juce::Colours::purple);
    slider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
    label.setText(Text, dontSendNotification);
    label.setFont(15.0f);
    label.setJustificationType(juce::Justification::centred);
    label.attachToComponent(&slider , false);
}


void WindFDTDpluginAudioProcessorEditor::setSliderAndLabelHorizontal(juce::Slider& slider, juce::Label& label, const String &Text)
{
    slider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    slider.setColour(0x1001310, juce::Colours::purple);
    slider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
    label.setText(Text, dontSendNotification);
    label.setFont(15.0f);
    label.setJustificationType(juce::Justification::centred);
    label.attachToComponent(&slider , false);
}


void WindFDTDpluginAudioProcessorEditor::setSliderAndLabelVertical(juce::Slider& slider, juce::Label& label, const String &Text)
{
    slider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    slider.setColour(0x1001310, juce::Colours::purple);
    slider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
    label.setText(Text, dontSendNotification);
    label.setFont(15.0f);
    label.setJustificationType(juce::Justification::centred);
    label.attachToComponent(&slider , false);
}


//==============================================================================
void WindFDTDpluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.fillAll (juce::Colours::black);
    g.setColour (juce::Colours::white);
    g.setFont(20.f);
    
}

void WindFDTDpluginAudioProcessorEditor::resized()
{
    auto boreArea = getLocalBounds();
    auto reedArea = boreArea.removeFromTop(boreArea.getHeight()*0.66);
    auto pressureArea = reedArea.removeFromLeft(reedArea.getWidth()*0.66);
    auto boreLengthArea = boreArea.removeFromLeft(boreArea.getWidth()*0.33);
    auto vibArea = reedArea.removeFromBottom(reedArea.getHeight()*0.36);
    vibArea = vibArea.removeFromTop(vibArea.getHeight()*0.80);
    boreLengthArea.removeFromLeft(boreLengthArea.getWidth()*0.16);
    cylinderLengthSlider.setBounds(boreLengthArea.removeFromTop(boreLengthArea.getHeight()*0.25));
    boreLengthArea.removeFromTop(boreLengthArea.getHeight()*0.33);
    bellLengthSlider.setBounds(boreLengthArea.removeFromTop(boreLengthArea.getHeight()*0.5));
    boreArea.removeFromBottom(boreArea.getHeight()*0.25);
    cylinderRadiusSlider.setBounds(boreArea.removeFromLeft(boreArea.getWidth()*0.33));
    bellEndRadiusSlider.setBounds(boreArea.removeFromLeft(boreArea.getWidth()*0.33));
    boreArea.removeFromRight(boreArea.getWidth()*0.2);
    bellGrowthMenu.setBounds(boreArea);

    reedArea.removeFromTop(reedArea.getHeight()*0.24);
    reedArea.removeFromBottom(reedArea.getHeight()*0.33);
    reedWidthSlider.setBounds(reedArea.removeFromRight(reedArea.getWidth()*0.5));
    reedMassSlider.setBounds(reedArea);
    pressureArea.removeFromTop(pressureArea.getHeight()*0.15);
    auto pressureMultArea = pressureArea.removeFromBottom(pressureArea.getHeight()*0.33);
    pressureArea.removeFromBottom(pressureArea.getHeight()*0.1);
    auto mouthPressureArea = pressureArea.removeFromLeft(pressureArea.getWidth()*0.33);
    auto pressureAdsrArea = pressureArea;
    attackSlider.setBounds(pressureAdsrArea.removeFromLeft(pressureAdsrArea.getWidth()*0.20));
    decaySlider.setBounds(pressureAdsrArea.removeFromLeft(pressureAdsrArea.getWidth()*0.40));
    sustainSlider.setBounds(pressureAdsrArea.removeFromLeft(pressureAdsrArea.getWidth()*0.40));
    releaseSlider.setBounds(pressureAdsrArea);
    pressureMultArea.removeFromBottom(pressureMultArea.getHeight()*0.4);
    pressureMultArea.removeFromRight(pressureMultArea.getWidth()*0.6);
    pressureMultArea.removeFromLeft(pressureMultArea.getWidth()*0.2);
    mouthPressureArea.removeFromTop(mouthPressureArea.getHeight()*0.16);
    pressureSlider.setBounds(mouthPressureArea.removeFromTop(mouthPressureArea.getHeight()*0.9));
    pressureMultArea.removeFromTop(pressureMultArea.getHeight()*0.25);
    pressureMultiply.setBounds(pressureMultArea);
    vibRateSlider.setBounds(vibArea.removeFromLeft(vibArea.getWidth()*0.5));
    vibAmountSlider.setBounds(vibArea);
}
    

