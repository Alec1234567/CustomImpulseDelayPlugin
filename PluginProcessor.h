/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class CustomImpulseDelayAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    CustomImpulseDelayAudioProcessor();
    ~CustomImpulseDelayAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;


    float delayLineGains[8] = { 1,1,1,1,1,1,1,1 };
    float filterCutoffs[8] = { 20000,20000 ,20000 ,20000 ,20000,20000,20000,20000 };

    int default_tempo = 100;
    int base_time = convert_ms_to_samples(convert_tempo_to_ms(default_tempo));
    bool change_time_flag = false;
    bool change_cutoff_flag = false;
    int sample_ms_beat;
    double my_sample_rate = 44100;

    int convert_ms_to_samples(float ms);
    float convert_tempo_to_ms(float tempo);
   

private:

    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delay[7];
    juce::dsp::StateVariableTPTFilter<float> LPFs[8];

    double sample_rate = 44100;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomImpulseDelayAudioProcessor)



};
