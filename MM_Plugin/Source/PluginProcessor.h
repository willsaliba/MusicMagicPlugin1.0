/* JUCE Plugin Processor --- BACK END --- */
#pragma once
#include <JuceHeader.h>

class MusicMagicAudioProcessor  : public juce::AudioProcessor
                                #if JucePlugin_Enable_ARA
                                 , public juce::AudioProcessorARAExtension
                                #endif
{
public:
    
    MusicMagicAudioProcessor();
    ~MusicMagicAudioProcessor() override;
    
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    
    //============================================================PREDEFINED & UNEDITED
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; };
    const juce::String getName() const override { return JucePlugin_Name;};
    bool acceptsMidi() const override { return true; };
    bool producesMidi() const override { return true; };
    bool isMidiEffect() const override { return false; };
    double getTailLengthSeconds() const override { return 0.0; };
    int getNumPrograms() override { return 1; };
    int getCurrentProgram() override { return 0; };
    void setCurrentProgram (int index) override {};
    const juce::String getProgramName (int index) override { return {}; };
    void changeProgramName (int index, const juce::String& newName) override {};
    void getStateInformation (juce::MemoryBlock& destData) override {};
    void setStateInformation (const void* data, int sizeInBytes) override {};
    void releaseResources() override {};
#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif
    //==============================================================================
    
    //playing the correct track
    juce::String trackPlaying {"in1"};
    void setTrackPlaying(juce::String track) { trackPlaying = track; };
    void sendMIDInotes();
    void sendNoSound();
    
    //first input track
    juce::File inputTrack;
    juce::String inputTrackPath {"null"};
    void loadInputFile();
    void clearInputSampler();
    int getNumSamplerSounds() { return firstSampler.getNumSounds(); };
    void loadInputFile(const juce::String& path);
    juce::AudioBuffer<float>& getFirstWaveForm() { return firstWaveForm;};
    
    //second input track
    juce::File secondInputTrack;
    juce::String secondInputTrackPath {"null"};
    void loadSecondFile();
    void clearSecondSampler();
    int getSecondNumSounds() { return secondSampler.getNumSounds(); };
    juce::AudioBuffer<float>& getSecondWaveForm() { return secWaveForm;};
    
    //output track
    juce::File outputTrack;
    juce::String pathToClip {"null"};
    juce::String getPath() { return pathToClip; };
    void clearOutputSampler();
    int getNumOutputSounds() { return outputSampler.getNumSounds(); };
    
    //making request
    bool process_request(juce::String prompt, juce::String action, juce::String random, juce::String time, juce::String side, juce::String firstStart, juce::String firstEnd, juce::String secStart, juce::String secEnd);
    bool send_request_to_model(juce::String prompt, juce::String action, juce::String random, juce::String time, juce::String side, juce::String firstStart, juce::String firstEnd, juce::String secStart, juce::String secEnd);
    
    //playhead
    std::atomic<bool>& isNoteBeingPlayed() { return isNotePlayed; };
    std::atomic<int>& getSampleCount() { return sampleCount; };
    
private:
    //Playing First Input Track
    juce::Synthesiser firstSampler;
    juce::AudioFormatManager firstFormatManager;
    juce::AudioFormatReader* firstFormatReader { nullptr };
    juce::AudioBuffer<float> firstWaveForm;
    
    //Playing Second Input Track
    juce::Synthesiser secondSampler;
    juce::AudioFormatManager secondFormatManager;
    juce::AudioFormatReader* secondFormatReader { nullptr };
    juce::AudioBuffer<float> secWaveForm;
    
    //Playing Output Track
    juce::Synthesiser outputSampler;
    juce::AudioFormatManager outputFormatManager;
    juce::AudioFormatReader* outputFormatReader { nullptr };
    
    //playhead
    std::atomic<bool> shouldUpdate {false};
    std::atomic<bool> isNotePlayed {false};
    std::atomic<int> sampleCount { 0 };
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MusicMagicAudioProcessor)
};
