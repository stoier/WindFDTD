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
//cylinderLengthSliderAttachment(audioProcessor.tree, "Cylinder Length", cylinderLengthSlider),
cylinderLengthSliderAttachment(audioProcessor.tree, "Bore Scale", cylinderLengthSlider),
cylinderRadiusSliderAttachment(audioProcessor.tree, "Cylinder Radius", cylinderRadiusSlider),
//bellLengthSliderAttachment(audioProcessor.tree, "Bell Length", bellLengthSlider),
bellLengthSliderAttachment(audioProcessor.tree, "Cylinder Bell Ratio", bellLengthSlider),
bellEndRadiusSliderAttachment(audioProcessor.tree, "Bell Radius", bellEndRadiusSlider),
reedMassSliderAttachment(audioProcessor.tree, "Reed Mass", reedMassSlider),
reedWidthSliderAttachment(audioProcessor.tree, "Reed Width", reedWidthSlider),
pressureSliderAttachment(audioProcessor.tree, "Pressure", pressureSlider),
attackSliderAttachment(audioProcessor.tree, "Attack", attackSlider), decaySliderAttachment(audioProcessor.tree, "Decay", decaySlider), sustainSliderAttachment(audioProcessor.tree, "Sustain", sustainSlider), releaseSliderAttachment(audioProcessor.tree, "Release", releaseSlider), vibRateSliderAttachment(audioProcessor.tree, "Vibrato Rate", vibRateSlider), vibAmountSliderAttachment(audioProcessor.tree, "Vibrato Amount", vibAmountSlider)
//bellGrowthAttachment(audioProcessor.tree, "Bell Growth", bellGrowthMenu)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    //setSliderAndLabelHorizontal(cylinderLengthSlider, cylinderLengthLabel, 1, 1000, 1, audioProcessor.cylinderLength, "Cylinder length");
    //setSliderAndLabelHorizontal(cylinderLengthSlider, cylinderLengthLabel, "Cylinder length");
    setSliderAndLabelHorizontal(cylinderLengthSlider, cylinderLengthLabel, "Bore scale");
    //cylinderLengthSlider.setTextValueSuffix("cm");
    cylinderLengthSlider.setSkewFactor(0.4);
    addAndMakeVisible(cylinderLengthSlider);
    addAndMakeVisible(cylinderLengthLabel);
    //addKeyListener (this);
    //cylinderLengthSlider.addListener(this);
    
    //setSliderAndLabelVertical(cylinderRadiusSlider, cylinderRadiusLabel, 1, 100, 1, audioProcessor.cylinderRadius, "Cylinder Radius");
    setSliderAndLabelVertical(cylinderRadiusSlider, cylinderRadiusLabel, "Cylinder Radius");
    cylinderRadiusSlider.setTextValueSuffix("cm");
    cylinderRadiusSlider.setSkewFactor(0.4);
    addAndMakeVisible(cylinderRadiusSlider);
    addAndMakeVisible(cylinderRadiusLabel);
    //faddKeyListener (this);
    //cylinderRadiusSlider.addListener(this);
    
    //setSliderAndLabelHorizontal(bellLengthSlider, bellLengthLabel, 1, 500, 1, audioProcessor.bellLength, "Bell length");
    //setSliderAndLabelHorizontal(bellLengthSlider, bellLengthLabel, "Bell length");
    //bellLengthSlider.setTextValueSuffix("cm");
    setSliderAndLabelHorizontal(bellLengthSlider, bellLengthLabel, "Cylinder Bell Ratio");
    //bellLengthSlider.setSkewFactor(0.4);
    addAndMakeVisible(bellLengthSlider);
    addAndMakeVisible(bellLengthLabel);
    //faddKeyListener (this);
    //bellLengthSlider.addListener(this);
    
    //setSliderAndLabelVertical(bellEndRadiusSlider, bellEndRadiusLabel, 1, 100, 1, audioProcessor.bellEndRadius, "Bell Radius");
    setSliderAndLabelVertical(bellEndRadiusSlider, bellEndRadiusLabel, "Bell Radius");
    bellEndRadiusSlider.setTextValueSuffix("cm");
    //bellEndRadiusSlider.setSkewFactor(0.4);
    addAndMakeVisible(bellEndRadiusSlider);
    addAndMakeVisible(bellEndRadiusLabel);
    //faddKeyListener (this);
    //bellEndRadiusSlider.addListener(this);
    
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
    
    
    //setSliderAndLabelRotary(reedMassSlider, reedMassLabel, 0.1, 50, 0.1, audioProcessor.reedMass, "Reed mass");
    setSliderAndLabelRotary(reedMassSlider, reedMassLabel, "Reed mass");
    reedMassSlider.setTextValueSuffix(" g");
    //reedMassSlider.setSkewFactor(0.50);
    addAndMakeVisible(reedMassSlider);
    addAndMakeVisible(reedMassLabel);
    
    //setSliderAndLabelRotary(reedWidthSlider, reedWidthLabel, 0.1, 50, 0.1, audioProcessor.reedWidth, "Reed width");
    setSliderAndLabelRotary(reedWidthSlider, reedWidthLabel, "Reed width");
    reedWidthSlider.setTextValueSuffix(" mm");
    //reedMassSlider.setSkewFactor(0.50);
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

