#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    ///__________________________
    
    // Retrieve sample rate
    fs = sampleRate;
    
    // Initialise the instance of the OneDWave class
    windFDTD = std::make_unique<WindFDTD> (1.0 / fs);
    
    // Add it to the application and make it visible (a must-do in JUCE)
    addAndMakeVisible(windFDTD.get());
    
    // Call the resized function now that the instance of the OneDWave class is initialised
    resized();
    
    // Start the graphics timer (refresh 15 times per second)
    startTimerHz (15);
    
    ///__________________________
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    ///__________________________

    // Prevent noise in the buffer.
    bufferToFill.clearActiveBufferRegion();

    // Get the number of channels.
    int numChannels = bufferToFill.buffer->getNumChannels();
    
    // Get pointers to output locations.
    float* const channelData1 = bufferToFill.buffer->getWritePointer (0, bufferToFill.startSample);
    float* const channelData2 = numChannels > 1 ? bufferToFill.buffer->getWritePointer (1, bufferToFill.startSample) : nullptr;

    float output = 0.0;

    std::vector<float* const*> curChannel {&channelData1, &channelData2};
        
    // For all samples in the buffer..
    for (int i = 0; i < bufferToFill.numSamples; ++i)
    {
        // Calculate the scheme once every sample
        windFDTD->calculateScheme();
        
        // Calculate the states once every sample
        windFDTD->updateStates();
        
        // Retrieve the output at 0.8 the system length
        output = windFDTD->getOutput (0.8);
        
        // Send the output to both channels.
        for (int channel = 0; channel < numChannels; ++channel)
            curChannel[channel][0][i] = limit(output, -1.0, 1.0);
    }
    
    ///__________________________
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    //__________________________
    
    // Check if the instance of the OneDWave class is initialised
    if (windFDTD != nullptr)
    {
        // Use the entire application window to visualise the state of the system
        windFDTD->setBounds (getLocalBounds());
    }
    
    ///__________________________

}

///__________________________

void MainComponent::timerCallback()
{
    // Repaint the application. This function gets called 15x per second (as defined in prepareToPlay)
    repaint();
}

// Implementation of the limiter
float MainComponent::limit (float val, float min, float max)
{
    if (val < min)
        return min;
    else if (val > max)
        return max;
    else
        return val;
}

///__________________________
