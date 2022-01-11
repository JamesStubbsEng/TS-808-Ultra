/*
  ==============================================================================

    ClipWDFb.h
    Created: 11 Jan 2022 12:48:43pm
    Author:  James

    The second WDF stage of the TS clipping stage as outlined by page 7 of
    https://www.researchgate.net/publication/299514713_An_Improved_and_Generalized_Diode_Clipper_Model_for_Wave_Digital_Filters

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

using namespace chowdsp::WDFT;

class ClipWDFb
{
public:
    ClipWDFb() = default;

    void prepare(double sampleRate)
    {
        C3.prepare((float)sampleRate);
    }

    void reset()
    {
        C3.reset();
    }

    // Takes voltage and returns current
    inline float processSample(float x)
    {
        Vs.setVoltage(x);

        Vs.incident(S1.reflected());
        auto y = current<float>(R4);
        S1.incident(Vs.reflected());

        return y;
    }

private:
    ResistorT<float> R4{ 4700.0f };

    CapacitorT<float> C3{ 47.0e-9f, 48000.0f };

    WDFSeriesT<float, decltype (C3), decltype (R4)> S1{ C3, R4 };

    IdealVoltageSourceT<float, decltype (S1)> Vs{ S1 };
};