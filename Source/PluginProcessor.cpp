/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

struct ChainSettings
{
    int  cylinderRadius { 0 },  bellRadius { 0 }, reedWidth { 0 }, attack { 0 }, decay { 0 }, sustain { 0 }, release { 0 }; float boreScale { 0 }, cylinderBellRatio { 0 }, reedMass { 0 }, pressure { 0 }, vibratoRate { 0 }, vibratoAmount { 0 };
};

ChainSettings getChainSettings(AudioProcessorValueTreeState& apvts);


//==============================================================================
WindFDTDpluginAudioProcessor::WindFDTDpluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                #if ! JucePlugin_IsMidiEffect
                 #if ! JucePlugin_IsSynth
                 #endif
                    .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                #endif
                    )
#endif
{

}

WindFDTDpluginAudioProcessor::~WindFDTDpluginAudioProcessor()
{
}

const juce::String WindFDTDpluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool WindFDTDpluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool WindFDTDpluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool WindFDTDpluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double WindFDTDpluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int WindFDTDpluginAudioProcessor::getNumPrograms()
{
    return 1;
}

int WindFDTDpluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void WindFDTDpluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String WindFDTDpluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void WindFDTDpluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void WindFDTDpluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
 
    // Retrieve sample rate
    fs = sampleRate;
    windSynth.setCurrentPlaybackSampleRate(fs);
    //Add voices (6 voices of polyphony)
    windSynth.clearVoices();
    for (int i = 0; i < 6; i++)
    {
        windSynth.addVoice(new WindFDTDVoice(1/fs));
    }
    windSynth.clearSounds();
    windSynth.addSound(new WindFDTDSound());
    
}

void WindFDTDpluginAudioProcessor::releaseResources()
{

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool WindFDTDpluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused (layouts);
  return true;
#else

  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
   && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
      return false;

 #if ! JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
      return false;
 #endif

  return true;
#endif
}
#endif

void WindFDTDpluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    
    auto chainSettings = getChainSettings(tree);
    for (int i = 0; i < windSynth.getNumVoices(); ++i)
    {
        if (auto voice = dynamic_cast<WindFDTDVoice*>(windSynth.getVoice(i)))
        {
            voice -> setADSR(fs);
            voice -> updateParameters(chainSettings.boreScale, chainSettings.cylinderRadius, chainSettings.cylinderBellRatio, chainSettings.bellRadius, bellGrowthMenuId, chainSettings.reedMass, chainSettings.reedWidth, chainSettings.pressure, chainSettings.attack, chainSettings.decay, chainSettings.sustain, chainSettings.release, pressureMultMenuId, chainSettings.vibratoRate, chainSettings.vibratoAmount);
        }
    }
    
    windSynth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    
}

//==============================================================================
bool WindFDTDpluginAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* WindFDTDpluginAudioProcessor::createEditor()
{
    return new WindFDTDpluginAudioProcessorEditor (*this);
}

//==============================================================================
void WindFDTDpluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    
    juce::MemoryOutputStream mos(destData,true);
    tree.state.writeToStream(mos);
}

void WindFDTDpluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto valueTree = juce::ValueTree::readFromData(data, sizeInBytes);
    if( valueTree.isValid() )
    {
        tree.replaceState(valueTree);
    }
}

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& tree)
{
    ChainSettings settings;
    
    settings.boreScale = tree.getRawParameterValue("Bore Scale")->load();
    settings.cylinderRadius = tree.getRawParameterValue("Cylinder Radius")->load();
    settings.cylinderBellRatio = tree.getRawParameterValue("Cylinder Bell Ratio")->load();
    settings.bellRadius = tree.getRawParameterValue("Bell Radius")->load();
    settings.reedMass = tree.getRawParameterValue("Reed Mass")->load();
    settings.reedWidth = tree.getRawParameterValue("Reed Width")->load();
    settings.pressure = tree.getRawParameterValue("Pressure")->load();
    settings.attack = tree.getRawParameterValue("Attack")->load();
    settings.decay = tree.getRawParameterValue("Decay")->load();
    settings.sustain = tree.getRawParameterValue("Sustain")->load();
    settings.release = tree.getRawParameterValue("Release")->load();
    settings.vibratoRate  = tree.getRawParameterValue("Vibrato Rate")->load();
    settings.vibratoAmount  = tree.getRawParameterValue("Vibrato Amount")->load();
    return settings;
}

juce::AudioProcessorValueTreeState::ParameterLayout WindFDTDpluginAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add(std::make_unique<juce::AudioParameterInt>("Cylinder Radius", "Cylinder Radius", 1, 20, 2));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Bore Scale", "Bore Scale", 0.1f, 4, 1.77f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Cylinder Bell Ratio", "Cylinder Bell Ratio", 0.f, 1.f, 0.8f));
    layout.add(std::make_unique<juce::AudioParameterInt>("Bell Radius", "Bell Radius", 1, 100, 10));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Reed Mass", "Reed Mass", 0.01f, 10.f, 0.537f));
    layout.add(std::make_unique<juce::AudioParameterInt>("Reed Width", "Reed Width", 1, 100, 15));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Pressure", "Pressure", 0.f, 1000.f, 0.f));
    layout.add(std::make_unique<juce::AudioParameterInt>("Attack", "Attack", 0, 1000, 500));
    layout.add(std::make_unique<juce::AudioParameterInt>("Decay", "Decay", 0, 1000, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Sustain", "Sustain", 0, 1, 1));
    layout.add(std::make_unique<juce::AudioParameterInt>("Release", "Release", 0, 5000, 500));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Vibrato Rate", "Vibrato Rate", NormalisableRange<float>(0.1f, 1000.f, 0.01f, 0.25f), 0.1f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Vibrato Amount", "Vibrato Amount", 0.f, 12.f, 0.f));
    return layout;
}

// Implementation of the limiter
float WindFDTDpluginAudioProcessor::limit (float val, float min, float max)
{
    if (val < min)
        return min;
    else if (val > max)
        return max;
    else
        return val;
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WindFDTDpluginAudioProcessor();
}
