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
    p1Smoothed.setTargetValue(jmap(drive, 0.0f, 10.0f, 10.0f, rPot));
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
