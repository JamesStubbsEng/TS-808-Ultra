/*
  ==============================================================================

    ToneStage.cpp
    Created: 7 Jan 2022 4:22:28pm
    Author:  James Stubbs

  ==============================================================================
*/

#include "ToneStage.h"

ToneStage::ToneStage()
{
}

void ToneStage::setTone(float tone)
{
    rLSmoothed.setTargetValue(jmap(tone, 0.0f, 10.0f, 10.0f, rPot));
}

void ToneStage::prepare(float sampleRate)
{
    chowdsp::IIRFilter<2>::reset();
    fs = (float)sampleRate;

    rLSmoothed.setCurrentAndTargetValue(rLSmoothed.getTargetValue());
    rLSmoothed.reset(sampleRate, 0.05);

    calcCoefs(rLSmoothed.getTargetValue());
}

void ToneStage::calcCoefs(float rL)
{
    // based on analog transfer funtion in 
    // https://ccrma.stanford.edu/~dtyeh/papers/yeh07_dafx_distortion.pdf
    // on page 6

    constexpr float Cs = (float)0.22e-6;
    constexpr float Rs = (float)1e3;
    constexpr float Ri = (float)10e3;
    constexpr float Cz = (float)0.22e-6;
    constexpr float Rz = 220.0f;
    constexpr float Rf = (float)1e3;

    constexpr float wp = 1.0f / (Cs * Rs * Ri / (Rs + Ri));
   
    const float Rl = rL;
    const float Rr = rPot - rL;

    const float wz = 1.0f / (Cz * (Rz + (Rl * Rr / (Rl + Rr))));

    const float Y = (Rl + Rr) * (Rz + (Rl * Rr / (Rl + Rr)));
    const float X = (Rr / (Rl + Rr)) / ((Rz + (Rl * Rr / (Rl + Rr))) * Y);
    const float W = Y / ((Rl * Rf) + Y);

    const float alpha = (Rl * Rf + Y) / (Y * Rs * Cs);

    // get analog coefficients
    float as[3], bs[3];
    bs[0] = 0.0f;
    bs[1] = alpha;
    bs[2] = alpha * W * wz;

    as[0] = 1.0f;
    as[1] = wp + wz + X;
    as[2] = wp * wz;

    // frequency warping (Use chowdsp for now to find wc. Later run matlab sim)

    const float wc = chowdsp::Bilinear::calcPoleFreq(as[0], as[1], as[2]);
    const auto K = wc == 0.0f ? 2.0f * fs : wc / std::tan(wc / (2.0f * fs));

    chowdsp::Bilinear::BilinearTransform<float, 3>::call(b, a, bs, as, K);

}

void ToneStage::processBlock(float* block, const int numSamples) noexcept
{
    if (rLSmoothed.isSmoothing())
    {
        for (int n = 0; n < numSamples; ++n)
        {
            calcCoefs(rLSmoothed.getNextValue());
            block[n] = processSample(block[n]);
        }
    }
    else
    {
        chowdsp::IIRFilter<2>::processBlock(block, numSamples);
    }
}
