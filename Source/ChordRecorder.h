#pragma once

#include <JuceHeader.h>
#include "ChordAnalyzer.h" 

struct RecordedChordEvent
{
    ChordInfo chord;
    double startTimeSeconds = 0.0; // Time from the start of the recording session
    double durationSeconds = 0.0;  // Duration of this chord event

    RecordedChordEvent(const ChordInfo& c, double startT) : chord(c), startTimeSeconds(startT) {}
};

class ChordRecorder
{
public:
    ChordRecorder();

    void startRecording();
    void stopRecording();

    // Called by PluginProcessor when a new stable chord is detected by the processor
    // or when notes change significantly enough to warrant a new event.
    void onProcessorChordUpdate(const ChordInfo& processorCurrentChord, bool processorChordIsValid);

    bool isRecordingActive() const;
    const std::vector<RecordedChordEvent>& getRecordedProgression() const;
    void clearProgression();

private:
    bool recordingActive = false;
    double currentSessionStartTimePoint = 0.0; // Time point (from juce::Time) when recording started

    std::vector<RecordedChordEvent> recordedProgression;
    ChordInfo lastRecordedChordState; // Stores the state of the last chord successfully added to the progression
};