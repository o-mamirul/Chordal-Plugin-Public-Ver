#include "ChordRecorder.h"

ChordRecorder::ChordRecorder()
{
}

void ChordRecorder::startRecording()
{
    if (!recordingActive)
    {
        recordingActive = true;
        currentSessionStartTimePoint = juce::Time::getMillisecondCounter() / 1000.0;
        recordedProgression.clear();
        lastRecordedChordState.clear(); // Clear the state of the last recorded chord
        DBG("ChordRecorder: Recording STARTED at " + juce::String(currentSessionStartTimePoint));
    }
}

void ChordRecorder::stopRecording()
{
    if (recordingActive)
    {
        recordingActive = false;
        double sessionStopTimePoint = juce::Time::getMillisecondCounter() / 1000.0;

        if (!recordedProgression.empty())
        {
            // Finalize the duration of the last chord
            RecordedChordEvent& lastEvent = recordedProgression.back();
            lastEvent.durationSeconds = (sessionStopTimePoint - currentSessionStartTimePoint) - lastEvent.startTimeSeconds;
            DBG("ChordRecorder: Finalized last chord (" + juce::String(lastEvent.chord.chordName) + ") duration: " + juce::String(lastEvent.durationSeconds));
        }
        DBG("ChordRecorder: Recording STOPPED at " + juce::String(sessionStopTimePoint) + ". Total events: " + juce::String(recordedProgression.size()));
    }
}

void ChordRecorder::onProcessorChordUpdate(const ChordInfo& processorCurrentChord, bool processorChordIsValid)
{
    if (!recordingActive)
        return;

    double currentTimeRelative = (juce::Time::getMillisecondCounter() / 1000.0) - currentSessionStartTimePoint;

    if (processorChordIsValid)
    {
        // Only add a new event if the chord has actually changed from the last *recorded* one
        if (processorCurrentChord.chordName != lastRecordedChordState.chordName ||
            processorCurrentChord.romanNumeral != lastRecordedChordState.romanNumeral || // Or other significant properties
            recordedProgression.empty()) // Always add if it's the first chord
        {
            if (!recordedProgression.empty())
            {
                // Finalize duration of the previous chord in the recording
                RecordedChordEvent& previousEvent = recordedProgression.back();
                previousEvent.durationSeconds = currentTimeRelative - previousEvent.startTimeSeconds;
                DBG("ChordRecorder: Updated previous chord (" + juce::String(previousEvent.chord.chordName) + ") duration: " + juce::String(previousEvent.durationSeconds));

                // Prevent negative or zero duration if updates are too fast
                if (previousEvent.durationSeconds <= 0.0) {
                    DBG("ChordRecorder: Warning - previous event duration was zero or negative. Correcting to a small value.");
                    previousEvent.durationSeconds = 0.01; // A tiny duration
                }
            }

            // Add the new chord event
            recordedProgression.emplace_back(processorCurrentChord, currentTimeRelative);
            lastRecordedChordState = processorCurrentChord; // Update the last recorded state
            DBG("ChordRecorder: New chord event ADDED: " + juce::String(processorCurrentChord.chordName) + " at " + juce::String(currentTimeRelative));
        }
    }
    else // processorChordIsValid is false (e.g., no notes or not enough for a chord)
    {
        // If there was a valid chord being held and now it's gone,
        // and it was the last one in our recording, we might want to finalize its duration.
        // However, stopRecording() will handle the very last chord's duration robustly.
        // If lastRecordedChordState was valid, it means a chord was sounding, and now it's not.
        // We don't add an "empty" event, but the *next* valid chord will correctly set the duration of lastRecordedChordState.
        // Or, stopRecording will.
        if (lastRecordedChordState.isValid) {
            // The previously sounding chord has ended. Its duration will be set by the next chord, or stopRecording.
            DBG("ChordRecorder: Notes cleared, previous chord " + juce::String(lastRecordedChordState.chordName) + " effectively ended.");
            // DON'T clear lastRecordedChordState here, because if recording stops without new notes,
            // stopRecording needs to know what the last chord was.
        }
    }
}

bool ChordRecorder::isRecordingActive() const
{
    return recordingActive;
}

const std::vector<RecordedChordEvent>& ChordRecorder::getRecordedProgression() const
{
    return recordedProgression;
}

void ChordRecorder::clearProgression()
{
    if (!recordingActive) // Only allow clearing if not actively recording
    {
        recordedProgression.clear();
        lastRecordedChordState.clear();
        DBG("ChordRecorder: Progression CLEARED.");
    }
}