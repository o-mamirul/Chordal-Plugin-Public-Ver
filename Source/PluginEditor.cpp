#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
ChordalVSTAudioProcessorEditor::ChordalVSTAudioProcessorEditor(ChordalVSTAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Set editor size
    setSize(600, 480);

    // Setup custom LookAndFeel
    setLookAndFeel(&chordalLookAndFeel);

    // Setup all components
    setupComponents();

    // Start timer for regular updates (30fps)
    startTimer(33);
}

ChordalVSTAudioProcessorEditor::~ChordalVSTAudioProcessorEditor()
{
    stopTimer();

    setLookAndFeel(nullptr);
}

void ChordalVSTAudioProcessorEditor::setupComponents()
{
    // Roman Numeral Label (made larger and more prominent)
    addAndMakeVisible(romanNumeralLabel);
    romanNumeralLabel.setFont(chordalLookAndFeel.getSourceSerifFont(48.0f, juce::Font::bold));
    romanNumeralLabel.setJustificationType(juce::Justification::centred);
    romanNumeralLabel.setText("", juce::dontSendNotification);
    romanNumeralLabel.setColour(juce::Label::textColourId, chordalLookAndFeel.colorTeal);

    // Chord Name Label (under roman numeral)
    addAndMakeVisible(chordNameLabel);
    chordNameLabel.setFont(chordalLookAndFeel.getSourceSerifFont(20.0f, juce::Font::bold));
    chordNameLabel.setJustificationType(juce::Justification::centred);
    chordNameLabel.setText("No Chord", juce::dontSendNotification);

    // Notes Display Label (under chord name)
    addAndMakeVisible(notesDisplayLabel);
    notesDisplayLabel.setFont(chordalLookAndFeel.getMonaSansFont(12.0f, juce::Font::plain));
    notesDisplayLabel.setJustificationType(juce::Justification::centred);
    notesDisplayLabel.setText("", juce::dontSendNotification);
    notesDisplayLabel.setColour(juce::Label::textColourId, chordalLookAndFeel.colorTextMuted);

    // Function Label (under chord notes name)
    addAndMakeVisible(functionLabel);
    functionLabel.setFont(chordalLookAndFeel.getMonaSansFont(14.0f, juce::Font::italic));
    functionLabel.setJustificationType(juce::Justification::centred);
    functionLabel.setText("", juce::dontSendNotification);
    functionLabel.setColour(juce::Label::textColourId, chordalLookAndFeel.colorTextMuted);

    // Key Label
    addAndMakeVisible(keyLabel);
    keyLabel.setFont(chordalLookAndFeel.getMonaSansFont(14.0f, juce::Font::bold));
    keyLabel.setText("Key:", juce::dontSendNotification);
    keyLabel.setColour(juce::Label::textColourId, chordalLookAndFeel.colorTextPrimary);

    // Key Selector
    addAndMakeVisible(keySelector);
    keySelector.addItem("C", 1);
    keySelector.addItem("C#/Db", 2);
    keySelector.addItem("D", 3);
    keySelector.addItem("D#/Eb", 4);
    keySelector.addItem("E", 5);
    keySelector.addItem("F", 6);
    keySelector.addItem("F#/Gb", 7);
    keySelector.addItem("G", 8);
    keySelector.addItem("G#/Ab", 9);
    keySelector.addItem("A", 10);
    keySelector.addItem("A#/Bb", 11);
    keySelector.addItem("B", 12);
    keySelector.setSelectedId(1); // C
    keySelector.onChange = [this] { keySelectionChanged(); };
    keySelector.setTooltip("Select the root note of the musical key.");

    // Mode Selector
    addAndMakeVisible(modeSelector);
    modeSelector.addItem("Major", 1);
    modeSelector.addItem("Minor", 2);
    modeSelector.setSelectedId(1); // Major
    modeSelector.onChange = [this] { modeSelectionChanged(); };
    modeSelector.setTooltip("Select whether the key is Major or Minor.");

    // Setup web login button

    addAndMakeVisible(webLoginButton);
    webLoginButton.setButtonText("Login to Web");
    webLoginButton.onClick = [this] { webLoginButtonClicked(); };
    webLoginButton.setTooltip("Open web platform in your browser");
    

    // Recording Components
    addAndMakeVisible(recordButton);
    recordButton.setButtonText(juce::CharPointer_UTF8("\xf0\x9f\x94\xb4 REC"));
    recordButton.onClick = [this] { recordButtonClicked(); };
    recordButton.setTooltip("Start or Stop recording the chord progression.");

    addAndMakeVisible(clearButton);
    clearButton.setButtonText("Clear");
    clearButton.onClick = [this] { clearButtonClicked(); };
    clearButton.setColour(juce::TextButton::buttonColourId, chordalLookAndFeel.colorIntermediateOrange);
    clearButton.setColour(juce::TextButton::textColourOffId, chordalLookAndFeel.colorWhite);
    clearButton.setTooltip("Clear the currently recorded progression.");

    addAndMakeVisible(exportButton);
    exportButton.setButtonText("Export");
    exportButton.onClick = [this] { exportButtonClicked(); };
    exportButton.setTooltip("Export the recorded progression to a JSON file.");
    exportButton.setEnabled(false); // Initially disabled

    addAndMakeVisible(recordingIndicatorLabel);
    recordingIndicatorLabel.setText("", juce::dontSendNotification);
    recordingIndicatorLabel.setFont(chordalLookAndFeel.getMonaSansFont(12.0f, juce::Font::bold));
    recordingIndicatorLabel.setColour(juce::Label::textColourId, chordalLookAndFeel.colorAccentPink);
    recordingIndicatorLabel.setJustificationType(juce::Justification::centred);

    // Suggestions Label
    addAndMakeVisible(suggestionsLabel);
    suggestionsLabel.setFont(chordalLookAndFeel.getMonaSansFont(14.0f, juce::Font::bold));
    suggestionsLabel.setText("Suggested Next Chords:", juce::dontSendNotification);
    suggestionsLabel.setColour(juce::Label::textColourId, chordalLookAndFeel.colorTextPrimary);

    // Dec 15th: Filter Button
    addAndMakeVisible(filterButton);
    filterButton.onClick = [this] { showFilterMenu(); };

    updateFilterButton();

    //Copyright label
    addAndMakeVisible(copyrightLabel);
    copyrightLabel.setText(juce::CharPointer_UTF8("ver. Beta 1 | \xc2\xa9\ Chordal 2025 All Rights Reserved"), juce::dontSendNotification);
    copyrightLabel.setFont(chordalLookAndFeel.getMonaSansFont(9.0f, juce::Font::plain));
    copyrightLabel.setJustificationType(juce::Justification::centred);
    copyrightLabel.setColour(juce::Label::textColourId, chordalLookAndFeel.colorTextMuted);

}

