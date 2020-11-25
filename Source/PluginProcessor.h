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
class SandysDrumMachineAudioProcessor  : public juce::AudioProcessor, AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    SandysDrumMachineAudioProcessor();
    ~SandysDrumMachineAudioProcessor() override;

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

    void samplerSetup();
    void addSamplerSound(String instrument);
    void loadSample();
    void parameterChanged(const String& parameterID, float newValue) override;

    AudioProcessorValueTreeState parameters;


private:
	
    Synthesiser sampler;

    File samplesFolder;
    StringArray instruments;

	//one voice for each rhythm generator
    const int numVoices = { 4 };

    AudioFormatManager audioFormatManager;

    MidiMessageCollector midiCollector;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SandysDrumMachineAudioProcessor)
};
