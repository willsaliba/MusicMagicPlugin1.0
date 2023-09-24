#include "PluginProcessor.h"
#include "PluginEditor.h"

MusicMagicAudioProcessor::MusicMagicAudioProcessor() : AudioProcessor (BusesProperties()
        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
    )
{
    //input1
    firstFormatManager.registerBasicFormats();
    firstSampler.addVoice(new juce::SamplerVoice());
    //input2
    secondFormatManager.registerBasicFormats();
    secondSampler.addVoice(new juce::SamplerVoice());
    //output
    outputFormatManager.registerBasicFormats();
    outputSampler.addVoice(new juce::SamplerVoice());
}

MusicMagicAudioProcessor::~MusicMagicAudioProcessor()
{
    clearInputSampler();
    clearSecondSampler();
    clearOutputSampler();
}

// PLAYING SOUND =================================================================

void MusicMagicAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    //clear any other sounds playing
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    int actualBufferChannels = buffer.getNumChannels();
    for (auto i = totalNumInputChannels; i < juce::jmin(totalNumOutputChannels, actualBufferChannels); ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    //for playhead
    juce::MidiMessage m;
    for (const juce::MidiMessageMetadata m : midiMessages) {
        juce::MidiMessage message = m.getMessage();
        if (message.isNoteOn()) {
            isNotePlayed = true;
        } else if (message.isNoteOff()) {
            isNotePlayed = false;
        }
    }
    sampleCount = isNotePlayed ? sampleCount += buffer.getNumSamples() : 0;

    //playing the correct track
    if (trackPlaying == "in1") {
        firstSampler.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    } else if (trackPlaying == "in2") {
        secondSampler.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    } else {
        outputSampler.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    }
}

void MusicMagicAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    firstSampler.setCurrentPlaybackSampleRate(sampleRate);
    secondSampler.setCurrentPlaybackSampleRate(sampleRate);
    outputSampler.setCurrentPlaybackSampleRate(sampleRate);
}

void MusicMagicAudioProcessor::sendMIDInotes()
{
    sendNoSound();
    juce::MidiBuffer midiMessages;
    midiMessages.addEvent(juce::MidiMessage::noteOn(1, 60, (juce::uint8)127), 0);
    juce::AudioBuffer<float> tempBuffer;
    processBlock(tempBuffer, midiMessages);
}

void MusicMagicAudioProcessor::sendNoSound()
{
    juce::MidiBuffer midiMessages;
    midiMessages.addEvent(juce::MidiMessage::noteOff(1, 60), 0);
    juce::AudioBuffer<float> tempBuffer;
    processBlock(tempBuffer, midiMessages);
}

// MANAGING TRACKS ===============================================================

void MusicMagicAudioProcessor::loadInputFile()
{
    firstSampler.clearSounds();
    //selecting file from directory
    juce::FileChooser chooser { "Please Load a First Input File" };
    //if sound chosen
    if ( chooser.browseForFileToOpen() ) {
        //adding file
        inputTrack = chooser.getResult();
        inputTrackPath = inputTrack.getFullPathName();
        firstFormatReader = firstFormatManager.createReaderFor(inputTrack);
        //draw waveform
        firstWaveForm.setSize(1, static_cast<int>(firstFormatReader->lengthInSamples));
        firstFormatReader->read(&firstWaveForm, 0, static_cast<int>(firstFormatReader->lengthInSamples), 0, true, false);
        //adding sound to Sampler
        juce::BigInteger range;
        range.setRange(0, 128, true);
        firstSampler.addSound( new juce::SamplerSound ("Sample", *firstFormatReader, range, 60, 0.1, 0.1, 10.0));
    }
}

void MusicMagicAudioProcessor::loadInputFile(const juce::String& path)
{
    if (inputTrackPath == "null") {
        firstSampler.clearSounds();
        //adding file using path retrieved by UI
        inputTrackPath = path;
        inputTrack = juce::File(path);
        firstFormatReader = firstFormatManager.createReaderFor(inputTrack);
        //draw waveform
        firstWaveForm.setSize(1, static_cast<int>(firstFormatReader->lengthInSamples));
        firstFormatReader->read(&firstWaveForm, 0, static_cast<int>(firstFormatReader->lengthInSamples), 0, true, false);
        //adding sound to Sampler
        juce::BigInteger range;
        range.setRange(0, 128, true);
        firstSampler.addSound( new juce::SamplerSound ("Sample", *firstFormatReader, range, 60, 0.1, 0.1, 10.0));
    } else if (inputTrackPath != "null" && secondInputTrackPath == "null") {
        secondSampler.clearSounds();
        //adding file using path retrieved by UI
        secondInputTrackPath = path;
        secondInputTrack = juce::File(path);
        secondFormatReader = secondFormatManager.createReaderFor(secondInputTrack);
        //draw waveform
        secWaveForm.setSize(1, static_cast<int>(secondFormatReader->lengthInSamples));
        secondFormatReader->read(&secWaveForm, 0, static_cast<int>(secondFormatReader->lengthInSamples), 0, true, false);
        //adding sound to Sampler
        juce::BigInteger range;
        range.setRange(0, 128, true);
        secondSampler.addSound( new juce::SamplerSound ("Sample", *secondFormatReader, range, 60, 0.1, 0.1, 10.0));
    }
}

void MusicMagicAudioProcessor::clearInputSampler()
{
    inputTrackPath = "null";
    firstSampler.clearSounds();
    if (firstFormatReader) {
        delete firstFormatReader;
        firstFormatReader = nullptr;
    }
}

