#pragma once

#include <JuceHeader.h>
#include "ChordAnalyzer.h" 
#include "ChordRecorder.h"
#include <vector>
#include <string>
#include <map>

//==============================================================================
class ChordalVSTAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    ChordalVSTAudioProcessor();
    ~ChordalVSTAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // ChordalVST-specific methods
    void processMidiInput(const juce::MidiBuffer& midiMessages);
    void updateCurrentChord(const std::vector<int>& notes);
    const ChordInfo& getCurrentChord() const { return currentChord; }  // Return reference

    // NEW: Categorized suggestions with explanations and colors
    std::vector<ChordSuggestion> getProgressionSuggestionsWithCategories() const;

    // Legacy method for backward compatibility (still works.... I think?)
    std::vector<std::string> getProgressionSuggestions() const;

    // Get extension tooltip for a suggested chord
    std::string getExtensionTooltip(const std::string& romanNumeral) const;

    // Key management
    void setKey(int rootNote, bool isMajor);
    int getCurrentKey() const { return currentKey; }
    bool isCurrentKeyMajor() const { return isKeyMajor; }

    // Active notes access for UI
    const std::vector<int>& getActiveNotes() const { return activeNotes; }  // Return reference

    //For recording 
    void startChordRecording();
    void stopChordRecording();
    bool isChordRecordingActive() const;
    void clearRecordedProgression();
    const std::vector<RecordedChordEvent>& getFullRecordedProgression() const; //Will be passed for JSON rendering soon.

    void exportRecordedProgressionToJSON();

    // Progression naming
    void setProgressionTitle(const juce::String& title) { progressionTitle = title; }
    const juce::String& getProgressionTitle() const { return progressionTitle; }

private:
    // Core analysis components
    std::unique_ptr<ChordAnalyzer> chordAnalyzer;
    ChordInfo currentChord;
    std::vector<int> activeNotes;

    // Key context
    int currentKey = 0; // C major by default
    bool isKeyMajor = true;

    // MIDI processing
    std::map<int, bool> noteStates; // note -> isActive

    //Record instantiation
    std::unique_ptr<ChordRecorder> chordRecorderInstance;

    // Progression naming
    juce::String progressionTitle = "Recorded Progression";

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChordalVSTAudioProcessor)
};