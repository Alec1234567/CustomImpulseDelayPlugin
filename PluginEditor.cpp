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
    setSize (800, 600);
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


    division.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);

    //need to figure out how to get 1,2/3,1/2,1/3,1/4,1/5,1/6,1/7,1/8
    division.setRange(1,16,1);
    division.setValue(0);
    division.addListener(this);
    addAndMakeVisible(division);

    sample_ms_beat_switch.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    sample_ms_beat_switch.setRange(1, 3,1);
    sample_ms_beat_switch.addListener(this);
    addAndMakeVisible(sample_ms_beat_switch);


    gain_exp_decay_button.setButtonText("Gain");
    gain_exp_decay_button.setClickingTogglesState(true);
    gain_exp_decay_button.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::aqua);
    addAndMakeVisible(gain_exp_decay_button);

    filter_exp_decay_button.setButtonText("Cutoff");
    filter_exp_decay_button.setClickingTogglesState(true);
    filter_exp_decay_button.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::aqua);
    addAndMakeVisible(filter_exp_decay_button);

    
    
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

double CustomImpulseDelayAudioProcessorEditor::division_from_slider(double slider_value) {
    //map [-8,4] to 1/8 to 4x
    return (1/slider_value)*4;
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
    
    for (int i = 0; i < sizeof(sliders) / sizeof(sliders[0]); i++) {
        sliders[i].setBounds(i * sliderWidth + leftMargin, topMargin, sliderWidth, sliderHeight);
    }

    for (int i = 0; i < sizeof(filterKnobs) / sizeof(filterKnobs[0]); i++) {
        filterKnobs[i].setBounds(i * sliderWidth + leftMargin, topMargin*2+sliderHeight, sliderWidth, sliderHeight);
    }

    timeKnob.setBounds(leftMargin, topMargin*2+3*sliderHeight, 100, 100);
    timeKnob.setTitle("Delay Time");

    sample_ms_beat_switch.setBounds(leftMargin+2*sliderWidth, topMargin * 2 + 3 * sliderHeight, 50, 50);
    
    division.setBounds(leftMargin + 4 * sliderWidth, topMargin * 2 + 3 * sliderHeight, 50, 50);

    int button_width = sliderWidth;
    int button_height = button_width/2;
   
    gain_exp_decay_button.setBounds(getWidth() - (button_width + leftMargin), topMargin * 3 + 2 * sliderHeight, button_width, button_height);
    filter_exp_decay_button.setBounds(getWidth() - (button_width + leftMargin), topMargin * 4 + 2 * sliderHeight + button_height, button_width, button_height);
}





void CustomImpulseDelayAudioProcessorEditor::sliderValueChanged(juce::Slider* slider) {

    //update parameters based on slider position


    //normal mode
    if(!gain_exponential_decay_mode){
        for (int i = 0; i < sizeof(sliders) / sizeof(sliders[0]); i++) {
            if (slider == &sliders[i]) {
                audioProcessor.delayLineGains[i] = sliders[i].getValue();
            }
        }
    }
    else { //gain exponential decay mode
     
        //changing the first slider only changes the a value
        if (slider == &sliders[0]) {
            audioProcessor.delayLineGains[0] = sliders[0].getValue();
            gain_exponential_parameter_a = audioProcessor.delayLineGains[0];
            for (int i = 0; i < sizeof(sliders) / sizeof(sliders[0]); i++) {
                audioProcessor.delayLineGains[i] = std::min(gain_exponential_parameter_a * pow(gain_exponential_parameter_b, i),maxGain);
                sliders[i].setValue(audioProcessor.delayLineGains[i]);
            }
        }

        else {

            for (int i = 1; i < (sizeof(sliders) / sizeof(sliders[0])); i++) {

                if (slider == &sliders[i]) {
                    audioProcessor.delayLineGains[i] = sliders[i].getValue();
                    float x2 = i;
                    float y2 = audioProcessor.delayLineGains[i];
                    float y1 = gain_exponential_parameter_a;
                 
                    gain_exponential_parameter_b = pow((y2 / y1), 1 / x2);

                    for (int j = 0; j < sizeof(sliders) / sizeof(sliders[0]); j++) {

                        audioProcessor.delayLineGains[j] = std::min(gain_exponential_parameter_a * pow(gain_exponential_parameter_b, j),maxGain);


                        //>>>>>>HERE IS THE CALL THAT CAUSES THE LOOP<<<<<<<<<

                        sliders[j].setValue(audioProcessor.delayLineGains[j],juce::dontSendNotification);
                        
                    }
                    
                }
            }
        }
    }



    for (int i = 0; i < sizeof(sliders) / sizeof(sliders[0]); i++) {
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

    if (slider == &division) {
        audioProcessor.division_value = division_from_slider(division.getValue());
        audioProcessor.change_time_flag = true;
    }
}