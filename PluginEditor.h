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
class CustomImpulseDelayAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Slider::Listener
{
public:
    CustomImpulseDelayAudioProcessorEditor (CustomImpulseDelayAudioProcessor&);
    ~CustomImpulseDelayAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(juce::Slider* slider) override;


private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    CustomImpulseDelayAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomImpulseDelayAudioProcessorEditor)
    juce::Slider sliders[8];
    juce::Slider filterKnobs[8];
    juce::NormalisableRange<double> logRange{ 20.0f, 20000.0f, 1.0f, 0.2f };

    juce::Slider timeKnob;
    juce::Slider sample_ms_beat_switch;
};
