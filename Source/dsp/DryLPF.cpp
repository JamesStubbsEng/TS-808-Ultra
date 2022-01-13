/*
  ==============================================================================

    DryLPF.cpp
    Created: 13 Jan 2022 11:39:54am
    Author:  James

  ==============================================================================
*/

#include "DryLPF.h"

DryLPF::DryLPF()
{
}

void DryLPF::setCutoff(float val)
{
    //skew here: 
    // y = 0 to 1
    // y = ((x-min)/(max-min))^factor
    float skewedValue = std::powf((val - 0.0f) / (10.0f - 0.0f), 0.1f);

    auto cutoffInHz = jmap(skewedValue, 0.0f, 1.0f, 20000.0f, 20.0f);
    cutoffSmoothed.setTargetValue(cutoffInHz);
}

void DryLPF::prepare(const juce::dsp::ProcessSpec& spec) noexcept
{
    sampleRate = spec.sampleRate;
    lpFilter.prepare(spec);
    lpFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    cutoffSmoothed.reset(sampleRate, 0.05);
}

void DryLPF::reset()
{
    lpFilter.reset();
}

