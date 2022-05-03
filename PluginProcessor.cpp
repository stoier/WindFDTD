/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

struct ChainSettings
{
    int cylinderLength { 0 }, cylinderRadius { 0 }, bellLength { 0 }, bellRadius { 0 }, reedWidth { 0 }; float reedMass { 0 };
};

ChainSettings getChainSettings(AudioProcessorValueTreeState& apvts);

//==============================================================================
WindFDTDpluginAudioProcessor::WindFDTDpluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                      //#if ! JucePlugin_IsMidiEffect
                       //#if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo())
                      //.withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       
                      //#endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo())
                       .withInput  ("Sidechain",  juce::AudioChannelSet::stereo())
                     //#endif
                       )
#endif
{

}

WindFDTDpluginAudioProcessor::~WindFDTDpluginAudioProcessor()
{
}

//==============================================================================
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
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
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
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    // Retrieve sample rate
    fs = sampleRate;
    windSynth.setCurrentPlaybackSampleRate(fs);
    //Add voices (2 voices of polyphony)
    windSynth.clearVoices();
    for (int i = 0; i < 4; i++)
    {
        windSynth.addVoice(new WindFDTDVoice(1/fs));
    }
    windSynth.clearSounds();
    windSynth.addSound(new WindFDTDSound());
    
    //getWindVoice(0)->startNote(60, 100, nullptr, 0);
    // Initialise the instance of the OneDWave class
    //windVoice = std::make_unique<WindVoice> (1.0 / fs);
}

void WindFDTDpluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool WindFDTDpluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  //#if JucePlugin_IsMidiEffect
  //  juce::ignoreUnused (layouts);
  //  return true;
  //#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    //if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
    // && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
     //   return false;
    return layouts.getMainInputChannelSet() == layouts.getMainOutputChannelSet()
                     && ! layouts.getMainInputChannelSet().isDisabled();
    // This checks if the input layout matches the output layout
   //#if ! JucePlugin_IsSynth
   // if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
   //     return false;
   //#endif

    //return true;
  //#endif
}
#endif

void WindFDTDpluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto mainInputOutput = getBusBuffer (buffer, true, 0);
    auto sideChainInput  = getBusBuffer (buffer, true, 1);

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        mainInputOutput.clear (i, 0, mainInputOutput.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    
    
    auto chainSettings = getChainSettings(tree);
    for (int i = 0; i < windSynth.getNumVoices(); ++i)
    {
        if (auto voice = dynamic_cast<WindFDTDVoice*>(windSynth.getVoice(i)))
        {
            voice -> updateParameters(chainSettings.cylinderLength, chainSettings.cylinderRadius, chainSettings.bellLength, chainSettings.bellRadius, bellGrowthMenuId, chainSettings.reedMass, chainSettings.reedWidth);
        }
    }
    
    
//    for (int channel = 0; channel < totalNumInputChannels; ++channel)
//    {
//        auto input = buffer.getReadPointer(channel);
//        // Loop over all the samples in our buffer
//        for (int i = 0; i < buffer.getNumSamples(); ++i)
//        {
//            for (int j = 0; j < windSynth.getNumVoices(); ++j)
//            {
//                if (auto voice = dynamic_cast<WindFDTDVoice*>(windSynth.getVoice(j)))
//                {
//                    voice -> getAudioInput(input[i]);
//                }
//            }
//        //auto* channelData = buffer.getWritePointer (channel);
//
//        // ..do something to the data...
//        //Read MIDI buffer into the synthesizer and render all the synth voices
//        windSynth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
//        }
//    }
    
    //auto inputLevel = sideChainInput.getRMSLevel(0, 0, sideChainInput.getNumSamples());
    for (int i = 0; i < windSynth.getNumVoices(); ++i)
    {
        if (auto voice = dynamic_cast<WindFDTDVoice*>(windSynth.getVoice(i)))
        {
      //      voice -> getAudioInput(inputLevel);
        }
    }
    windSynth.renderNextBlock(mainInputOutput, midiMessages, 0, mainInputOutput.getNumSamples());
}

//==============================================================================
bool WindFDTDpluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* WindFDTDpluginAudioProcessor::createEditor()
{
    return new WindFDTDpluginAudioProcessorEditor (*this);
}

//==============================================================================
void WindFDTDpluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    //juce::MemoryOutputStream mos(destData,true);
    //tree.state.writeToStream(mos);
}

void WindFDTDpluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    //auto valueTree = juce::ValueTree::readFromData(data, sizeInBytes);
    //if( valueTree.isValid() )
    //{
    //    tree.replaceState(valueTree);
    //}
}

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& tree)
{
    ChainSettings settings;
    
    settings.cylinderLength = tree.getRawParameterValue("Cylinder Length")->load();
    settings.cylinderRadius = tree.getRawParameterValue("Cylinder Radius")->load();
    settings.bellLength = tree.getRawParameterValue("Bell Length")->load();
    settings.bellRadius = tree.getRawParameterValue("Bell Radius")->load();
    settings.reedMass = tree.getRawParameterValue("Reed Mass")->load();
    settings.reedWidth = tree.getRawParameterValue("Reed Width")->load();
    return settings;
}

juce::AudioProcessorValueTreeState::ParameterLayout WindFDTDpluginAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add(std::make_unique<juce::AudioParameterInt>("Cylinder Length", "Cylinder Length", 1, 1000, 100));
    layout.add(std::make_unique<juce::AudioParameterInt>("Cylinder Radius", "Cylinder Radius", 1, 100, 2));
    layout.add(std::make_unique<juce::AudioParameterInt>("Bell Length", "Bell Length", 1, 500, 20));
    layout.add(std::make_unique<juce::AudioParameterInt>("Bell Radius", "Bell Radius", 1, 100, 10));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Reed Mass", "Reed Mass", 0.01, 10, 0.537));
    layout.add(std::make_unique<juce::AudioParameterInt>("Reed Width", "Reed Width", 1, 100, 5));
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

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WindFDTDpluginAudioProcessor();
}
