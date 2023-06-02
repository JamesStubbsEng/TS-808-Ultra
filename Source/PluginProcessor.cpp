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
                       ), parameters(*this, nullptr, Identifier("sampld"),
                           {
                               std::make_unique<AudioParameterFloat>("inputGain", "Input Gain", NormalisableRange<float>(-80.0f, 20.0f,0.1f, 2.0f), 0.0f),
                               std::make_unique<AudioParameterFloat>("drive", "Drive", NormalisableRange<float>(0.0f, 10.0f,0.1f), 2.0f),
                               std::make_unique<AudioParameterFloat>("tone", "Tone", NormalisableRange<float>(0.0f, 10.0f,0.1f), 5.0f),
                               std::make_unique<AudioParameterFloat>("mix", "Mix", NormalisableRange<float>(0.0f, 10.0f,0.1f), 10.0f),
                               std::make_unique<AudioParameterFloat>("filter", "Direct Filter", NormalisableRange<float>(0.0f, 10.0f,0.1f), 0.0f),
                               std::make_unique<AudioParameterFloat>("drySquash", "Direct Squash", NormalisableRange<float>(0.0f, 10.0f,0.1f), 0.0f),
                               std::make_unique<AudioParameterFloat>("outputGain", "Output Gain", NormalisableRange<float>(-30.0f, 30.0f,0.1f), 0.0f),
                              
                           })
#endif
{
    inputGainParameter = parameters.getRawParameterValue("inputGain");
    driveParameter = parameters.getRawParameterValue("drive");
    toneParameter = parameters.getRawParameterValue("tone");
    mixParameter = parameters.getRawParameterValue("mix");
    filterParameter = parameters.getRawParameterValue("filter");
    drySquashParameter = parameters.getRawParameterValue("drySquash");
    outputGainParameter = parameters.getRawParameterValue("outputGain");
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
    dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    dryWetMixer.prepare(spec);
    dryWetMixer.setMixingRule(juce::dsp::DryWetMixingRule::sin3dB);

    dryComp.prepare(sampleRate);
    dryLPF.prepare(spec);

    inputGain.prepare(spec);
    outputGain.prepare(spec);

    oversampling.initProcessing (samplesPerBlock);
    parallelBuffer.setSize(spec.numChannels, spec.maximumBlockSize);

    for (int ch = 0; ch < 2; ++ch)
    {
        clippingStage[ch].prepare((float)sampleRate * std::powf(2.0f, float(oversampleFactor)));
        clippingStage[ch].setDrive(*driveParameter);
        toneStage[ch].prepare((float)sampleRate);
        toneStage[ch].setTone(*toneParameter);
    }    
}

void TS808UltraAudioProcessor::releaseResources()
{
    dryWetMixer.reset();
    dryLPF.reset();
    inputGain.reset();
    outputGain.reset();
    oversampling.reset();

    for (int ch = 0; ch < 2; ++ch)
    {
        clippingStage[ch].reset();
        toneStage[ch].reset();
    }
        
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
    const auto numSamples = buffer.getNumSamples();

    dsp::AudioBlock<float> block(buffer);
    auto context = juce::dsp::ProcessContextReplacing<float>(block);  

    // input gain stage
    inputGain.setGainDecibels(*inputGainParameter);
    inputGain.process(context);

    // copy original dry signal into a buffer
    parallelBuffer.makeCopyOf(buffer, true);
    dsp::AudioBlock<float> parallelBlock(parallelBuffer);
    auto parallelContext = juce::dsp::ProcessContextReplacing<float>(parallelBlock);

    //------start of di parallel processing---------
    dryComp.setThreshold(*drySquashParameter);
    dryComp.processBlock(parallelBuffer);

    dryLPF.setCutoff(*filterParameter);
    dryLPF.processBlock(parallelContext);

    dryWetMixer.setWetMixProportion(jmap((float)*mixParameter, 0.0f, 10.0f, 0.0f, 1.0f));
    dryWetMixer.pushDrySamples(parallelBlock);
    //------end of di parallel processing-----------

    
    //--------start of TS808 processing------------
   
    auto osBlock = oversampling.processSamplesUp (block);

    for (int ch = 0; ch < osBlock.getNumChannels(); ++ch)
    {
        clippingStage[ch].setDrive (*driveParameter);
        auto* x = osBlock.getChannelPointer(ch);

        for (int n = 0; n < osBlock.getNumSamples(); ++n)
            x[n] = clippingStage[ch].processSample (x[n]);
    }
    
    oversampling.processSamplesDown (block);

    // tone stage
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* x = buffer.getWritePointer(ch);

        toneStage[ch].setTone(*toneParameter);
        toneStage[ch].processBlock(x, numSamples);
    }

    //--------end of TS808 processing-----------

    // mix signals
    dryWetMixer.mixWetSamples(block);
    
    // Output Gain stage
    outputGain.setGainDecibels(*outputGainParameter);
    outputGain.process(context);
}

//==============================================================================
bool TS808UltraAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TS808UltraAudioProcessor::createEditor()
{
    return new TS808UltraAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void TS808UltraAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void TS808UltraAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TS808UltraAudioProcessor();
}
