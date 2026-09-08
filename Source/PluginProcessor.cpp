#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ChordalVSTAudioProcessor::ChordalVSTAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannels::stereo, true)
#endif
       // .withOutput("Output", juce::AudioChannels::stereo, true)
#endif
    )
#endif
{
    // Initialize chord analyzer
    chordAnalyzer = std::make_unique<ChordAnalyzer>();
    chordRecorderInstance = std::make_unique<ChordRecorder>();

    // Initialize current chord as empty
    currentChord.clear();

    // Set default key (C major)
    currentKey = 0;
    isKeyMajor = true;

    // Initialize note states
    for (int i = 0; i < 128; ++i)
    {
        noteStates[i] = false;
    }
}

ChordalVSTAudioProcessor::~ChordalVSTAudioProcessor()
{
}

//==============================================================================
const juce::String ChordalVSTAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ChordalVSTAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool ChordalVSTAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool ChordalVSTAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double ChordalVSTAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ChordalVSTAudioProcessor::getNumPrograms()
{
    return 1;
}

int ChordalVSTAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ChordalVSTAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String ChordalVSTAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void ChordalVSTAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void ChordalVSTAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(sampleRate, samplesPerBlock);

    // Initialize any resources needed for processing
    activeNotes.clear();
    for (auto& noteState : noteStates)
    {
        noteState.second = false;
    }

    currentChord.clear();
}

void ChordalVSTAudioProcessor::releaseResources()
{
    // Clean up resources
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ChordalVSTAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
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

void ChordalVSTAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Process MIDI input for chord analysis
    processMidiInput(midiMessages);

    // Audio passthrough
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        juce::ignoreUnused(channelData);
    }
}

void ChordalVSTAudioProcessor::processMidiInput(const juce::MidiBuffer& midiMessages)
{
    bool notesChanged = false;

    for (const auto metadata : midiMessages)
    {
        auto message = metadata.getMessage();

        if (message.isNoteOn())
        {
            int noteNumber = message.getNoteNumber();
            if (!noteStates[noteNumber]) // Only process if state changes
            {
                noteStates[noteNumber] = true;
                activeNotes.push_back(noteNumber);
                notesChanged = true;
                DBG("Note ON: " + juce::String(noteNumber) + " (" + juce::String(ChordAnalyzer::noteToString(noteNumber)) + ")");
            }
        }
        else if (message.isNoteOff())
        {
            int noteNumber = message.getNoteNumber();
            if (noteStates[noteNumber]) // Only process if state changes
            {
                noteStates[noteNumber] = false;
                activeNotes.erase(std::remove(activeNotes.begin(), activeNotes.end(), noteNumber), activeNotes.end());
                notesChanged = true;
                DBG("Note OFF: " + juce::String(noteNumber) + " (" + juce::String(ChordAnalyzer::noteToString(noteNumber)) + ")");
            }
        }
        else if (message.isAllNotesOff() || message.isAllSoundOff())
        {
            if (!activeNotes.empty()) // Only if there were active notes
            {
                activeNotes.clear();
                for (auto& noteStatePair : noteStates) // Reset all note states
                {
                    noteStatePair.second = false;
                }
                notesChanged = true;
                DBG("All notes off");
            }
        }
    }

    if (notesChanged)
    {
        // Sort activeNotes to ensure consistent order for analysis if needed by ChordAnalyzer
        // (though ChordAnalyzer currently normalizes to chroma and uses a set, so order might not matter there)????
        std::sort(activeNotes.begin(), activeNotes.end());
        updateCurrentChord(activeNotes); // This will now also interact with the recorder
    }
}


