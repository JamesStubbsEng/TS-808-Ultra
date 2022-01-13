/*
  ==============================================================================

    DryLPF.h
    Created: 13 Jan 2022 11:39:54am
    Author:  James

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class DryLPF
{
public:
    DryLPF();
    void setCutoff(float val);
    void prepare(const juce::dsp::ProcessSpec& spec) noexcept;
    void reset();
    template <typename ProcessContext>
    void processBlock(const ProcessContext& context) noexcept;

private:
    float sampleRate = 44100.0f;

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> cutoffSmoothed;
    dsp::StateVariableTPTFilter<float> lpFilter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DryLPF)
};

template<typename ProcessContext>
inline void DryLPF::processBlock(const ProcessContext& context) noexcept
{
    const auto& inputBlock = context.getInputBlock();
    auto& outputBlock = context.getOutputBlock();
    const auto numChannels = outputBlock.getNumChannels();
    const auto numSamples = outputBlock.getNumSamples();

    if (context.isBypassed)
    {
        outputBlock.copyFrom(inputBlock);
        return;
    }

    for (size_t i = 0; i < numSamples; ++i)
    {
        lpFilter.setCutoffFrequency(cutoffSmoothed.getNextValue());
        for (size_t channel = 0; channel < numChannels; ++channel)
        {
            auto* inputSamples = inputBlock.getChannelPointer(channel);
            auto* outputSamples = outputBlock.getChannelPointer(channel);
            outputSamples[i] = lpFilter.processSample((int)channel, inputSamples[i]);
        }
    }  
}
