/*
  ==============================================================================

    DryComp.cpp
    Created: 13 Jan 2022 11:42:22am
    Author:  James

  ==============================================================================
*/

#include "DryComp.h"

DryComp::DryComp()
{
    compressor.setAttack(10.0);
    compressor.setRelease(50.0);
    compressor.setRatio(1.0 / 4.0);
}

void DryComp::setThreshold(float val)
{
    // apply exponential skewing to threshold value
    //skew here: 
    // y = 0 to 1
    // y = ((x-min)/(max-min))^factor
    float skewedValue = std::powf((val - 0.0f) / (10.0f - 0.0f), 2.0f);

    auto thesholdInDB = jmap(skewedValue, 0.0f, 1.0f, 0.0f, -80.0f);
    thresholdSmoothed.setTargetValue(thesholdInDB);
}

void DryComp::prepare(float sampleRate) noexcept
{
    compressor.setSampleRate(sampleRate);
    compressor.initRuntime();
    thresholdSmoothed.reset(sampleRate, 0.05);
}

void DryComp::processBlock(juce::AudioBuffer<float>& buffer)
{
    auto numChannels = buffer.getNumChannels();
    auto numSamples = buffer.getNumSamples();

    jassert(numChannels < 3 && numChannels > 0);

    if (numChannels == 1)
    {
        auto* in = buffer.getReadPointer(0);
        auto* out = buffer.getWritePointer(0);

        for (size_t i = 0; i < numSamples; ++i)
        {
            compressor.setThresh(thresholdSmoothed.getNextValue());
            double currBlockL = in[i];
            double currBlockR = in[i];
            compressor.process(currBlockL, currBlockR);
            out[i] = static_cast<float>(currBlockL);
        }
    }
    else
    {
        auto* inL = buffer.getReadPointer(0);
        auto* outL = buffer.getWritePointer(0);
        auto* inR = buffer.getReadPointer(1);
        auto* outR = buffer.getWritePointer(1);

        for (size_t i = 0; i < numSamples; ++i)
        {
            compressor.setThresh(thresholdSmoothed.getNextValue());

            double leftBlock = inL[i];
            double rightBlock = inR[i];

            compressor.process(leftBlock, rightBlock);
            outL[i] = static_cast<float>(leftBlock);
            outR[i] = static_cast<float>(rightBlock);
        }
    }
}


