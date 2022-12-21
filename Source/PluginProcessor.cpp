/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

struct ChainSettings
{
    int  excF { 0 }, xPosMod { 0 }, yPosMod { 0 }, numStrings { 0 }, sTenDiff { 0 }, sTen { 0 }, cylinderRadius { 0 },  bellRadius { 0 };
    float sig0 { 0 }, sig1 { 0 }, lengthX { 0 }, lengthY { 0 }, excX { 0 }, excY { 0 }, lisX { 0 }, lisY { 0 }, thickness { 0 }, excT { 0 }, vB { 0 }, FB { 0 }, a { 0 }, bAtt1 { 0 }, bDec1 { 0 }, bSus1 { 0 }, bRel1 { 0 }, FBEnv1 { 0 }, vBEnv1 { 0 }, lfoRate { 0 }, sLen { 0 }, sRad { 0 }, sPosSpread { 0 }, sSig0 { 0 }, cylinderLength { 0 }, bellLength { 0 };
};

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);

//==============================================================================
PlateAudioProcessor::PlateAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

PlateAudioProcessor::~PlateAudioProcessor()
{
}

//==============================================================================
const juce::String PlateAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PlateAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PlateAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PlateAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PlateAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PlateAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PlateAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PlateAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PlateAudioProcessor::getProgramName (int index)
{
    return {};
}

void PlateAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PlateAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    hit = false;
    firstHit = false;
    // Retrieve sample rate
    fs = sampleRate;
    thinPlate = std::make_unique<ThinPlate> (1.0 / fs);
    thinPlate-> getSampleRate(fs);
    thinPlate-> initParameters();
}

void PlateAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PlateAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void PlateAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    
    //Allow midi notes to activate a plate hit
    juce::MidiBuffer::Iterator mIt(midiMessages);
    juce::MidiMessage curMes;
    int samplePosition;
    while (mIt.getNextEvent(curMes, samplePosition))
    {
        if (curMes.isNoteOn())
        {
            hit = true;
            bowStart = true;
        }
        if (curMes.isNoteOff())
        {
            bowEnd = true;
        }
    }
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    auto* channelDataL = buffer.getWritePointer (0);
    auto* channelDataR = buffer.getWritePointer (1);
    
    auto chainSettings = getChainSettings(tree);
    
    thinPlate -> updateParameters(chainSettings.sig0, chainSettings.sig1, chainSettings.lengthX, chainSettings.lengthY, chainSettings.excX, chainSettings.excY, chainSettings.lisX, chainSettings.lisY, chainSettings.thickness, chainSettings.excF, chainSettings.excT, chainSettings.vB, chainSettings.FB, chainSettings.a, excTypeId, chainSettings.bAtt1, chainSettings.bDec1 , chainSettings.bSus1, chainSettings.bRel1, chainSettings.FBEnv1, chainSettings.vBEnv1, chainSettings.lfoRate, chainSettings.xPosMod, chainSettings.yPosMod, chainSettings.numStrings, chainSettings.sLen, chainSettings.sPosSpread, chainSettings.sTen, chainSettings.sTenDiff, chainSettings.sRad, chainSettings.sSig0, chainSettings.cylinderLength, chainSettings.cylinderRadius, chainSettings.bellLength, chainSettings.bellRadius, bellGrowthMenuId, tubeConn, springConn);
    thinPlate -> updatePlateMaterial(plateMaterialId);
    thinPlate -> getSampleRate(fs);

    if (hit == true)
    {
        firstHit = true;
        thinPlate-> initParameters();
        thinPlate -> plateHit();
        hit = false;
    }
    
    if (bowStart == true)
    {
        firstBow = true;
        thinPlate-> initParameters();
        thinPlate -> startBow();
        bowStart = false;
    }
    
    if (bowEnd == true)
    {
        thinPlate -> endBow();
        bowEnd = false;
    }
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        if (firstHit == true || firstBow == true)
        {
            thinPlate->calculateScheme();
            output = thinPlate->getOutput();
            channelDataL[i] = limit(output, -1, 1);
            channelDataR[i] = channelDataL[i];
        }
    }
}


//==============================================================================
bool PlateAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PlateAudioProcessor::createEditor()
{
    return new PlateAudioProcessorEditor (*this);
}

//==============================================================================
void PlateAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream mos(destData,true);
    tree.state.writeToStream(mos);
}

void PlateAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
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
    
    settings.sig0 = tree.getRawParameterValue("Frequency Independent Damping")->load();
    settings.sig1 = tree.getRawParameterValue("Frequency Dependent Damping")->load();
    settings.lengthX = tree.getRawParameterValue("Plate length X")->load();
    settings.lengthY = tree.getRawParameterValue("Plate length Y")->load();
    settings.excX = tree.getRawParameterValue("Excitation pos X")->load();
    settings.excY = tree.getRawParameterValue("Excitation pos Y")->load();
    settings.lisX = tree.getRawParameterValue("Listening pos X")->load();
    settings.lisY = tree.getRawParameterValue("Listening pos Y")->load();
    settings.thickness = tree.getRawParameterValue("Plate thickness")->load();
    settings.excF = tree.getRawParameterValue("Excitation force")->load();
    settings.excT = tree.getRawParameterValue("Excitation time")->load();
    settings.vB = tree.getRawParameterValue("Bow velocity")->load();
    settings.FB = tree.getRawParameterValue("Bow force")->load();
    settings.a = tree.getRawParameterValue("Friction")-> load();
    settings.bAtt1 = tree.getRawParameterValue("Bow attack 1")-> load();
    settings.bDec1 = tree.getRawParameterValue("Bow decay 1")-> load();
    settings.bSus1 = tree.getRawParameterValue("Bow sustain 1")-> load();
    settings.bRel1 = tree.getRawParameterValue("Bow release 1")-> load();
    settings.FBEnv1 = tree.getRawParameterValue("Bow force env 1")-> load();
    settings.vBEnv1 = tree.getRawParameterValue("Bow velocity env 1")-> load();
    settings.lfoRate = tree.getRawParameterValue("LFO Rate")-> load();
    settings.xPosMod = tree.getRawParameterValue("X Pos Mod Depth")-> load();
    settings.yPosMod = tree.getRawParameterValue("Y Pos Mod Depth")-> load();
    settings.numStrings = tree.getRawParameterValue("Number of Strings")-> load();
    settings.sLen = tree.getRawParameterValue("String Length")-> load();
    settings.sRad = tree.getRawParameterValue("String Radius")-> load();
    settings.sTen = tree.getRawParameterValue("String Tension")-> load();
    settings.sTenDiff = tree.getRawParameterValue("String Tension Difference")-> load();
    settings.sPosSpread = tree.getRawParameterValue("String Position Spread")-> load();
    settings.sSig0 = tree.getRawParameterValue("String Damping") -> load();
    settings.cylinderLength = tree.getRawParameterValue("Cylinder Length")->load();
    settings.cylinderRadius = tree.getRawParameterValue("Cylinder Radius")->load();
    settings.bellLength = tree.getRawParameterValue("Bell Length")->load();
    settings.bellRadius = tree.getRawParameterValue("Bell Radius")->load();
    return settings;
}

juce::AudioProcessorValueTreeState::ParameterLayout PlateAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("Frequency Independent Damping", "Frequency Independent Damping", 0.01f, 10.f, 1.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Frequency Dependent Damping", "Frequency Dependent Damping", juce::NormalisableRange<float>(0.0001f, 0.1f, 0.00001f, 0.35f), 0.0005f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Plate length X", "Plate length X", 0.2f, 1.f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Plate length Y", "Plate length Y", 0.2f, 1.f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Excitation pos X", "Excitation pos X", 0.1f, 0.8f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Excitation pos Y", "Excitation pos Y", 0.1f, 0.8f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Listening pos X", "Listening pos X", 0.1f, 0.9f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Listening pos Y", "Listening pos Y", 0.1f, 0.9f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Plate thickness", "Plate thickness", 4.f, 20.f, 8.f));
    layout.add(std::make_unique<juce::AudioParameterInt>("Excitation force", "Excitation force", 1, 100, 10));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Excitation time", "Excitation time", 0.1f, 5.f, 1.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Bow velocity", "Bow velocity", 0.05, 0.3f, 0.1f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Bow force", "Bow force", 0.05, 0.3f, 0.1f));
    layout.add(std::make_unique<juce::AudioParameterInt>("Friction", "Friction", 1, 100, 1));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Bow attack 1", "Bow attack 1", 0.01f, 5.f, 0.01f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Bow decay 1", "Bow decay 1", 0.01f, 5.f, 0.01f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Bow sustain 1", "Bow sustain 1", 0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Bow release 1", "Bow release 1", 0.01f, 5.f, 0.01f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Bow force env 1", "Bow force env 1", -1.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Bow velocity env 1", "Bow velocity env 1", -1.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LFO Rate", "LFO Rate", 0.01, 10, 0.1));
    layout.add(std::make_unique<juce::AudioParameterInt>("X Pos Mod Depth", "X Pos Mod Depth", 0, 100, 0));
    layout.add(std::make_unique<juce::AudioParameterInt>("Y Pos Mod Depth", "Y Pos Mod Depth", 0, 100, 0));
    layout.add(std::make_unique<juce::AudioParameterInt>("Number of Strings", "Number of Strings", 0, 8, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>("String Length", "String Length" , 0.1f, 0.9f, 0.2f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("String Radius", "String Radius", 0.01f, 2.f, 1.f));
    layout.add(std::make_unique<juce::AudioParameterInt>("String Tension", "String Tension" , 500, 2000, 1000));
    layout.add(std::make_unique<juce::AudioParameterInt>("String Tension Difference", "String Tension Difference" , 0, 100, 25));
    layout.add(std::make_unique<juce::AudioParameterInt>("String Position Spread", "String Position Spread", 10, 90, 50));
    layout.add(std::make_unique<juce::AudioParameterFloat>("String Damping", "String Damping", 0.01f, 5.f, 0.2f));
    layout.add(std::make_unique<juce::AudioParameterInt>("Cylinder Radius", "Cylinder Radius", 1, 20, 2));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Cylinder Length", "Cylinder Length", 0.1f, 4, 1.77f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Bell Length", "Bell Length", 0.f, 1.f, 0.8f));
    layout.add(std::make_unique<juce::AudioParameterInt>("Bell Radius", "Bell Radius", 1, 100, 10));

    return layout;
}

float PlateAudioProcessor::limit (float val, float min, float max)
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
    return new PlateAudioProcessor();
}
