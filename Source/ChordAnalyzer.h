#pragma once

#include <JuceHeader.h>
#include <vector>
#include <string>
#include <map>

//======================//

// Chord information structure
struct ChordInfo
{
    std::string chordName;          // e.g., "Cmaj7"
    std::string romanNumeral;       // e.g., "I7"
    std::string function;           // e.g., "Tonic"
    std::vector<int> notes;         // MIDI note numbers
    int rootNote = -1;              // Root note (0-11)
    std::string quality;            // "major", "minor", "diminished", etc.
    std::string extensions;         // "7", "sus4", etc.
    bool isValid = false;

    ChordInfo() = default;

    void clear()
    {
        chordName.clear();
        romanNumeral.clear();
        function.clear();
        notes.clear();
        rootNote = -1;
        quality.clear();
        extensions.clear();
        isValid = false;
    }
};

// Suggestion category enum
enum class SuggestionCategory
{
    Basic,          // Blue - Common progressions
    Intermediate,   // Yellow - Secondary dominants, tritone substitutions
    Advanced        // Red - Chromatic mediants
};

// Structure to hold chord suggestion with category
struct ChordSuggestion
{
    std::string romanNumeral;
    SuggestionCategory category;
    std::string explanation;

    ChordSuggestion(const std::string& numeral, SuggestionCategory cat, const std::string& expl = "")
        : romanNumeral(numeral), category(cat), explanation(expl) {}
};

//======================//

class ChordAnalyzer
{
public:
    ChordAnalyzer();
    ~ChordAnalyzer() = default;

    // Main analysis method
    ChordInfo analyzeChord(const std::vector<int>& midiNotes, int keyRoot = 0, bool keyIsMajor = true);

    // Progression suggestions with categories
    std::vector<ChordSuggestion> suggestNextChordsWithCategories(const ChordInfo& currentChord, int keyRoot = 0, bool keyIsMajor = true);

    // Progression suggestions (LEGACY METHOD for backwards compatability; returns only Roman Numerals)
    std::vector<std::string> suggestNextChords(const ChordInfo& currentChord, int keyRoot = 0, bool keyIsMajor = true);

    // Extension suggestions - generates variations of a basic chord with common extensions
    std::vector<std::string> generateChordExtensions(const std::string& baseRomanNumeral,
                                                    const std::string& chordFunction,
                                                    int keyRoot,
                                                    bool keyIsMajor);

    // Get extension tooltip text for a basic chord suggestion
    std::string getExtensionTooltip(const std::string& baseRomanNumeral,
                                    int keyRoot,
                                    bool keyIsMajor);

    // Utility methods
    static std::string noteToString(int midiNote);
    static std::string getRomanNumeral(int chordRoot, int keyRoot, bool keyIsMajor, const std::string& quality);
    static std::string getChordFunction(const std::string& romanNumeral, int keyRoot, bool keyIsMajor);

    //Inherited Method for Tooltips
    static std::string getTheoryTooltip(const ChordInfo& chord, int keyRoot, bool keyIsMajor);

private:
    // Analysis helpers
    std::vector<int> normalizeToChroma(const std::vector<int>& midiNotes);
    std::string identifyChordQuality(const std::vector<int>& chromaNotes, int root);
    int getLowestNote(const std::vector<int>& midiNotes);
    //int findMostLikelyRoot(const std::vector<int>& chromaNotes); //Possibly bad implementation

    //-- Dec 15: Refactor of the suggestion features --//
    // Helper to determine if a chord is functionally dominant
    static bool isFunctionallyDominant(const std::string& baseRoman, const std::string& quality, bool keyIsMajor);

    // Helper to calculate scale degree from chord root
    int getScaleDegree(int chordRoot, int keyRoot);

    // Suggestion helpers
    std::vector<ChordSuggestion> getBasicSuggestions(const std::string& baseRoman, bool keyIsMajor);
    std::vector<ChordSuggestion> getIntermediateSuggestions(const std::string& baseRoman, const std::string& quality, int keyRoot, bool keyIsMajor);
    std::vector<ChordSuggestion> getAdvancedSuggestions(const std::string& baseRoman, int keyRoot, bool keyIsMajor);

    //-------------------------------------------------//
    
    // Chord pattern recognition
    bool matchesPattern(const std::vector<int>& chromaNotes, int root, const std::vector<int>& pattern);

    // Note name mapping
    static const std::vector<std::string> noteNames;
    static const std::map<std::string, std::string> romanNumeralFunctions;

    //======================================//
    // Chord patterns (intervals from root)
    //======================================//

    // Triads
    static const std::vector<int> majorTriad;
    static const std::vector<int> minorTriad;
    static const std::vector<int> diminishedTriad;
    static const std::vector<int> augmentedTriad;

    // Suspended chords
    static const std::vector<int> sus2;
    static const std::vector<int> sus4;

    // Seventh chords
    static const std::vector<int> majorSeventh;
    static const std::vector<int> minorSeventh;
    static const std::vector<int> dominantSeventh;
    static const std::vector<int> halfDiminishedSeventh;
    static const std::vector<int> diminishedSeventh;
    static const std::vector<int> augmentedSeventh;
    static const std::vector<int> minorMajorSeventh;

    // Ninths
    static const std::vector<int> majorNinth;
    static const std::vector<int> minorNinth;
    static const std::vector<int> dominantNinth;

    // Elevenths
    static const std::vector<int> majorEleventh;
    static const std::vector<int> minorEleventh;
    static const std::vector<int> dominantEleventh;

    // Thirteenths
    static const std::vector<int> majorThirteenth;
    static const std::vector<int> minorThirteenth;
    static const std::vector<int> dominantThirteenth;

    // Add chords
    static const std::vector<int> add9;
    static const std::vector<int> add6;
    static const std::vector<int> minorAdd9;
    static const std::vector<int> minorAdd6;

    // Extended dominants
    static const std::vector<int> dominant7flat9;
    static const std::vector<int> dominant7sharp9;
    static const std::vector<int> dominant7flat5;
    static const std::vector<int> dominant7sharp5;
    static const std::vector<int> dominant7flat5flat9;

    // Extended alterations
    static const std::vector<int> majorSevenSharp11;
    static const std::vector<int> majorSevenFlat5;
    static const std::vector<int> majorSevenSharp5;
    static const std::vector<int> minorSevenFlat5;
};