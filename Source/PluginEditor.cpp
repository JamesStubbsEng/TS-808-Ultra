/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TS808UltraAudioProcessorEditor::TS808UltraAudioProcessorEditor (TS808UltraAudioProcessor& p, AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), valueTreeState(vts)
{
    driveSlider.setSliderStyle(juce::Slider::Rotary);
    driveSlider.setScrollWheelEnabled(false);
    driveSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 30);
    driveSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    driveLabel.setText("Drive", NotificationType::dontSendNotification);
    driveLabel.attachToComponent(&driveSlider, false);
    addAndMakeVisible(driveSlider);
    driveSliderAttachment.reset(new SliderAttachment(valueTreeState, "drive", driveSlider));

    toneSlider.setSliderStyle(juce::Slider::Rotary);
    toneSlider.setScrollWheelEnabled(false);
    toneSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 30);
    toneSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    toneLabel.setText("Tone", NotificationType::dontSendNotification);
    toneLabel.attachToComponent(&toneSlider, false);
    addAndMakeVisible(toneSlider);
    toneSliderAttachment.reset(new SliderAttachment(valueTreeState, "tone", toneSlider));

    mixSlider.setSliderStyle(juce::Slider::Rotary);
    mixSlider.setScrollWheelEnabled(false);
    mixSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 30);
    mixSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    mixLabel.setText("Mix", NotificationType::dontSendNotification);
    mixLabel.attachToComponent(&mixSlider, false);
    addAndMakeVisible(mixSlider);
    mixSliderAttachment.reset(new SliderAttachment(valueTreeState, "mix", mixSlider));

    filterSlider.setSliderStyle(juce::Slider::Rotary);
    filterSlider.setScrollWheelEnabled(false);
    filterSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 30);
    filterSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    filterLabel.setText("Filter", NotificationType::dontSendNotification);
    filterLabel.attachToComponent(&filterSlider, false);
    addAndMakeVisible(filterSlider);
    filterSliderAttachment.reset(new SliderAttachment(valueTreeState, "filter", filterSlider));

    drySquashSlider.setSliderStyle(juce::Slider::Rotary);
    drySquashSlider.setScrollWheelEnabled(false);
    drySquashSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 30);
    drySquashSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    drySquashLabel.setText("Dry Squash", NotificationType::dontSendNotification);
    drySquashLabel.attachToComponent(&drySquashSlider, false);
    addAndMakeVisible(drySquashSlider);
    drySquashSliderAttachment.reset(new SliderAttachment(valueTreeState, "drySquash", drySquashSlider));

    gainSlider.setSliderStyle(juce::Slider::Rotary);
    gainSlider.setScrollWheelEnabled(false);
    gainSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 30);
    gainSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    gainLabel.setText("Gain", NotificationType::dontSendNotification);
    gainLabel.attachToComponent(&gainSlider, false);
    addAndMakeVisible(gainSlider);
    gainSliderAttachment.reset(new SliderAttachment(valueTreeState, "gain", gainSlider));

    setSize (500, 400);
}

TS808UltraAudioProcessorEditor::~TS808UltraAudioProcessorEditor()
{
}

//==============================================================================
void TS808UltraAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void TS808UltraAudioProcessorEditor::resized()
{
    auto area = getBounds().reduced(20);
    auto sliderWidth = area.getWidth() / 3;
    auto sliderHeight = area.getHeight() / 2;

    auto topRow = area.removeFromTop(sliderHeight);

    driveSlider.setBounds(topRow.removeFromLeft(sliderWidth).reduced(30));
    toneSlider.setBounds(topRow.removeFromLeft(sliderWidth).reduced(30));
    mixSlider.setBounds(topRow.removeFromLeft(sliderWidth).reduced(30));

    filterSlider.setBounds(area.removeFromLeft(sliderWidth).reduced(30));
    drySquashSlider.setBounds(area.removeFromLeft(sliderWidth).reduced(30));
    gainSlider.setBounds(area.removeFromLeft(sliderWidth).reduced(30));
}
