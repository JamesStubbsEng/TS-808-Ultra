/*
  ==============================================================================

    ToneStage.h
    Created: 7 Jan 2022 4:22:28pm
    Author:  James Stubbs

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class ToneStage : public chowdsp::IIRFilter<2>
{
public:
    ToneStage();
    void setTone(float tone);
    void prepare(float sampleRate);
    void calcCoefs(float curTreble);
    void processBlock(float* block, const int numSamples) noexcept override;

private:
    float taperPotSim(float in);
    float fs = 44100.0f;

    const float rPot = 20000.0f;

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> rLSmoothed;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToneStage)
};