void ChordalVSTAudioProcessorEditor::paint(juce::Graphics& g)
{

    juce::ColourGradient gradient(chordalLookAndFeel.colorWhite, 0, 0,
        chordalLookAndFeel.colorSoftGreen, 0, (float)getHeight(), false);
    g.setGradientFill(gradient);
    g.fillAll();

    // Draw main border
    g.setColour(chordalLookAndFeel.colorTealMedium);
    g.drawRect(getLocalBounds(), 2);

    //g.setColour(colorTealMedium);
    //g.drawRect(getLocalBounds(), 1);

    // Draw title bar background
    /*
    auto titleArea = getLocalBounds().removeFromTop(40);
    g.setColour(juce::Colour(0xff2a4a2a));
    g.fillRect(titleArea);

    // Draw title
    g.setColour(colorAquamarine);
    g.setFont(juce::Font(16.0f, juce::Font::bold));
    g.drawFittedText("ChordalVST - Chord Analysis", titleArea,
        juce::Justification::centred, 1);
    */

    // Draw section separators
    g.setColour(chordalLookAndFeel.colorTealLight);

    // Separator after controls
    g.drawHorizontalLine(65, 20, getWidth() - 20);

    // Separator before suggestions
    g.drawHorizontalLine(getHeight() - 160, 20, getWidth() - 20);

    // Draw animated Roman numeral with easing
    if (isAnimating || romanNumeralAlpha > 0.0f)
    {
        g.setColour(chordalLookAndFeel.colorTeal.withAlpha(romanNumeralAlpha));
        g.setFont(chordalLookAndFeel.getSourceSerifFont(48.0f, juce::Font::bold));

        auto romanArea = romanNumeralLabel.getBounds();
        auto targetX = romanArea.getX();
        auto animatedX = targetX + romanNumeralOffsetX;

        g.drawFittedText(romanNumeralLabel.getText(),
            animatedX, romanArea.getY(),
            romanArea.getWidth(), romanArea.getHeight(),
            juce::Justification::centred, 1);
    }
}

void ChordalVSTAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds.reduce(20, 20); // Overall padding

    // Title area
    //bounds.removeFromTop(40);
    //bounds.removeFromTop(10); // Space after title

    // Control section (Key selection + Recording controls)
    auto controlSection = bounds.removeFromTop(60);

    // Left side - Key controls
    auto keyArea = controlSection.removeFromLeft(250);
    auto keyRow1 = keyArea.removeFromTop(25);
    keyLabel.setBounds(keyRow1.removeFromLeft(40));
    keyRow1.removeFromLeft(5);
    keySelector.setBounds(keyRow1.removeFromLeft(80));
    keyRow1.removeFromLeft(10);
    modeSelector.setBounds(keyRow1.removeFromLeft(70));

    // Login-Token Button
    //webLoginButton.setBounds(keyRow1.removeFromLeft(80));


    // Right side - Recording controls
    controlSection.removeFromLeft(20); // Spacing
    auto recArea = controlSection;
    auto recRow1 = recArea.removeFromTop(25);
    auto recRow2 = recArea.removeFromTop(15);

    // First row of recording controls
    recordButton.setBounds(recRow1.removeFromLeft(60));
    //recRow1.removeFromLeft(10);
    //recordingIndicatorLabel.setBounds(recRow1.removeFromLeft(60));
    recRow1.removeFromLeft(10);
    clearButton.setBounds(recRow1.removeFromLeft(60));
    recRow1.removeFromLeft(10);
    exportButton.setBounds(recRow1.removeFromLeft(60));

    // Login-Token Button Alt Placement
    recRow1.removeFromLeft(10);
    webLoginButton.setBounds(recRow1.removeFromLeft(80));

    // Second row of recording controls
    recordingIndicatorLabel.setBounds(recRow2.removeFromLeft(60));
    //recRow2.removeFromLeft(10);
    //exportButton.setBounds(recRow2.removeFromLeft(60));

    bounds.removeFromTop(40); // Space after controls

    // Main chord display area (centered)
    auto chordDisplayArea = bounds.removeFromTop(200);
    auto chordCenterArea = chordDisplayArea.withSizeKeepingCentre(400, 200);

    // Roman numeral (largest, top)
    romanNumeralLabel.setBounds(chordCenterArea.removeFromTop(50));
    chordCenterArea.removeFromTop(5);

    // Chord name (medium, middle)
    chordNameLabel.setBounds(chordCenterArea.removeFromTop(35));
    chordCenterArea.removeFromTop(5);

    // Notes display (Dec 15: under chord name)
    notesDisplayLabel.setBounds(chordCenterArea.removeFromTop(25));
    chordCenterArea.removeFromTop(5);

    // Function (smallest, bottom)
    functionLabel.setBounds(chordCenterArea.removeFromTop(25));

    bounds.removeFromTop(20); // Space before suggestions

    // Suggestions area
    auto suggestionsHeaderArea = bounds.removeFromTop(20);
    suggestionsLabel.setBounds(suggestionsHeaderArea.removeFromLeft(150));
    filterButton.setBounds(suggestionsHeaderArea.removeFromLeft(30).reduced(2)); //New Filtering button..
    
    bounds.removeFromTop(10);

    // Suggestion buttons layout
    auto suggestionArea = bounds.removeFromTop(60);
    int numButtons = (int)suggestionButtons.size();
    if (numButtons > 0)
    {
        const int maxButtonsPerRow = 4;
        const int buttonHeight = 35;
        const int rowSpacing = 8;
        const int buttonSpacing = 8;

        // Calculate how many buttons in each row
        int row1Count = juce::jmin(numButtons, maxButtonsPerRow);
        int row2Count = juce::jmax(0, numButtons - maxButtonsPerRow);

        // Calculate button width based on the row with more buttons
        int maxRowCount = juce::jmax(row1Count, row2Count);
        int buttonWidth = (suggestionArea.getWidth() - (maxRowCount - 1) * buttonSpacing) / maxRowCount;
        buttonWidth = juce::jmax(80, juce::jmin(130, buttonWidth));

        // Position first row
        if (row1Count > 0)
        {
            int totalWidth1 = row1Count * buttonWidth + (row1Count - 1) * buttonSpacing;
            int startX1 = ((suggestionArea.getWidth() - totalWidth1) / 2) + 20;
            int row1Y = suggestionArea.getY();

            for (int i = 0; i < row1Count; ++i)
            {
                int x = startX1 + i * (buttonWidth + buttonSpacing);
                suggestionButtons[i]->setBounds(x, row1Y, buttonWidth, buttonHeight);
                suggestionButtons[i]->setVisible(true);
            }
        }

        // Position second row
        if (row2Count > 0)
        {
            int totalWidth2 = row2Count * buttonWidth + (row2Count - 1) * buttonSpacing;
            int startX2 = ((suggestionArea.getWidth() - totalWidth2) / 2) + 20;
            int row2Y = suggestionArea.getY() + buttonHeight + rowSpacing;

            for (int i = 0; i < row2Count; ++i)
            {
                int buttonIndex = maxButtonsPerRow + i;
                int x = startX2 + i * (buttonWidth + buttonSpacing);
                suggestionButtons[buttonIndex]->setBounds(x, row2Y, buttonWidth, buttonHeight);
                suggestionButtons[buttonIndex]->setVisible(true);
            }
        }

        // Hide any extra buttons (if there are more than 8)
        for (int i = maxButtonsPerRow * 2; i < numButtons; ++i)
        {
            suggestionButtons[i]->setVisible(false);
        }
    }

    // Copyright label at bottom centre
    auto bottomArea = getLocalBounds().removeFromBottom(25);
    copyrightLabel.setBounds(bottomArea.reduced(20, 5));

}

