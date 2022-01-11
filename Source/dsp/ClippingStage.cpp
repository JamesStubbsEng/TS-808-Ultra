/*
  ==============================================================================

    ClippingStage.cpp
    Created: 7 Jan 2022 4:22:18pm
    Author:  James Stubbs

  ==============================================================================
*/

#include "ClippingStage.h"

ClippingStage::ClippingStage()
{
}

void ClippingStage::setDrive(float drive)
{
    auto audioTaperPotOutput = audioTaperPotSim(drive);
    p1Smoothed.setTargetValue(jmap(audioTaperPotOutput, 0.0f, 10.0f, 10.0f, rPot));
}

void ClippingStage::reset()
{
    clipWDFa.reset();
    clipWDFb.reset();
    clipWDFc.reset();
}

void ClippingStage::prepare(float sampleRate)
{
    fs = (float)sampleRate;

    p1Smoothed.setCurrentAndTargetValue(10.0f);
    p1Smoothed.reset(sampleRate, 0.05);

    clipWDFa.prepare(sampleRate);
    clipWDFb.prepare(sampleRate);
    clipWDFc.prepare(sampleRate);
}

float ClippingStage::processSample(float x) noexcept
{
    clipWDFc.setPotResitanceValue(p1Smoothed.getNextValue());

    const float clipWDFaOut = clipWDFa.processSample(x);
    const float clipWDFbOut = clipWDFb.processSample(clipWDFaOut);
    return clipWDFc.processSample(clipWDFbOut);
}

float ClippingStage::audioTaperPotSim(float in)
{
    jassert(in >= 0.0f && in <= 10.0f);

    if (in >= 0 && in <= 5.0f)
        return in / 5.0f;
    else 
        return  (9.0f / 5.0f) * in - 8.0f;
}
