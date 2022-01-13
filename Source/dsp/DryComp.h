/*
  ==============================================================================

    DryComp.h
    Created: 13 Jan 2022 11:42:22am
    Author:  James

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../External/SimpleComp/SimpleComp.h"

class DryComp
{
public:
    DryComp();
    void setThreshold(float val);
    void prepare(float sampleRate) noexcept;
    void processBlock(juce::AudioBuffer<float>& buffer);

private:

    chunkware_simple::SimpleComp compressor;

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> thresholdSmoothed;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DryComp)
};