void ChordalVSTAudioProcessor::updateCurrentChord(const std::vector<int>& notes)
{
    ChordInfo newChordAnalysisResult; 
    bool newChordIsValid = false;

    if (notes.size() >= 2)
    {
        newChordAnalysisResult = chordAnalyzer->analyzeChord(notes, currentKey, isKeyMajor);
        newChordIsValid = newChordAnalysisResult.isValid; // Should be true if analyzeChord was successful
    }
    // else: newChordAnalysisResult remains default (isValid = false)

    // Check if the processor's live display chord has changed
    if (newChordIsValid)
    {
        if (newChordAnalysisResult.chordName != currentChord.chordName ||
            newChordAnalysisResult.romanNumeral != currentChord.romanNumeral ||
            !currentChord.isValid) // If previous was invalid, and now it's valid
        {
            currentChord = newChordAnalysisResult; // Update processor's live currentChord


            DBG("Processor: Chord updated to: " + juce::String(currentChord.chordName) +
                " (" + juce::String(currentChord.romanNumeral) + ") - " +
                juce::String(currentChord.function));

            if (auto* editor = getActiveEditor())
            {
                editor->repaint(); // Trigger UI update
            }
        }
    }

    else // notes.size() < 2 or analysis failed
    {
        if (currentChord.isValid) // If there *was* a valid live chord
        {
            currentChord.clear(); // Clear processor's live currentChord
            
            DBG("Processor: Live chord cleared");
            
            if (auto* editor = getActiveEditor())
            {
                editor->repaint(); // Trigger UI update
            }
        }
    }

    // Regardless of whether the *display* chord changed,
    // inform the recorder about the current state from the analyzer.
    if (chordRecorderInstance->isRecordingActive())
    {
        // Pass the latest analysis result (newChordAnalysisResult) and its validity
        chordRecorderInstance->onProcessorChordUpdate(newChordAnalysisResult, newChordIsValid);
    }
}

std::vector<ChordSuggestion> ChordalVSTAudioProcessor::getProgressionSuggestionsWithCategories() const
{
    if (currentChord.isValid)
    {
        return chordAnalyzer->suggestNextChordsWithCategories(currentChord, currentKey, isKeyMajor);
    }
    return {};
}

// EXISTING METHOD: Keep this for backward compatibility (no changes needed)
std::vector<std::string> ChordalVSTAudioProcessor::getProgressionSuggestions() const
{
    if (currentChord.isValid)
    {
        return chordAnalyzer->suggestNextChords(currentChord, currentKey, isKeyMajor);
    }
    return {};
}


std::string ChordalVSTAudioProcessor::getExtensionTooltip(const std::string& romanNumeral) const
{
    return chordAnalyzer->getExtensionTooltip(romanNumeral, currentKey, isKeyMajor);
}

void ChordalVSTAudioProcessor::setKey(int rootNote, bool isMajor)
{
    if (currentKey != rootNote || isKeyMajor != isMajor)
    {
        currentKey = rootNote;
        isKeyMajor = isMajor;

        if (!activeNotes.empty())
        {
            updateCurrentChord(activeNotes);
        }
        
        else // if no notes active, but key changes, ensure an invalid chord is passed to recorder if recording
        {
            if (chordRecorderInstance->isRecordingActive()) {
                ChordInfo emptyChord; // isValid will be false
                chordRecorderInstance->onProcessorChordUpdate(emptyChord, false);
            }
        }

        DBG("Key changed to: " + juce::String(ChordAnalyzer::noteToString(currentKey)) +
            (isKeyMajor ? " major" : " minor"));
    }
}

//==============================================================================


void ChordalVSTAudioProcessor::startChordRecording()
{
    chordRecorderInstance->startRecording();
    // Potentially update UI to show recording is active
    if (auto* editor = getActiveEditor()) editor->repaint(); 
}

void ChordalVSTAudioProcessor::stopChordRecording()
{
    chordRecorderInstance->stopRecording();
    // Potentially update UI
    if (auto* editor = getActiveEditor()) editor->repaint();


    const auto& prog = chordRecorderInstance->getRecordedProgression();
    DBG("--- Recorded Progression ---");
    for (const auto& event : prog)
    {
        DBG("Chord: " + juce::String(event.chord.chordName) +
            " | Roman: " + juce::String(event.chord.romanNumeral) +
            " | Start: " + juce::String(event.startTimeSeconds, 2) +
            "s | Duration: " + juce::String(event.durationSeconds, 2) + "s");
    }
    DBG("--- End of Progression ---");
}

bool ChordalVSTAudioProcessor::isChordRecordingActive() const
{
    return chordRecorderInstance->isRecordingActive();
}

void ChordalVSTAudioProcessor::clearRecordedProgression()
{
    chordRecorderInstance->clearProgression();
    // Potentially update UI
    if (auto* editor = getActiveEditor()) editor->repaint();
}

const std::vector<RecordedChordEvent>& ChordalVSTAudioProcessor::getFullRecordedProgression() const
{
    return chordRecorderInstance->getRecordedProgression();
}