void ChordalVSTAudioProcessorEditor::timerCallback()
{
    updateChordDisplay();
    updateProgressionSuggestions();

    // Update recording UI state
    if (audioProcessor.isChordRecordingActive())
    {
        recordButton.setButtonText("STOP");
        recordButton.setColour(juce::TextButton::buttonColourId, chordalLookAndFeel.colorAccentPink);
        recordingIndicatorLabel.setText("REC", juce::dontSendNotification);
    }
    else
    {
        recordButton.setButtonText("REC");
        recordButton.setColour(juce::TextButton::buttonColourId, chordalLookAndFeel.colorTealMedium);
        recordingIndicatorLabel.setText("", juce::dontSendNotification);
    }

    // Enable/disable buttons based on state
    bool hasRecording = !audioProcessor.getFullRecordedProgression().empty();
    bool isRecording = audioProcessor.isChordRecordingActive();

    exportButton.setEnabled(hasRecording && !isRecording);
    clearButton.setEnabled(hasRecording && !isRecording);

    if (isAnimating)
    {
        // Ease in animation (cubic easing)
        romanNumeralAlpha = juce::jmin(1.0f, romanNumeralAlpha + 0.08f);

        // Cubic ease-out for position
        float progress = romanNumeralAlpha;
        float easedProgress = 1.0f - std::pow(1.0f - progress, 3.0f);
        romanNumeralOffsetX = 30.0f * (1.0f - easedProgress);

        if (romanNumeralAlpha >= 1.0f)
        {
            isAnimating = false;
            romanNumeralOffsetX = 0.0f;
        }

        repaint();
    }
}

void ChordalVSTAudioProcessorEditor::triggerRomanNumeralAnimation()
{
    romanNumeralAlpha = 0.0f;
    romanNumeralOffsetX = 30.0f;
    isAnimating = true;
}

