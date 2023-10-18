/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <iostream> //for print statements
using namespace std;

//==============================================================================
CustomImpulseDelayAudioProcessor::CustomImpulseDelayAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

CustomImpulseDelayAudioProcessor::~CustomImpulseDelayAudioProcessor()
{
}

//==============================================================================
const juce::String CustomImpulseDelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CustomImpulseDelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CustomImpulseDelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CustomImpulseDelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CustomImpulseDelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CustomImpulseDelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CustomImpulseDelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CustomImpulseDelayAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String CustomImpulseDelayAudioProcessor::getProgramName (int index)
{
    return {};
}

void CustomImpulseDelayAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void CustomImpulseDelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    sample_rate = sampleRate;


    //delay times in ms
    //TODO write tempo/beat conversion to time

    //loop through delay lines to prep them
    //
    for (int i = 0; i < sizeof(delay) / sizeof(delay[0]); i++) {
        delay[i].setMaximumDelayInSamples(192000);
        delay[i].reset();
        delay[i].prepare(spec);
        delay[i].setDelay(base_time*(i+1));
    }

    for (int i = 0; i < sizeof(LPFs) / sizeof(LPFs[0]); i++) {
        LPFs[i].prepare(spec);
        LPFs[i].setType(juce::dsp::StateVariableTPTFilterType::lowpass);
        LPFs[i].setCutoffFrequency(filterCutoffs[i]);
    }

 

}

void CustomImpulseDelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CustomImpulseDelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void CustomImpulseDelayAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* inSamples = buffer.getReadPointer(channel);
        auto* outSamples = buffer.getWritePointer(channel);

        for (int n = 0; n < buffer.getNumSamples(); n++) {

            //get the input
            float input = inSamples[n];

            //initialize output
            float output = 0;

            
            for (int i = 0; i < sizeof(delay) / sizeof(delay[0]); i++) {
                float pop = delay[i].popSample(channel, -1, true);
                pop = LPFs[i + 1].processSample(channel, pop);

                output = output + pop*delayLineGains[i+1];

            }

            for (int i = 0; i < sizeof(delay) / sizeof(delay[0]); i++) {
                delay[i].pushSample(channel, input);
            }
            

            //output = tempdelay.popSample(channel, -1, true);
            //tempdelay.pushSample(channel, input);
            output = output + LPFs[0].processSample(channel, input*delayLineGains[0]); //the first slider is simply the dry knob
            outSamples[n] = output;



            //set the delay time differently if things have changed
            if (change_time_flag) {
                for (int i = 0; i < sizeof(delay) / sizeof(delay[0]); i++) {
                    delay[i].setDelay(base_time * (i + 1));
                    change_time_flag = false;
                }
            }

            if (change_cutoff_flag) {
                for (int i = 0; i < sizeof(LPFs) / sizeof(LPFs[0]); i++) {
                    LPFs[i].setCutoffFrequency(filterCutoffs[i]);
                    change_cutoff_flag = false;
                }
            }
        }

    }



 
    
}


//converter functions

int CustomImpulseDelayAudioProcessor::convert_ms_to_samples(float ms) {
    float samples = (ms/1000)*my_sample_rate;
    return (int)samples;
}

float CustomImpulseDelayAudioProcessor::convert_tempo_to_ms(float bpm) {
    float ms_per_minute = 60 * 1000;

    float ms_per_beat = ms_per_minute / bpm;

    return ms_per_beat;
}

//==============================================================================
bool CustomImpulseDelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CustomImpulseDelayAudioProcessor::createEditor()
{
    return new CustomImpulseDelayAudioProcessorEditor (*this);
}

//==============================================================================
void CustomImpulseDelayAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void CustomImpulseDelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CustomImpulseDelayAudioProcessor();
}

