/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TS808UltraAudioProcessor::TS808UltraAudioProcessor()
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

TS808UltraAudioProcessor::~TS808UltraAudioProcessor()
{
}

//==============================================================================
const juce::String TS808UltraAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TS808UltraAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TS808UltraAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TS808UltraAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TS808UltraAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TS808UltraAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TS808UltraAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TS808UltraAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TS808UltraAudioProcessor::getProgramName (int index)
{
    return {};
}

void TS808UltraAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void TS808UltraAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    oversampling.initProcessing (samplesPerBlock);

    for (int ch = 0; ch < 2; ++ch)
        diodeClipper[ch].prepare (sampleRate);
}

void TS808UltraAudioProcessor::releaseResources()
{
    oversampling.reset();

    for (int ch = 0; ch < 2; ++ch)
        diodeClipper[ch].reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TS808UltraAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void TS808UltraAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    buffer.applyGain(Decibels::decibelsToGain (5.0f));
    
    dsp::AudioBlock<float> block (buffer);
    auto osBlock = oversampling.processSamplesUp (block);

    float* ptrArray[] = {osBlock.getChannelPointer (0), osBlock.getChannelPointer (1)};
    AudioBuffer<float> osBuffer (ptrArray, 2, static_cast<int> (osBlock.getNumSamples()));

    for (int ch = 0; ch < osBuffer.getNumChannels(); ++ch)
    {
        diodeClipper[ch].setCircuitParams (1000.0f);
        auto* x = osBuffer.getWritePointer (ch);

        for (int n = 0; n < osBuffer.getNumSamples(); ++n)
            x[n] = diodeClipper[ch].processSample (x[n]);
    }
    
    oversampling.processSamplesDown (block);
    
    buffer.applyGain(Decibels::decibelsToGain (-5.0f));
}

//==============================================================================
bool TS808UltraAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TS808UltraAudioProcessor::createEditor()
{
    return new TS808UltraAudioProcessorEditor (*this);
}

//==============================================================================
void TS808UltraAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void TS808UltraAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TS808UltraAudioProcessor();
}