void ChordalVSTAudioProcessorEditor::updateChordDisplay()
{
    const auto& currentChord = audioProcessor.getCurrentChord();
    int currentKey = audioProcessor.getCurrentKey();
    bool isKeyMajor = audioProcessor.isCurrentKeyMajor();

    if (currentChord.isValid)
    {

        // Check if Roman numeral changed to trigger animation
        if (lastDisplayedRomanNumeral != currentChord.romanNumeral)
        {
            triggerRomanNumeralAnimation();
            lastDisplayedRomanNumeral = currentChord.romanNumeral;
        }

        romanNumeralLabel.setText(juce::String(currentChord.romanNumeral), juce::dontSendNotification);
        chordNameLabel.setText(juce::String(currentChord.chordName), juce::dontSendNotification);
        functionLabel.setText(juce::String(currentChord.function), juce::dontSendNotification);

        //== Notes Display ==//

        juce::String notesText = "";
        const auto& notes = currentChord.notes;
        for (size_t i = 0; i < notes.size(); ++i)
        {
            int midiNote = notes[i];
            int chroma = midiNote % 12;
            juce::String noteName = juce::String(ChordAnalyzer::noteToString(midiNote));

            notesText += noteName + " (" + juce::String(midiNote) + ")";

            if (i < notes.size() - 1)
            {
                notesText += "  ";
            }
        }

        notesDisplayLabel.setText(notesText, juce::dontSendNotification);


        //== Update tooltips dynamically ==//

        chordNameLabel.setTooltip(chordTooltipsGenerator.getChordTooltip(currentChord, currentKey, isKeyMajor));
        romanNumeralLabel.setTooltip(chordTooltipsGenerator.getRomanNumeralTooltip(currentChord.romanNumeral, isKeyMajor));
        functionLabel.setTooltip(chordTooltipsGenerator.getFunctionTooltip(currentChord.function, currentChord.romanNumeral));
    }
    else
    {
        romanNumeralLabel.setText("", juce::dontSendNotification);
        chordNameLabel.setText("No Chord", juce::dontSendNotification);
        functionLabel.setText("Play some notes!", juce::dontSendNotification);

        chordNameLabel.setTooltip("Play 2 or more notes to see chord analysis");
        romanNumeralLabel.setTooltip("");
        functionLabel.setTooltip("");

        notesDisplayLabel.setText("Where the notes that build the chord would be displayed", juce::dontSendNotification);

        lastDisplayedRomanNumeral = "";
        romanNumeralAlpha = 0.0f;
        isAnimating = false;
    }
}

//--- Refactored Chord Suggestions ---//