//void Wind_FDTD_pluginAudioProcessorEditor::setSliderAndLabelRotary(juce::Slider& slider, juce::Label& label, double minLim, double maxLim, double step, double defaultValue, const String &Text)
void WindFDTDpluginAudioProcessorEditor::setSliderAndLabelRotary(juce::Slider& slider, juce::Label& label, const String &Text)
{
    slider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    //slider.setColour(0x1001300, juce::Colours::aliceblue);
    slider.setColour(0x1001311, juce::Colours::purple);
    slider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
    //slider.setRange (minLim, maxLim, step);
    //slider.setValue (defaultValue);
    label.setText(Text, dontSendNotification);
    label.setFont(15.0f);
    label.setJustificationType(juce::Justification::centred);
    label.attachToComponent(&slider , false);
}

//void Wind_FDTD_pluginAudioProcessorEditor::setSliderAndLabelHorizontal(juce::Slider& slider, juce::Label& label, double minLim, double maxLim, double step, double defaultValue, const String &Text)
void WindFDTDpluginAudioProcessorEditor::setSliderAndLabelHorizontal(juce::Slider& slider, juce::Label& label, const String &Text)
{
    slider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    slider.setColour(0x1001310, juce::Colours::purple);
    slider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
    //slider.setRange (minLim, maxLim, step);
    //slider.setValue (defaultValue);
    label.setText(Text, dontSendNotification);
    label.setFont(15.0f);
    label.setJustificationType(juce::Justification::centred);
    label.attachToComponent(&slider , false);
}

//void Wind_FDTD_pluginAudioProcessorEditor::setSliderAndLabelVertical(juce::Slider& slider, juce::Label& label, double minLim, double maxLim, double step, double defaultValue, const String &Text)
void WindFDTDpluginAudioProcessorEditor::setSliderAndLabelVertical(juce::Slider& slider, juce::Label& label, const String &Text)
{
    slider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    slider.setColour(0x1001310, juce::Colours::purple);
    slider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
    //slider.setRange (minLim, maxLim, step);
    //slider.setValue (defaultValue);
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
    
    //g.setColour (juce::Colours::white);
    //g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void WindFDTDpluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto boreArea = getLocalBounds();
    auto reedArea = boreArea.removeFromTop(boreArea.getHeight()*0.66);
    auto pressureArea = reedArea.removeFromLeft(reedArea.getWidth()*0.66);
    auto boreLengthArea = boreArea.removeFromLeft(boreArea.getWidth()*0.33);
    boreLengthArea.removeFromLeft(boreLengthArea.getWidth()*0.16);
    cylinderLengthSlider.setBounds(boreLengthArea.removeFromTop(boreLengthArea.getHeight()*0.25));
    boreLengthArea.removeFromTop(boreLengthArea.getHeight()*0.33);
    bellLengthSlider.setBounds(boreLengthArea.removeFromTop(boreLengthArea.getHeight()*0.5));
    boreArea.removeFromBottom(boreArea.getHeight()*0.25);
    cylinderRadiusSlider.setBounds(boreArea.removeFromLeft(boreArea.getWidth()*0.33));
    bellEndRadiusSlider.setBounds(boreArea.removeFromLeft(boreArea.getWidth()*0.33));
    boreArea.removeFromRight(boreArea.getWidth()*0.2);
    bellGrowthMenu.setBounds(boreArea);
    reedArea.removeFromTop(reedArea.getHeight()*0.15);
    reedArea.removeFromBottom(reedArea.getHeight()*0.33);
    auto vibArea = reedArea.removeFromBottom(reedArea.getHeight()*0.5);
    vibArea = vibArea.removeFromBottom(vibArea.getHeight()*0.6);
    reedWidthSlider.setBounds(reedArea.removeFromRight(reedArea.getWidth()*0.5));
    reedMassSlider.setBounds(reedArea);
    pressureArea.removeFromTop(pressureArea.getHeight()*0.15);
    auto pressureMultArea = pressureArea.removeFromBottom(pressureArea.getHeight()*0.33);
    pressureSlider.setBounds(pressureArea.removeFromLeft(pressureArea.getWidth()*0.33));
    attackSlider.setBounds(pressureArea.removeFromLeft(pressureArea.getWidth()*0.25));
    decaySlider.setBounds(pressureArea.removeFromLeft(pressureArea.getWidth()*0.33));
    sustainSlider.setBounds(pressureArea.removeFromLeft(pressureArea.getWidth()*0.5));
    releaseSlider.setBounds(pressureArea);
    pressureMultArea.removeFromBottom(pressureMultArea.getHeight()*0.4);
    pressureMultArea.removeFromRight(pressureMultArea.getWidth()*0.74);
    pressureMultArea.removeFromLeft(pressureMultArea.getWidth()*0.25);
    pressureMultArea.removeFromTop(pressureMultArea.getHeight()*0.25);
    pressureMultiply.setBounds(pressureMultArea);
    vibRateSlider.setBounds(vibArea.removeFromLeft(vibArea.getWidth()*0.5));
    vibAmountSlider.setBounds(vibArea);
    //bellEndRadiusSlider.setBounds(boreArea.removeFromLeft(boreArea.getWidth()*0.5));
}
    
//void Wind_FDTD_pluginAudioProcessorEditor::sliderValueChanged(Slider *slider)
//{
 //   if (slider == &cylinderLengthSlider)
 //   {
 //       audioProcessor.setCylinderLength(cylinderLengthSlider.getValue());
 //   }
//}

//bool WindFDTDpluginAudioProcessorEditor::keyPressed(const KeyPress &key, Component *originatingComponent)
//{
//    DBG("KeyPressed");
//    audioProcessor.getWindVoice(0)->startNote(60, 100, nullptr, 0);
//}