void MusicMagicAudioProcessor::loadSecondFile()
{
    secondSampler.clearSounds();
    //selecting file from directory
    juce::FileChooser chooser { "Please Load a Second Input File" };
    //if sound chosen
    if ( chooser.browseForFileToOpen() ) {
        //adding file
        secondInputTrack = chooser.getResult();
        secondInputTrackPath = secondInputTrack.getFullPathName();
        secondFormatReader = secondFormatManager.createReaderFor(secondInputTrack);
        //draw waveform
        secWaveForm.setSize(1, static_cast<int>(secondFormatReader->lengthInSamples));
        secondFormatReader->read(&secWaveForm, 0, static_cast<int>(secondFormatReader->lengthInSamples), 0, true, false);
        //adding sound to Sampler
        juce::BigInteger range;
        range.setRange(0, 128, true);
        secondSampler.addSound( new juce::SamplerSound ("Sample", *secondFormatReader, range, 60, 0.1, 0.1, 10.0));
    }
}

void MusicMagicAudioProcessor::clearSecondSampler()
{
    secondInputTrackPath = "null";
    secondSampler.clearSounds();
    if (secondFormatReader) {
        delete secondFormatReader;
        secondFormatReader = nullptr;
    }
}

void MusicMagicAudioProcessor::clearOutputSampler()
{
    pathToClip = "";
    outputSampler.clearSounds();
    if (outputFormatReader) {
        delete outputFormatReader;
        outputFormatReader = nullptr;
    }
}

// MANAGING REQUESTS ==============================================================================

bool MusicMagicAudioProcessor::process_request(juce::String prompt, juce::String action, juce::String random, juce::String time, juce::String side, juce::String firstStart, juce::String firstEnd, juce::String secStart, juce::String secEnd)
{
    //checking universal conditions
    if (firstStart > firstEnd || secStart > secEnd || action == "Unselected") return false;
    //checking action specific conditions
    if (action == "Generate") {
        if (prompt != "") return true;
    } else if (action == "Replace") {
        DBG("making it");
        if (prompt != "" && inputTrackPath != "null") return true;
    } else if (action == "Extend") {
        if (prompt != "" && inputTrackPath != "null" && side != "null") return true;
    } else if (action == "Fill") {
        if (prompt != "" && inputTrackPath != "null" && secondInputTrackPath != "null") return true;
    }
    //if valid request send it to the model
    return false;
}

bool MusicMagicAudioProcessor::send_request_to_model(juce::String prompt, juce::String action, juce::String random, juce::String time, juce::String side, juce::String firstStart, juce::String firstEnd, juce::String secStart, juce::String secEnd)
{
    //converting args to std::string
    std::string thePrompt = prompt.toStdString();
    std::string theAction = action.toStdString();
    std::string theRandom = random.toStdString();
    std::string thePath = inputTrackPath.toStdString();
    std::string secPath = secondInputTrackPath.toStdString();
    std::string theTime = time.toStdString();
    std::string theSide = side.toStdString();
    std::string fStart = firstStart.toStdString();
    std::string fEnd = firstEnd.toStdString();
    std::string sStart = secStart.toStdString();
    std::string sEnd = secEnd.toStdString();
    
    //### hardcoded filepath
    std::string condaPythonPath = "/Users/willsaliba/opt/anaconda3/envs/riffusion/bin/python3";
    std::string modelPath = "/Users/willsaliba/Documents/Topics/diffusion-music";
    std::string cmd = condaPythonPath + " " + modelPath +  "/plugin_requests.py \""
        + thePrompt + "\" \""
        + theAction + "\" \""
        + theRandom + "\" \""
        + thePath + "\" \""
        + secPath + "\" \""
        + theTime + "\" \""
        + theSide + "\" \""
        + fStart + "\" \""
        + fEnd + "\" \""
        + sStart + "\" \""
        + sEnd + "\"";
    
    DBG(cmd);
    //juce run command process
    juce::ChildProcess childProcess;
    if (! childProcess.start(juce::String(cmd)) ) {
       DBG("Failed to start the process.");
       return false;
    }
    //capture output from model
    juce::String result = childProcess.readAllProcessOutput();
    result = result.removeCharacters(" \n\r");
    DBG("---"); DBG(result); DBG("---");
    //if success
    if ( result.contains("SUCCESS")) {
        outputSampler.clearSounds();
        //processing path file ### hardcoded filepath
        pathToClip = "/Users/willsaliba/Documents/Topics/diffusion-music/outputs/generated_clip.mp3";
        outputTrack = juce::File(pathToClip);
        outputFormatReader = outputFormatManager.createReaderFor(outputTrack);
        //adding sound to Sampler
        juce::BigInteger range;
        range.setRange(0, 128, true);
        outputSampler.addSound( new juce::SamplerSound ("Sample", *outputFormatReader, range, 60, 0.1, 0.1, 10.0));
        return true;
    }
    return false;
}

// PREDEFINED ==============================================================================

#ifndef JucePlugin_PreferredChannelConfigurations
bool MusicMagicAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif
    return true;
  #endif
}
#endif

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{ return new MusicMagicAudioProcessor(); } //new plugin instance

juce::AudioProcessorEditor* MusicMagicAudioProcessor::createEditor()
{ return new MusicMagicAudioProcessorEditor (*this); } //new editor instance
