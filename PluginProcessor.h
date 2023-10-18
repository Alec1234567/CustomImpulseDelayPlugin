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

    int base_time = 10000;
    bool change_time_flag = false;
    bool change_cutoff_flag = false;
   

private:

    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delay[7];
    juce::dsp::StateVariableTPTFilter<float> LPFs[8];

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomImpulseDelayAudioProcessor)



};
