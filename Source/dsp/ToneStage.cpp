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
    auto gTaperPotOutput = taperPotSim(tone);
    rLSmoothed.setTargetValue(jmap(gTaperPotOutput, 0.0f, 10.0f, 10.0f, rPot));
}

void ToneStage::prepare(float sampleRate)
{
    chowdsp::IIRFilter<2>::reset();
    fs = (float)sampleRate;

    rLSmoothed.setCurrentAndTargetValue(10.0f);
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

float ToneStage::taperPotSim(float in)
{
    jassert(in >= 0.0f && in <= 10.0f);

    // g taper
    //if (in >= 0 && in <= 1.0f)
    //    return 3.0f * in;
    //else if (in > 1.0f && in <= 9.0f)
    //    return 0.5f * in + 2.5f;
    //else
    //    return 3.0f * in - 20.0f;

    // inverse z
    //if (in >= 0 && in <= 4.0f)
    //    return 0.5f * in;
    //else if (in > 4.0f && in <= 6.0f)
    //    return 3.0f * in - 10.0f;
    //else
    //    return 0.5f * in + 5.0f;

    // inverse log taper
    if (in >= 0 && in <= 5.0f)
        return (9.0f / 5.0f) * in;
    else
        return  (1.0f / 5.0f) * in + 8.0f;
}