void ChordalVSTAudioProcessorEditor::updateProgressionSuggestions()
{
    auto categorizedSuggestions = audioProcessor.getProgressionSuggestionsWithCategories();

    // Check if suggestions have actually changed
    bool suggestionsChanged = false;
    if (categorizedSuggestions.size() != lastCategorizedSuggestions.size())
    {
        suggestionsChanged = true;
    }
    else
    {
        for (size_t i = 0; i < categorizedSuggestions.size(); ++i)
        {
            if (categorizedSuggestions[i].romanNumeral != lastCategorizedSuggestions[i].romanNumeral ||
                categorizedSuggestions[i].category != lastCategorizedSuggestions[i].category)
            {
                suggestionsChanged = true;
                break;
            }
        }
    }

    if (!suggestionsChanged)
        return;

    lastCategorizedSuggestions = categorizedSuggestions;

    // Apply filter
    std::vector<ChordSuggestion> filteredSuggestions;
    for (const auto& sug : categorizedSuggestions)
    {
        bool include = false;
        switch (currentFilter)
        {
        case SuggestionFilter::All:
            include = true;
            break;
        case SuggestionFilter::BasicAndIntermediate:
            include = (sug.category != SuggestionCategory::Advanced);
            break;
        case SuggestionFilter::BasicOnly:
            include = (sug.category == SuggestionCategory::Basic);
            break;
        }

        if (include)
            filteredSuggestions.push_back(sug);
    }

    // Clear old buttons
    for (auto& button : suggestionButtons)
    {
        if (button)
            removeChildComponent(button.get());
    }
    suggestionButtons.clear();

    // Create buttons for filtered suggestions with colors
    for (const auto& suggestion : filteredSuggestions)
    {
        auto button = std::make_unique<juce::TextButton>(juce::String(suggestion.romanNumeral));

        // Set color based on category
        juce::Colour buttonColor;
        juce::Colour textColor = juce::Colours::white;

        switch (suggestion.category)
        {
        case SuggestionCategory::Basic:
            buttonColor = chordalLookAndFeel.colorBasicBlue;
            break;
        case SuggestionCategory::Intermediate:
            buttonColor = chordalLookAndFeel.colorIntermediateOrange;
            textColor = juce::Colours::black; // Better contrast
            break;
        case SuggestionCategory::Advanced:
            buttonColor = chordalLookAndFeel.colorAdvancedRed;
            break;
        }

        button->setColour(juce::TextButton::buttonColourId, buttonColor);
        button->setColour(juce::TextButton::textColourOffId, textColor);
        button->setColour(juce::TextButton::buttonOnColourId, buttonColor.brighter(0.2f));

        // Build tooltip with explanation
        juce::String categoryName;
        switch (suggestion.category)
        {
        case SuggestionCategory::Basic:
            categoryName = "Basic";
            break;
        case SuggestionCategory::Intermediate:
            categoryName = "Intermediate";
            break;
        case SuggestionCategory::Advanced:
            categoryName = "Advanced";
            break;
        }

        juce::String tooltip = juce::String(suggestion.romanNumeral) +
            " [" + categoryName + "]\n\n" +
            juce::String(suggestion.explanation);

        button->setTooltip(tooltip);

        button->onClick = [suggestion]() {
            DBG("Suggestion clicked: " + juce::String(suggestion.romanNumeral));
            };

        addAndMakeVisible(*button);
        suggestionButtons.push_back(std::move(button));
    }

    resized();
}

void ChordalVSTAudioProcessorEditor::showFilterMenu()
{
    juce::PopupMenu menu;

    menu.addItem(1, "Basic + Intermediate + Advanced",
        true, currentFilter == SuggestionFilter::All);
    menu.addItem(2, "Basic + Intermediate",
        true, currentFilter == SuggestionFilter::BasicAndIntermediate);
    menu.addItem(3, "Basic Only",
        true, currentFilter == SuggestionFilter::BasicOnly);

    menu.showMenuAsync(juce::PopupMenu::Options()
        .withTargetComponent(&filterButton)
        .withStandardItemHeight(25),
        [this](int result)
        {
            if (result > 0)
            {
                switch (result)
                {
                case 1: currentFilter = SuggestionFilter::All; break;
                case 2: currentFilter = SuggestionFilter::BasicAndIntermediate; break;
                case 3: currentFilter = SuggestionFilter::BasicOnly; break;
                }
                updateFilterButton();
                updateProgressionSuggestions();
            }
        });
}

void ChordalVSTAudioProcessorEditor::updateFilterButton() //Update filter button tooltip
{
    switch (currentFilter)
    {
    case SuggestionFilter::All:
        filterButton.setTooltip("Showing: All suggestions\nClick to filter");
        break;
    case SuggestionFilter::BasicAndIntermediate:
        filterButton.setTooltip("Showing: Basic + Intermediate\nClick to change");
        break;
    case SuggestionFilter::BasicOnly:
        filterButton.setTooltip("Showing: Basic only\nClick to change");
        break;
    }
}

//--------------------//
//--- Key Selection --//

void ChordalVSTAudioProcessorEditor::keySelectionChanged()
{
    int selectedKeyRoot = keySelector.getSelectedId() - 1; // Convert to 0-11
    bool isMajor = modeSelector.getSelectedId() == 1;
    audioProcessor.setKey(selectedKeyRoot, isMajor);
    updateChordDisplay(); // Update display immediately on key change
    updateProgressionSuggestions();
}

void ChordalVSTAudioProcessorEditor::modeSelectionChanged()
{
    int selectedKeyRoot = keySelector.getSelectedId() - 1; // Convert to 0-11
    bool isMajor = modeSelector.getSelectedId() == 1;
    audioProcessor.setKey(selectedKeyRoot, isMajor);
    updateChordDisplay(); // Update display immediately on mode change
    updateProgressionSuggestions();
}

