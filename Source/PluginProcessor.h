/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "WindFDTDVoice.h"

//==============================================================================
/**
*/
class WindFDTDpluginAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    WindFDTDpluginAudioProcessor();
    ~WindFDTDpluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    float limit (float val, float min, float max);
    

    int bellGrowthMenuId = 1;
    int pressureMultMenuId = 1;

    
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    juce::AudioProcessorValueTreeState tree{*this, nullptr, "Patams", createParameterLayout()};
    
    juce::SynthesiserVoice* getWindVoice (int i) {
        return windSynth.getVoice (i);
    }
private:
    
    double fs; // Sample rate which we can retrieve from the prepareToPlay function
    
    juce::Synthesiser windSynth;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WindFDTDpluginAudioProcessor)
};
