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
    cutoffSmoothed.setTargetValue(val);
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

