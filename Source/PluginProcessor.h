/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "dsp/ClippingStage.h"
#include "dsp/ToneStage.h"

//==============================================================================
/**
*/
class TS808UltraAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    TS808UltraAudioProcessor();
    ~TS808UltraAudioProcessor() override;

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

private:
    //==============================================================================
    AudioProcessorValueTreeState parameters;

    std::atomic<float>* driveParameter = nullptr;
    std::atomic<float>* toneParameter = nullptr;
    std::atomic<float>* mixParameter = nullptr;
    std::atomic<float>* filterParameter = nullptr;
    std::atomic<float>* drySquashParameter = nullptr;
    std::atomic<float>* gainParameter = nullptr;

    ClippingStage clippingStage[2];
    ToneStage toneStage[2];

    size_t oversampleFactor = 1;
    dsp::Oversampling<float> oversampling { 2, oversampleFactor, dsp::Oversampling<float>::filterHalfBandPolyphaseIIR };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TS808UltraAudioProcessor)
};
