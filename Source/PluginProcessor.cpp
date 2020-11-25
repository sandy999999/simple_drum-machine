/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SandysDrumMachineAudioProcessor::SandysDrumMachineAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
     ), parameters(*this, nullptr, "SandysDrumMachine", {
         std::make_unique<AudioParameterFloat>("Sound Choice", "Sound Choice", 0.0f, 4.0f, 0.0f)})
#endif
{
    parameters.addParameterListener("Sound Choice", this);
    samplerSetup();
}

SandysDrumMachineAudioProcessor::~SandysDrumMachineAudioProcessor()
{
}

//==============================================================================
const juce::String SandysDrumMachineAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SandysDrumMachineAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SandysDrumMachineAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SandysDrumMachineAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SandysDrumMachineAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SandysDrumMachineAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SandysDrumMachineAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SandysDrumMachineAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SandysDrumMachineAudioProcessor::getProgramName (int index)
{
    return {};
}

void SandysDrumMachineAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SandysDrumMachineAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	
    midiCollector.reset(sampleRate);

    sampler.setCurrentPlaybackSampleRate(sampleRate);
}

void SandysDrumMachineAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SandysDrumMachineAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
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

void SandysDrumMachineAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;

    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
	
    auto numSamples = buffer.getNumSamples();
	
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, numSamples);

    sampler.renderNextBlock(buffer, midiMessages, 0, numSamples);
}

//==============================================================================
bool SandysDrumMachineAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SandysDrumMachineAudioProcessor::createEditor()
{
    return new SandysDrumMachineAudioProcessorEditor (*this);
}

//==============================================================================
void SandysDrumMachineAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void SandysDrumMachineAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

void SandysDrumMachineAudioProcessor::samplerSetup()
{
    samplesFolder = File::getSpecialLocation(File::userDesktopDirectory).getChildFile("DrumMachineSounds");
    instruments = { "kick", "snare", "hihat", "conga", "tom" };


    for (int i = 0; i < numVoices; i++) {
        sampler.addVoice(new SamplerVoice());
    }

    // set up our AudioFormatManager class as detailed in the API docs
    // we can now use WAV and AIFF files!
    audioFormatManager.registerBasicFormats();
    loadSample();
}

void SandysDrumMachineAudioProcessor::loadSample()
{
    if (sampler.getNumSounds() != 0)
    {
        for (int i = 0; i < sampler.getNumSounds(); i++)
        {
            sampler.removeSound(i);
        }
    }

    auto soundChoice = parameters.getRawParameterValue("Sound Choice")->load();


        if (soundChoice == 0.0f)
        {
            for (int i = 0; i < sampler.getNumSounds(); i++)
            {
                sampler.removeSound(i);
            }
            addSamplerSound(instruments[0] + ".wav");
        }
        if (soundChoice == 1.0f)
        {
            for (int i = 0; i < sampler.getNumSounds(); i++)
            {
                sampler.removeSound(i);
            }
            addSamplerSound(instruments[1] + ".wav");
        }
        if (soundChoice == 2.0f)
        {
            for (int i = 0; i < sampler.getNumSounds(); i++)
            {
                sampler.removeSound(i);
            }
            addSamplerSound(instruments[2] + ".wav");
        }
        if (soundChoice == 3.0f)
        {
            for (int i = 0; i < sampler.getNumSounds(); i++)
            {
                sampler.removeSound(i);
            }
            addSamplerSound(instruments[3] + ".wav");
        }
        if (soundChoice == 4.0f)
        {
            for (int i = 0; i < sampler.getNumSounds(); i++)
            {
                sampler.removeSound(i);
            }
            addSamplerSound(instruments[4] + ".wav");
        }
}

void SandysDrumMachineAudioProcessor::addSamplerSound(String instrument)
{	
    File* file = new File(samplesFolder.getChildFile(instrument));

    ScopedPointer<AudioFormatReader> fileReader(audioFormatManager.createReaderFor(*file));

	// allow our sound to be played on all notes
    BigInteger allNotes;
    allNotes.setRange(0, 128, true);

    sampler.addSound(new SamplerSound(instrument, *fileReader, allNotes, 60, 0, 10, 10.0));

	/*
    fileReader = nullptr;
    delete file;
    */
}

void SandysDrumMachineAudioProcessor::parameterChanged(const String& parameterID, float newValue)
{
    if (sampler.getNumSounds() != 0)
    {
        for (int i = 0; i < sampler.getNumSounds(); i++)
        {
            sampler.removeSound(i);
        }
    }

    DBG(newValue);
    auto soundChoice = newValue;

    if (soundChoice == 0.0f)
    {
        addSamplerSound(instruments[0] + ".wav");
    }
    if (soundChoice == 1.0f)
    {
        addSamplerSound(instruments[1] + ".wav");
    }
    if (soundChoice == 2.0f)
    {
        addSamplerSound(instruments[2] + ".wav");
    }
    if (soundChoice == 3.0f)
    {
        addSamplerSound(instruments[3] + ".wav");
    }
    if (soundChoice == 4.0f)
    {
        addSamplerSound(instruments[4] + ".wav");
    }
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SandysDrumMachineAudioProcessor();
}
