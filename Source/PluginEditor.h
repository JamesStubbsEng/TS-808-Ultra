/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class TS808UltraAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    TS808UltraAudioProcessorEditor (TS808UltraAudioProcessor&, AudioProcessorValueTreeState& vts);
    ~TS808UltraAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    

private:
    TS808UltraAudioProcessor& audioProcessor;

    AudioProcessorValueTreeState& valueTreeState;

    typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

    Slider driveSlider;
    Slider toneSlider;
    Slider mixSlider;
    Slider filterSlider;
    Slider drySquashSlider;
    Slider gainSlider;

    Label driveLabel;
    Label toneLabel;
    Label mixLabel;
    Label filterLabel;
    Label drySquashLabel;
    Label gainLabel;

    std::unique_ptr<SliderAttachment> driveSliderAttachment;
    std::unique_ptr<SliderAttachment> toneSliderAttachment;
    std::unique_ptr<SliderAttachment> mixSliderAttachment;
    std::unique_ptr<SliderAttachment> filterSliderAttachment;
    std::unique_ptr<SliderAttachment> drySquashSliderAttachment;
    std::unique_ptr<SliderAttachment> gainSliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TS808UltraAudioProcessorEditor)
};
