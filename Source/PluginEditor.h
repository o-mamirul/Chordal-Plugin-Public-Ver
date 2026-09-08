#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ChordTooltips.h"
#include "TokenGenerator.h"
#include "VSTLoginDialog.h"
#include "ChordalLookAndFeel.h"

class VSTLoginDialog;

//==============================================================================
class ChordalVSTAudioProcessorEditor : public juce::AudioProcessorEditor,
    public juce::Timer
{
public:
    ChordalVSTAudioProcessorEditor(ChordalVSTAudioProcessor&);
    ~ChordalVSTAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    // Timer callback for updating display
    void timerCallback() override;

private:
    // Reference to processor
    ChordalVSTAudioProcessor& audioProcessor;

    juce::TooltipWindow tooltipWindow{ this, 700 };

    //--------- Custom LookAndFeel ------------//
    ChordalLookAndFeel chordalLookAndFeel;


    //--------- Components ------------//

    // UI Components
    juce::Label chordNameLabel;
    juce::Label romanNumeralLabel;
    juce::Label functionLabel;
    juce::Label keyLabel;
    juce::Label notesDisplayLabel; // Dec 15: Display notes with MIDI numbers

    // Logo and branding
    juce::Label copyrightLabel;

    // Key selection
    juce::ComboBox keySelector;
    juce::ComboBox modeSelector;


    // Progression suggestions
    juce::Label suggestionsLabel;
    std::vector<std::unique_ptr<juce::TextButton>> suggestionButtons;
    std::vector<ChordSuggestion> lastCategorizedSuggestions; // Dec 15th: New cache categorized suggestions

    std::vector<std::string> lastSuggestions; // (LEGACY) Cache to prevent unnecessary updates

    enum class SuggestionFilter
    {
        All,
        BasicAndIntermediate,
        BasicOnly
    };

    SuggestionFilter currentFilter = SuggestionFilter::All;
    juce::TextButton filterButton;

    //Recording
    juce::TextButton recordButton;
    juce::TextButton clearButton;
    juce::TextButton exportButton;
    juce::Label recordingIndicatorLabel;

    juce::String userEmail;
    juce::String userPasswordHash;

    //Unique Windows
    juce::TextButton webLoginButton;
    std::unique_ptr<juce::DialogWindow> loginDialogWindow;

    //--------- Animation State ------------//

    float romanNumeralAlpha = 0.0f;         // For fade-in animation
    float romanNumeralOffsetX = 30.0f;      // For slide-in animation
    bool isAnimating = false;
    std::string lastDisplayedRomanNumeral;  // Track changes

    //---------------------------//

    ChordTooltips chordTooltipsGenerator;

    // Layout
    void setupComponents();
    void updateChordDisplay();
    void updateProgressionSuggestions();
    void triggerRomanNumeralAnimation(); // Dec 15: Trigger animation

    //Filter methods
    void showFilterMenu();
    void updateFilterButton();

    // Other Callbacks
    void keySelectionChanged();
    void modeSelectionChanged();

    // Login Callbacks
    void webLoginButtonClicked();

    void showLoginDialog();
    void performWebLogin(const juce::String& email, const juce::String& password);
    

    // Record Callbacks
    void recordButtonClicked();
    void clearButtonClicked();
    void exportButtonClicked();



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChordalVSTAudioProcessorEditor)
};