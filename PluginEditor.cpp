/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CustomImpulseDelayAudioProcessorEditor::CustomImpulseDelayAudioProcessorEditor (CustomImpulseDelayAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    for (int i = 0; i < sizeof(sliders) / sizeof(sliders[0]); i++) {
        sliders[i].setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
        sliders[i].setRange(0, 2, 0.01);
        sliders[i].setValue(1);
        sliders[i].addListener(this);
        addAndMakeVisible(sliders[i]);
    }


    
    for (int i = 0; i < sizeof(filterKnobs) / sizeof(filterKnobs[0]); i++) {
        filterKnobs[i].setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
        filterKnobs[i].setNormalisableRange(logRange);
        filterKnobs[i].setValue(20000);
        filterKnobs[i].addListener(this);
        addAndMakeVisible(filterKnobs[i]);
    }


    timeKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    timeKnob.setRange(40, 200);
    timeKnob.setValue(audioProcessor.default_tempo);
    timeKnob.addListener(this);
    addAndMakeVisible(timeKnob);

    sample_ms_beat_switch.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    sample_ms_beat_switch.setRange(1, 3,1);
    sample_ms_beat_switch.addListener(this);
    addAndMakeVisible(sample_ms_beat_switch);
    
}

CustomImpulseDelayAudioProcessorEditor::~CustomImpulseDelayAudioProcessorEditor()
{
}

//==============================================================================
void CustomImpulseDelayAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void CustomImpulseDelayAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    //GUI DESIGN

    //make margins so components don't reach the edge
    auto leftMargin = getWidth() * 0.02;
    auto topMargin = getHeight() * 0.04;
    auto sliderWidth = (getWidth() - (2 * leftMargin)) / 8;
    auto sliderHeight = (getHeight() - (2 * topMargin)) / 4;
    int width = 50;
    int height = 100;

    for (int i = 0; i < sizeof(sliders) / sizeof(sliders[0]); i++) {
        sliders[i].setBounds(i * sliderWidth + leftMargin, topMargin, sliderWidth, sliderHeight);
    }

    for (int i = 0; i < sizeof(filterKnobs) / sizeof(filterKnobs[0]); i++) {
        filterKnobs[i].setBounds(i * sliderWidth + leftMargin, topMargin*2+sliderHeight, sliderWidth, sliderHeight);
    }

    timeKnob.setBounds(leftMargin, topMargin*2+3*sliderHeight, 100, 100);
    timeKnob.setTitle("Delay Time");

    sample_ms_beat_switch.setBounds(leftMargin+2*sliderWidth, topMargin * 2 + 3 * sliderHeight, 50, 50);
    

    
}

/// <summary>
/// ///
/// </summary>
/// <param name="slider"></param>

void CustomImpulseDelayAudioProcessorEditor::sliderValueChanged(juce::Slider* slider) {

    //update parameters based on slider position

    for (int i = 0; i < sizeof(sliders) / sizeof(sliders[0]); i++) {
        if (slider == &sliders[i]) {
            audioProcessor.delayLineGains[i] = sliders[i].getValue();
        }
        
        if (slider == &filterKnobs[i]) {
            audioProcessor.filterCutoffs[i] = filterKnobs[i].getValue();
            audioProcessor.change_cutoff_flag = true;
        }
    }
    if (slider == &timeKnob) {
        audioProcessor.base_time = audioProcessor.convert_ms_to_samples(audioProcessor.convert_tempo_to_ms(timeKnob.getValue()));
        audioProcessor.change_time_flag = true;
    }

    if (slider == &sample_ms_beat_switch) {
        audioProcessor.sample_ms_beat = sample_ms_beat_switch.getValue();
    }

}