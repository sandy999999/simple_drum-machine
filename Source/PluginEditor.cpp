/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SandysDrumMachineAudioProcessorEditor::SandysDrumMachineAudioProcessorEditor (SandysDrumMachineAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    addAndMakeVisible(textLabel);
    textLabel.setFont(textFont);
    addAndMakeVisible(soundChoice);
    soundChoice.addItem("Kick", 1);
    soundChoice.addItem("Snare", 2);
    soundChoice.addItem("HiHat", 3);
    soundChoice.addItem("Conga", 4);
    soundChoice.addItem("Tom", 5);

    soundChoice.setJustificationType((Justification::centred));

    soundChoice.setSelectedId(1);

    soundSelection = new AudioProcessorValueTreeState::ComboBoxAttachment(audioProcessor.parameters, "Sound Choice", soundChoice);

    setSize (200, 200);
}

SandysDrumMachineAudioProcessorEditor::~SandysDrumMachineAudioProcessorEditor()
{
}

//==============================================================================
void SandysDrumMachineAudioProcessorEditor::paint (juce::Graphics& g)
{
}

void SandysDrumMachineAudioProcessorEditor::resized()
{
    Rectangle<int> area = getLocalBounds().reduced(40);

    soundChoice.setBounds(area.removeFromTop(20));
}
