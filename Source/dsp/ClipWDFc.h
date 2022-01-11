/*
  ==============================================================================

    ClipWDFc.h
    Created: 11 Jan 2022 12:48:58pm
    Author:  James

    The third WDF stage of the TS clipping stage as outlined by page 7 of
    https://www.researchgate.net/publication/299514713_An_Improved_and_Generalized_Diode_Clipper_Model_for_Wave_Digital_Filters

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

using namespace chowdsp::WDFT;

class ClipWDFc
{
public:
    ClipWDFc() = default;

    void prepare(double sampleRate)
    {
        C4.prepare((float)sampleRate);
    }

    void reset()
    {
        C4.reset();
    }

    void setPotResitanceValue(float newPotR)
    {
        constexpr float R6 = (float)51e3;
        Is.setResistanceValue(R6 + newPotR);
    }

    // Takes current and returns voltage
    inline float processSample(float x)
    {
        Is.setCurrent(x);

        dp.incident(P1.reflected());
        auto y = voltage<float>(C4);
        P1.incident(dp.reflected());

        return y;
    }

private:
    ResistiveCurrentSourceT<float> Is;

    CapacitorT<float> C4{ 51.0e-12f };  

    WDFParallelT<float, decltype (Is), decltype (C4)> P1{ Is, C4 };

    // 1N914 diode pair at 25C and VR = 20V
    DiodePairT<float, decltype (P1)> dp{ P1, 25e-9f };
   
};