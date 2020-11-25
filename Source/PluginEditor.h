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
class SandysDrumMachineAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SandysDrumMachineAudioProcessorEditor (SandysDrumMachineAudioProcessor&);
    ~SandysDrumMachineAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
	
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SandysDrumMachineAudioProcessor& audioProcessor;

    Label textLabel{ {}, "Sound Choice" };
    Font textFont{ 12.0f };
    ComboBox soundChoice;

    ScopedPointer<AudioProcessorValueTreeState::ComboBoxAttachment> soundSelection;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SandysDrumMachineAudioProcessorEditor)
};