void ChordalVSTAudioProcessor::exportRecordedProgressionToJSON()
{
    if (chordRecorderInstance->isRecordingActive() || chordRecorderInstance->getRecordedProgression().empty())
    {
        DBG("Export Aborted: Either recording is active or no progression recorded.");
        // Optionally show an alert to the user from the editor if called inappropriately
        return;
    }

    // Choose File
    juce::FileChooser fileChooser("Save Chord Progression",
        juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),
        "*.json",
        true); // useNativeDialogs = true

    if (fileChooser.browseForFileToSave(true)) // true for "warn if file already exists"
    {
        juce::File chosenFile = fileChooser.getResult();

        // 2. Construct JSON Object
        juce::var topLevelObject = new juce::DynamicObject();

        // --- Top-Level Metadata ---
        // You might want to add a way for the user to set a title in the UI later
        topLevelObject.getDynamicObject()->setProperty("progressionTitle", progressionTitle);
        topLevelObject.getDynamicObject()->setProperty("keyRoot", currentKey);
        topLevelObject.getDynamicObject()->setProperty("isKeyMajor", isKeyMajor);
        // topLevelObject.getDynamicObject()->setProperty("tempo", 120); // Placeholder
        topLevelObject.getDynamicObject()->setProperty("timestamp", juce::Time::getCurrentTime().toISO8601(true));
        topLevelObject.getDynamicObject()->setProperty("chordalVersion", JucePlugin_VersionString);


        // --- Chord Events Array ---
        juce::Array<juce::var> chordEventsArray;
        const auto& recordedProg = chordRecorderInstance->getRecordedProgression();

        for (const auto& recEvent : recordedProg)
        {
            juce::var chordEventVar = new juce::DynamicObject();
            auto* chordEventObj = chordEventVar.getDynamicObject();

            chordEventObj->setProperty("startTime", recEvent.startTimeSeconds);
            chordEventObj->setProperty("duration", recEvent.durationSeconds);

            juce::var chordInfoVar = new juce::DynamicObject();
            auto* chordInfoObj = chordInfoVar.getDynamicObject();

            chordInfoObj->setProperty("name", juce::String(recEvent.chord.chordName));

            // Store notes as a juce::Array<juce::var>
            juce::Array<juce::var> notesArray;
            for (int note : recEvent.chord.notes) {
                notesArray.add(note);
            }
            chordInfoObj->setProperty("notes", notesArray);

            chordInfoObj->setProperty("rootNote", recEvent.chord.rootNote); // Chroma (0-11)
            chordInfoObj->setProperty("quality", juce::String(recEvent.chord.quality));
            chordInfoObj->setProperty("extensions", juce::String(recEvent.chord.extensions)); // Ensure ChordInfo has this
            chordInfoObj->setProperty("romanNumeral", juce::String(recEvent.chord.romanNumeral));
            chordInfoObj->setProperty("function", juce::String(recEvent.chord.function));
            // chordInfoObj->setProperty("inversion", 0); // Placeholder 

            chordEventObj->setProperty("chordInfo", chordInfoVar);
            chordEventsArray.add(chordEventVar);
        }
        topLevelObject.getDynamicObject()->setProperty("chordEvents", chordEventsArray);

        // 3. Convert to String and Save
        juce::String jsonString = juce::JSON::toString(topLevelObject, true); // true for pretty print

        if (chosenFile.replaceWithText(jsonString))
        {
            DBG("Progression exported successfully to: " + chosenFile.getFullPathName());
           
            juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
                "Export Successful",
                "Progression saved to:\n" + chosenFile.getFullPathName());
        }
        else
        {
            DBG("Error saving progression to file.");
            juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
                "Export Failed",
                "Could not save the progression to the specified file.");
        }
    }
    else
    {
        DBG("JSON Export cancelled by user.");
    }
}

//==============================================================================

bool ChordalVSTAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* ChordalVSTAudioProcessor::createEditor()
{
    return new ChordalVSTAudioProcessorEditor(*this);
}

//==============================================================================
void ChordalVSTAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    std::unique_ptr<juce::XmlElement> xml(new juce::XmlElement("ChordalVSTSettings"));

    xml->setAttribute("currentKey", currentKey);
    xml->setAttribute("isKeyMajor", isKeyMajor);

    copyXmlToBinary(*xml, destData);
}

void ChordalVSTAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName("ChordalVSTSettings"))
        {
            currentKey = xmlState->getIntAttribute("currentKey", 0);
            isKeyMajor = xmlState->getBoolAttribute("isKeyMajor", true);
        }
    }
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChordalVSTAudioProcessor();
}