//--- Recording Button --//

void ChordalVSTAudioProcessorEditor::recordButtonClicked()
{
    if (audioProcessor.isChordRecordingActive())
    {
        audioProcessor.stopChordRecording();
    }
    else
    {
        audioProcessor.startChordRecording();
    }
    // Timer will update button text/color
}

void ChordalVSTAudioProcessorEditor::clearButtonClicked()
{
    if (!audioProcessor.isChordRecordingActive()) // Safety check
    {
        // Optional: Add a confirmation dialog
        juce::AlertWindow::showOkCancelBox(juce::AlertWindow::WarningIcon,
            "Clear Progression",
            "Are you sure you want to clear the recorded progression?",
            "Clear",
            "Cancel",
            nullptr,
            juce::ModalCallbackFunction::create([this](int result)
                {
                    if (result == 1) // OK
                    {
                        audioProcessor.clearRecordedProgression();
                        // Timer will update exportButton enabled state
                    }
                }));
    }
}

void ChordalVSTAudioProcessorEditor::exportButtonClicked()
{
    if (!audioProcessor.isChordRecordingActive() && !audioProcessor.getFullRecordedProgression().empty())
    {
        // Show dialog to name the progression
        juce::AlertWindow nameWindow("Name Your Progression",
            "Enter a name for this chord progression:",
            juce::AlertWindow::NoIcon);

        nameWindow.addTextEditor("progressionName", "Recorded Progression", "Progression Name:");
        nameWindow.addButton("Export", 1, juce::KeyPress(juce::KeyPress::returnKey));
        nameWindow.addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));

        if (nameWindow.runModalLoop() == 1) // If user clicked Export
        {
            juce::String progressionName = nameWindow.getTextEditorContents("progressionName");

            if (progressionName.isEmpty())
                progressionName = "Recorded Progression";

            // Set the title in the processor
            audioProcessor.setProgressionTitle(progressionName);

            // Now export
            audioProcessor.exportRecordedProgressionToJSON();
        }
    }
}

//-- Login Token Button --//

void ChordalVSTAudioProcessorEditor::webLoginButtonClicked()
{
    showLoginDialog();
}

void ChordalVSTAudioProcessorEditor::showLoginDialog()
{
    auto* dialog = new VSTLoginDialog();

    dialog->onLoginAttempt = [this](juce::String email, juce::String password) {
        performWebLogin(email, password);
        };

    dialog->onCancel = [this]() {
        if (loginDialogWindow != nullptr)
            loginDialogWindow->exitModalState(0);
            loginDialogWindow->setVisible(false);
        };

    loginDialogWindow.reset(new juce::DialogWindow("Login to Chordal Web",
        chordalLookAndFeel.colorSoftGreen,
        false));

    loginDialogWindow->setContentOwned(dialog, true);
    loginDialogWindow->centreWithSize(400, 300);
    loginDialogWindow->setVisible(true);
    loginDialogWindow->enterModalState(true);

}

void ChordalVSTAudioProcessorEditor::performWebLogin(const juce::String& email,
    const juce::String& password)
{
    // Validate inputs
    if (email.isEmpty() || password.isEmpty())
    {
        if (auto* dialog = dynamic_cast<VSTLoginDialog*>(loginDialogWindow->getContentComponent()))
        {
            dialog->setStatus("Please enter both email and password");
        }
        return;
    }


    // Generate token and launch browser
    juce::String baseUrl = "https://localhost:44393";

    // NOTE: ONLY THROUGH HTTPS!
    bool success = TokenGenerator::launchBrowserAuthentication(email, password, baseUrl);

    if (success)
    {
        DBG("Browser launched successfully");
        if (loginDialogWindow != nullptr)
        {
            loginDialogWindow->exitModalState(1);
        }
    }
    else
    {
        if (auto* dialog = dynamic_cast<VSTLoginDialog*>(loginDialogWindow->getContentComponent()))
        {
            dialog->setStatus("Failed to launch browser");
        }
    }
}