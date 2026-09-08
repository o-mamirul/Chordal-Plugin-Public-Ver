#include "ChordAnalyzer.h"
#include "ChordTooltips.h" 
#include <algorithm>
#include <set>

const std::vector<std::string> ChordAnalyzer::noteNames = {
    "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
};

const std::map<std::string, std::string> ChordAnalyzer::romanNumeralFunctions = {
    {"I", "Tonic"}, {"i", "Tonic"},
    {"II", "Supertonic"}, {"ii", "Supertonic"},
    {"III", "Mediant"}, {"iii", "Mediant"},
    {"IV", "Subdominant"}, {"iv", "Subdominant"},
    {"V", "Dominant"}, {"v", "Dominant"},
    {"VI", "Submediant"}, {"vi", "Submediant"},
    {"VII", "Leading Tone"}, {"vii", "Leading Tone"}
};

// ========== CHORD PATTERNS (semitone intervals from root) ==========
// 
// Triads
const std::vector<int> ChordAnalyzer::majorTriad = { 0, 4, 7 };
const std::vector<int> ChordAnalyzer::minorTriad = { 0, 3, 7 };
const std::vector<int> ChordAnalyzer::diminishedTriad = { 0, 3, 6 };
const std::vector<int> ChordAnalyzer::augmentedTriad = { 0, 4, 8 };

// Suspended chords
const std::vector<int> ChordAnalyzer::sus2 = { 0, 2, 7 };
const std::vector<int> ChordAnalyzer::sus4 = { 0, 5, 7 };

// Seventh chords
const std::vector<int> ChordAnalyzer::majorSeventh = { 0, 4, 7, 11 };
const std::vector<int> ChordAnalyzer::minorSeventh = { 0, 3, 7, 10 };
const std::vector<int> ChordAnalyzer::dominantSeventh = { 0, 4, 7, 10 };
const std::vector<int> ChordAnalyzer::halfDiminishedSeventh = { 0, 3, 6, 10 };
const std::vector<int> ChordAnalyzer::diminishedSeventh = { 0, 3, 6, 9 };
const std::vector<int> ChordAnalyzer::augmentedSeventh = { 0, 4, 8, 10 };
const std::vector<int> ChordAnalyzer::minorMajorSeventh = { 0, 3, 7, 11 };

// Ninths
const std::vector<int> ChordAnalyzer::majorNinth = { 0, 2, 4, 7, 11 };
const std::vector<int> ChordAnalyzer::minorNinth = { 0, 2, 3, 7, 10 };
const std::vector<int> ChordAnalyzer::dominantNinth = { 0, 2, 4, 7, 10 };

// Elevenths
const std::vector<int> ChordAnalyzer::majorEleventh = { 0, 2, 4, 5, 7, 11 };
const std::vector<int> ChordAnalyzer::minorEleventh = { 0, 2, 3, 5, 7, 10 };
const std::vector<int> ChordAnalyzer::dominantEleventh = { 0, 2, 4, 5, 7, 10 };

// Thirteenths
const std::vector<int> ChordAnalyzer::majorThirteenth = { 0, 2, 4, 5, 7, 9, 11 };
const std::vector<int> ChordAnalyzer::minorThirteenth = { 0, 2, 3, 5, 7, 9, 10 };
const std::vector<int> ChordAnalyzer::dominantThirteenth = { 0, 2, 4, 5, 7, 9, 10 };

// Add chords
const std::vector<int> ChordAnalyzer::add9 = { 0, 2, 4, 7 };
const std::vector<int> ChordAnalyzer::add6 = { 0, 4, 7, 9 };
const std::vector<int> ChordAnalyzer::minorAdd9 = { 0, 2, 3, 7 };
const std::vector<int> ChordAnalyzer::minorAdd6 = { 0, 3, 7, 9 };

// Extended dominants
const std::vector<int> ChordAnalyzer::dominant7flat9 = { 0, 1, 4, 7, 10 };
const std::vector<int> ChordAnalyzer::dominant7sharp9 = { 0, 3, 4, 7, 10 };
const std::vector<int> ChordAnalyzer::dominant7flat5 = { 0, 4, 6, 10 };
const std::vector<int> ChordAnalyzer::dominant7sharp5 = { 0, 4, 8, 10 };
const std::vector<int> ChordAnalyzer::dominant7flat5flat9 = { 0, 1, 4, 6, 10 };

// Extended alterations
const std::vector<int> ChordAnalyzer::majorSevenSharp11 = { 0, 4, 6, 7, 11 };
const std::vector<int> ChordAnalyzer::majorSevenFlat5 = { 0, 4, 6, 11 };
const std::vector<int> ChordAnalyzer::majorSevenSharp5 = { 0, 4, 8, 11 };
const std::vector<int> ChordAnalyzer::minorSevenFlat5 = { 0, 3, 6, 10 };

ChordAnalyzer::ChordAnalyzer()
{
}

ChordInfo ChordAnalyzer::analyzeChord(const std::vector<int>& midiNotes, int keyRoot, bool keyIsMajor)
{
    ChordInfo chord;

    if (midiNotes.size() < 2)
    {
        return chord; // Invalid chord, return empty
    }

    // Store original notes
    chord.notes = midiNotes;

    // Convert to chroma (0-11)
    std::vector<int> chromaNotes = normalizeToChroma(midiNotes);

    // Remove duplicates and sort
    std::set<int> uniqueChroma(chromaNotes.begin(), chromaNotes.end());
    chromaNotes.assign(uniqueChroma.begin(), uniqueChroma.end());

    if (chromaNotes.size() < 2)
    {
        return chord;
    }

    // Find the most likely root
    //int root = findMostLikelyRoot(chromaNotes);
    int root = getLowestNote(midiNotes) % 12;
    chord.rootNote = root;

    // Identify chord quality and extensions
    chord.quality = identifyChordQuality(chromaNotes, root);

    // Build chord name
    chord.chordName = noteNames[root] + chord.quality;

    // Calculate Roman numeral
    chord.romanNumeral = getRomanNumeral(root, keyRoot, keyIsMajor, chord.quality);

    // Get harmonic function
    chord.function = getChordFunction(chord.romanNumeral, keyRoot, keyIsMajor);

    chord.isValid = true;
    return chord;
}

std::vector<int> ChordAnalyzer::normalizeToChroma(const std::vector<int>& midiNotes)
{
    std::vector<int> chroma;
    for (int note : midiNotes)
    {
        chroma.push_back(note % 12);
    }
    return chroma;
}

int ChordAnalyzer::getLowestNote(const std::vector<int>& midiNotes)
{
    if (midiNotes.empty()) return 0;

    return *std::min_element(midiNotes.begin(), midiNotes.end());
}

/*
int ChordAnalyzer::findMostLikelyRoot(const std::vector<int>& chromaNotes)
{
    // Simple heuristic: try each note as root and see which gives the best match
    int bestRoot = chromaNotes[0];
    int bestScore = 0;

    for (int potentialRoot : chromaNotes)
    {
        int score = 0;

        // Check how well this root fits common chord patterns
        if (matchesPattern(chromaNotes, potentialRoot, majorTriad)) score += 10;
        if (matchesPattern(chromaNotes, potentialRoot, minorTriad)) score += 10;
        if (matchesPattern(chromaNotes, potentialRoot, dominantSeventh)) score += 8;
        if (matchesPattern(chromaNotes, potentialRoot, majorSeventh)) score += 8;
        if (matchesPattern(chromaNotes, potentialRoot, minorSeventh)) score += 8;
        if (matchesPattern(chromaNotes, potentialRoot, diminishedTriad)) score += 6;
        if (matchesPattern(chromaNotes, potentialRoot, sus4)) score += 5;
        if (matchesPattern(chromaNotes, potentialRoot, sus2)) score += 5;

        if (score > bestScore)
        {
            bestScore = score;
            bestRoot = potentialRoot;
        }
    }

    return bestRoot;
}
*/

std::string ChordAnalyzer::identifyChordQuality(const std::vector<int>& chromaNotes, int root)
{
    // Check patterns in order of complexity (most complex first)

    // Thirteenths (7 notes)
    if (matchesPattern(chromaNotes, root, majorThirteenth))
        return "maj13";
    if (matchesPattern(chromaNotes, root, minorThirteenth))
        return "m13";
    if (matchesPattern(chromaNotes, root, dominantThirteenth))
        return "13";

    // Elevenths (6 notes)
    if (matchesPattern(chromaNotes, root, majorEleventh))
        return "maj11";
    if (matchesPattern(chromaNotes, root, minorEleventh))
        return "m11";
    if (matchesPattern(chromaNotes, root, dominantEleventh))
        return "11";

    // Ninths (5 notes)
    if (matchesPattern(chromaNotes, root, majorNinth))
        return "maj9";
    if (matchesPattern(chromaNotes, root, minorNinth))
        return "m9";
    if (matchesPattern(chromaNotes, root, dominantNinth))
        return "9";

    // Altered dominants (4-5 notes)
    if (matchesPattern(chromaNotes, root, dominant7flat5flat9))
        return "7♭5♭9";
    if (matchesPattern(chromaNotes, root, dominant7flat9))
        return "7♭9";
    if (matchesPattern(chromaNotes, root, dominant7sharp9))
        return "7♯9";
    if (matchesPattern(chromaNotes, root, dominant7sharp5))
        return "7♯5";

    // Extended alterations (4-5 notes)
    if (matchesPattern(chromaNotes, root, majorSevenSharp11))
        return "maj7♯11";
    if (matchesPattern(chromaNotes, root, majorSevenSharp5))
        return "maj7♯5";
    if (matchesPattern(chromaNotes, root, majorSevenFlat5))
        return "maj7♭5";

    // Seventh chords (4 notes)
    if (matchesPattern(chromaNotes, root, minorMajorSeventh))
        return "mMaj7";
    if (matchesPattern(chromaNotes, root, majorSeventh))
        return "maj7";
    if (matchesPattern(chromaNotes, root, minorSeventh))
        return "m7";
    if (matchesPattern(chromaNotes, root, dominantSeventh))
        return "7";
    if (matchesPattern(chromaNotes, root, halfDiminishedSeventh))
        return "m7♭5";
    if (matchesPattern(chromaNotes, root, diminishedSeventh))
        return "°7";
    if (matchesPattern(chromaNotes, root, augmentedSeventh))
        return "aug7";
    if (matchesPattern(chromaNotes, root, dominant7flat5))
        return "7♭5";

    // Add chords (4 notes)
    if (matchesPattern(chromaNotes, root, add9))
        return "add9";
    if (matchesPattern(chromaNotes, root, add6))
        return "6";
    if (matchesPattern(chromaNotes, root, minorAdd9))
        return "madd9";
    if (matchesPattern(chromaNotes, root, minorAdd6))
        return "m6";

    // Suspended chords (3 notes)
    if (matchesPattern(chromaNotes, root, sus4))
        return "sus4";
    if (matchesPattern(chromaNotes, root, sus2))
        return "sus2";

    // Triads (3 notes)
    if (matchesPattern(chromaNotes, root, augmentedTriad))
        return "aug";
    if (matchesPattern(chromaNotes, root, majorTriad))
        return "";  // Major triad (no suffix)
    if (matchesPattern(chromaNotes, root, minorTriad))
        return "m";
    if (matchesPattern(chromaNotes, root, diminishedTriad))
        return "°";

    return ""; // Default to major if no pattern matches
}

bool ChordAnalyzer::matchesPattern(const std::vector<int>& chromaNotes, int root, const std::vector<int>& pattern)
{
    for (int interval : pattern)
    {
        int expectedNote = (root + interval) % 12;
        if (std::find(chromaNotes.begin(), chromaNotes.end(), expectedNote) == chromaNotes.end())
        {
            return false;
        }
    }
    return true;
}


//-- Dec 15th: New methods for the dominant and suggestion refactor.. --//

int ChordAnalyzer::getScaleDegree(int chordRoot, int keyRoot)
{
    return (chordRoot - keyRoot + 12) % 12;
}

bool ChordAnalyzer::isFunctionallyDominant(const std::string& baseRoman, const std::string& quality, bool keyIsMajor)
{
    // Dominant 7th quality (major triad + minor 7th)
    bool hasDominant7Quality = (quality == "7" || quality == "9" || quality == "11" || quality == "13" ||
        quality.find("7♭") != std::string::npos || quality.find("7♯") != std::string::npos);

    // Check if it's on the V degree
    bool isOnVDegree = (baseRoman == "V" || baseRoman == "v");

    // Minor 7th chords on V in minor keys ARE dominant
    if (isOnVDegree && (quality == "m7" || quality == "m9" || quality == "m11"))
    {
        return true;
    }

    // Half-diminished on VII acts as dominant
    /*
    if ((baseRoman == "VII" || baseRoman == "vii") && quality == "m7♭5")
    {
        return true;
    }
    */

    // Secondary dominants: any chord with dominant 7th quality that's NOT a diatonic m7
    // (unless it's the V chord)
    if (hasDominant7Quality)
    {
        if (isOnVDegree)
        {
            return true;
        }

        // Other scale degrees with dominant 7th quality are secondary dominants
        // But NOT if they're naturally occurring m7 chords in the key
        // In major: ii7, iii7, vi7 are naturally minor 7ths
        // In minor: iv7, v7 are naturally minor 7ths

        if (keyIsMajor)
        {
            // In major, these are naturally m7, so "7" quality makes them secondary dominants
            if ((baseRoman == "II" || baseRoman == "III" || baseRoman == "VI") && quality != "m7")
            {
                return true; // Secondary dominant
            }
        }
        else // Minor key
        {
            // In minor, these are naturally m7, so "7" quality makes them secondary dominants  
            if ((baseRoman == "IV" || baseRoman == "V") && quality != "m7")
            {
                return true; // Secondary dominant
            }
        }

        // Any other degree with dominant 7 quality is a secondary dominant
        if (baseRoman != "ii" && baseRoman != "iii" && baseRoman != "vi" && baseRoman != "vii" &&// major key diatonic m7
            baseRoman != "iv" && baseRoman != "v" && baseRoman != "i" && baseRoman != "VI" &&// minor key diatonic m7
            !isOnVDegree) // Already handled above
        {
            return true;
        }
    }

    return false;
}

//----------------------------------------------------------------------//

std::string ChordAnalyzer::getRomanNumeral(int chordRoot, int keyRoot, bool keyIsMajor, const std::string& quality)
{
    int scaleDegree = (chordRoot - keyRoot + 12) % 12;

    std::string baseNumeral;

    if (keyIsMajor)
    {
        // Major key - get the scale degree
        switch (scaleDegree)
        {
        case 0:  baseNumeral = "I";     break;
        case 1:  baseNumeral = "♭II";   break;
        case 2:  baseNumeral = "II";    break;
        case 3:  baseNumeral = "♭III";  break;
        case 4:  baseNumeral = "III";   break;
        case 5:  baseNumeral = "IV";    break;
        case 6:  baseNumeral = "♯IV";   break;  // More common than ♭V
        case 7:  baseNumeral = "V";     break;
        case 8:  baseNumeral = "♭VI";   break;
        case 9:  baseNumeral = "VI";    break;
        case 10: baseNumeral = "♭VII";  break;
        case 11: baseNumeral = "VII";   break;
        default: baseNumeral = "?";     break;
        }
    }
    else
    {
        // Minor key - get the scale degree
        switch (scaleDegree)
        {
        case 0:  baseNumeral = "I";     break;  // Will be adjusted based on quality
        case 1:  baseNumeral = "♭II";   break;
        case 2:  baseNumeral = "II";    break;
        case 3:  baseNumeral = "III";   break;  // ♭III in major = III in minor
        case 4:  baseNumeral = "♯III";  break;
        case 5:  baseNumeral = "IV";    break;  // Will be adjusted based on quality
        case 6:  baseNumeral = "♯IV";   break;
        case 7:  baseNumeral = "V";     break;  // Will be adjusted based on quality
        case 8:  baseNumeral = "VI";    break;  // ♭VI in major = VI in minor
        case 9:  baseNumeral = "♯VI";   break;
        case 10: baseNumeral = "VII";   break; // ♭VII in major = VII in minor
        case 11: baseNumeral = "#VII";   break; // Leading tone
        default: baseNumeral = "?";     break;
        }
    }

    //=========================================================================//
    // NOW ADJUST CASE BASED ON ACTUAL CHORD QUALITY (not scale expectations)
    //=========================================================================//

    if (quality == "m" || quality == "m7" || quality == "m7♭5" || quality == "m9" ||
        quality == "m11" || quality == "m13" || quality == "madd9" || quality == "m6")
    {
        // Minor chords = lowercase
        std::transform(baseNumeral.begin(), baseNumeral.end(), baseNumeral.begin(), ::tolower);
    }
    else if (quality == "°" || quality == "°7")
    {
        // Diminished chords = lowercase
        std::transform(baseNumeral.begin(), baseNumeral.end(), baseNumeral.begin(), ::tolower);
    }
    else if (quality == "" || quality == "7" || quality == "maj7" || quality == "aug" ||
        quality == "aug7" || quality == "sus2" || quality == "sus4" || quality == "9" ||
        quality == "11" || quality == "13" || quality == "maj9" || quality == "maj11" ||
        quality == "maj13" || quality == "add9" || quality == "6" ||
        quality.find("7♭") != std::string::npos || quality.find("7♯") != std::string::npos)
    {
        // Major, dominant, augmented, and suspended chords = uppercase
        std::transform(baseNumeral.begin(), baseNumeral.end(), baseNumeral.begin(), ::toupper);
    }

    // Add quality indicators
    if (quality == "7")
        baseNumeral += "7";
    else if (quality == "m7")
        baseNumeral += "7";
    else if (quality == "maj7")
        baseNumeral += "maj7";
    else if (quality == "m7♭5")
        baseNumeral += "ø7";
    else if (quality == "°7")
        baseNumeral += "°7";
    else if (quality == "°")
        baseNumeral += "°";
    else if (quality == "aug")
        baseNumeral += "+";
    else if (quality == "aug7")
        baseNumeral += "+7";
    else if (quality == "sus4")
        baseNumeral += "sus4";
    else if (quality == "sus2")
        baseNumeral += "sus2";
    else if (quality == "9")
        baseNumeral += "9";
    else if (quality == "m9")
        baseNumeral += "9";
    else if (quality == "maj9")
        baseNumeral += "maj9";
    else if (quality == "11")
        baseNumeral += "11";
    else if (quality == "m11")
        baseNumeral += "11";
    else if (quality == "maj11")
        baseNumeral += "maj11";
    else if (quality == "13")
        baseNumeral += "13";
    else if (quality == "m13")
        baseNumeral += "13";
    else if (quality == "maj13")
        baseNumeral += "maj13";
    else if (quality == "add9")
        baseNumeral += "add9";
    else if (quality == "madd9")
        baseNumeral += "add9";
    else if (quality == "6")
        baseNumeral += "6";
    else if (quality == "m6")
        baseNumeral += "6";
    else if (quality == "mMaj7")
        baseNumeral += "mMaj7";
    else if (quality == "7♭9")
        baseNumeral += "7♭9";
    else if (quality == "7♯9")
        baseNumeral += "7♯9";
    else if (quality == "7♭5")
        baseNumeral += "7♭5";
    else if (quality == "7♯5")
        baseNumeral += "7♯5";
    else if (quality == "7♭5♭9")
        baseNumeral += "7♭5♭9";
    else if (quality == "maj7♯11")
        baseNumeral += "maj7♯11";
    else if (quality == "maj7♭5")
        baseNumeral += "maj7♭5";
    else if (quality == "maj7♯5")
        baseNumeral += "maj7♯5";

    return baseNumeral;
}

std::string ChordAnalyzer::getChordFunction(const std::string& romanNumeral, int keyRoot, bool keyIsMajor)
{
    // Extract base Roman numeral without extensions
    std::string base = romanNumeral;
    std::string fullChord = romanNumeral;

    // Remove common extensions to get base numeral
    size_t pos;
    if ((pos = base.find("7")) != std::string::npos) base = base.substr(0, pos);
    if ((pos = base.find("9")) != std::string::npos) base = base.substr(0, pos);
    if ((pos = base.find("11")) != std::string::npos) base = base.substr(0, pos);
    if ((pos = base.find("13")) != std::string::npos) base = base.substr(0, pos);
    if ((pos = base.find("sus")) != std::string::npos) base = base.substr(0, pos);
    if ((pos = base.find("°")) != std::string::npos) base = base.substr(0, pos);
    if ((pos = base.find("ø")) != std::string::npos) base = base.substr(0, pos);
    if ((pos = base.find("+")) != std::string::npos) base = base.substr(0, pos);
    if ((pos = base.find("add")) != std::string::npos) base = base.substr(0, pos);
    if ((pos = base.find("♭")) != std::string::npos) base = base.substr(0, pos);
    if ((pos = base.find("♯")) != std::string::npos) base = base.substr(0, pos);
    if ((pos = base.find("maj")) != std::string::npos) base = base.substr(0, pos);
    if ((pos = base.find("mMaj")) != std::string::npos) base = base.substr(0, pos);

    // Extract quality from full chord
    std::string quality = "";
    if ((pos = fullChord.find("m7")) != std::string::npos) quality = "m7";
    else if ((pos = fullChord.find("7")) != std::string::npos) quality = "7";

    // Check if functionally dominant using improved logic
    if (isFunctionallyDominant(base, quality, keyIsMajor))
    {
        return "Dominant";

    }


    auto it = romanNumeralFunctions.find(base);
    if (it != romanNumeralFunctions.end())
        return it->second;

    return "Borrowed/Passing";
}
//----------------------------------------------------------------------//
//          Dec 15th: New Refactored Suggestion System 
//     (note: The legacy methods is still here just underneath) 
//----------------------------------------------------------------------//

std::vector<ChordSuggestion> ChordAnalyzer::getBasicSuggestions(const std::string& baseRoman, bool keyIsMajor)
{
    std::vector<ChordSuggestion> suggestions;

    if (keyIsMajor)
    {
        // Major key progressions
        if (baseRoman == "I" || baseRoman == "i")
        {
            suggestions.push_back(ChordSuggestion("vi", SuggestionCategory::Basic, "Relative minor - smooth, emotional transition"));
            suggestions.push_back(ChordSuggestion("IV", SuggestionCategory::Basic, "Subdominant - classic departure from tonic"));
            suggestions.push_back(ChordSuggestion("V", SuggestionCategory::Basic, "Dominant - creates tension for resolution"));
            suggestions.push_back(ChordSuggestion("ii", SuggestionCategory::Basic, "Supertonic - prepares for V or vi"));
        }
        else if (baseRoman == "ii" || baseRoman == "II")
        {
            suggestions.push_back(ChordSuggestion("V", SuggestionCategory::Basic, "Classic ii-V motion"));
            suggestions.push_back(ChordSuggestion("I", SuggestionCategory::Basic, "Direct resolution to tonic"));
            suggestions.push_back(ChordSuggestion("vi", SuggestionCategory::Basic, "Deceptive motion"));
        }
        else if (baseRoman == "iii" || baseRoman == "III")
        {
            suggestions.push_back(ChordSuggestion("vi", SuggestionCategory::Basic, "Smooth voice leading"));
            suggestions.push_back(ChordSuggestion("IV", SuggestionCategory::Basic, "Ascending progression"));
            suggestions.push_back(ChordSuggestion("I", SuggestionCategory::Basic, "Return to tonic"));
        }
        else if (baseRoman == "IV" || baseRoman == "iv")
        {
            suggestions.push_back(ChordSuggestion("I", SuggestionCategory::Basic, "Plagal cadence - peaceful resolution"));
            suggestions.push_back(ChordSuggestion("V", SuggestionCategory::Basic, "Building tension"));
            suggestions.push_back(ChordSuggestion("vi", SuggestionCategory::Basic, "Deceptive progression"));
            suggestions.push_back(ChordSuggestion("ii", SuggestionCategory::Basic, "Circle of fifths"));
        }
        else if (baseRoman == "V" || baseRoman == "v")
        {
            suggestions.push_back(ChordSuggestion("I", SuggestionCategory::Basic, "Authentic cadence - strongest resolution"));
            suggestions.push_back(ChordSuggestion("vi", SuggestionCategory::Basic, "Deceptive cadence"));
            suggestions.push_back(ChordSuggestion("IV", SuggestionCategory::Basic, "Retrogression"));
        }
        else if (baseRoman == "vi" || baseRoman == "VI")
        {
            suggestions.push_back(ChordSuggestion("IV", SuggestionCategory::Basic, "Popular vi-IV motion"));
            suggestions.push_back(ChordSuggestion("ii", SuggestionCategory::Basic, "Preparing dominant"));
            suggestions.push_back(ChordSuggestion("V", SuggestionCategory::Basic, "Building to resolution"));
            suggestions.push_back(ChordSuggestion("I", SuggestionCategory::Basic, "Direct to tonic"));
        }
        else if (baseRoman == "vii" || baseRoman == "VII")
        {
            suggestions.push_back(ChordSuggestion("I", SuggestionCategory::Basic, "Leading tone resolution"));
            suggestions.push_back(ChordSuggestion("iii", SuggestionCategory::Basic, "Mediant connection"));
        }
    }
    else // Minor key
    {
        if (baseRoman == "i" || baseRoman == "I")
        {
            suggestions.push_back(ChordSuggestion("iv", SuggestionCategory::Basic, "Minor subdominant"));
            suggestions.push_back(ChordSuggestion("V", SuggestionCategory::Basic, "Dominant (often major in minor)"));
            suggestions.push_back(ChordSuggestion("VII", SuggestionCategory::Basic, "Subtonic - characteristic minor sound"));
            suggestions.push_back(ChordSuggestion("VI", SuggestionCategory::Basic, "Relative major"));
        }
        else if (baseRoman == "ii" || baseRoman == "II")
        {
            suggestions.push_back(ChordSuggestion("V", SuggestionCategory::Basic, "To dominant"));
            suggestions.push_back(ChordSuggestion("i", SuggestionCategory::Basic, "Return to tonic"));
        }
        else if (baseRoman == "III" || baseRoman == "iii")
        {
            suggestions.push_back(ChordSuggestion("VI", SuggestionCategory::Basic, "Relative major connection"));
            suggestions.push_back(ChordSuggestion("iv", SuggestionCategory::Basic, "To subdominant"));
            suggestions.push_back(ChordSuggestion("VII", SuggestionCategory::Basic, "Descending progression"));
        }
        else if (baseRoman == "iv" || baseRoman == "IV")
        {
            suggestions.push_back(ChordSuggestion("i", SuggestionCategory::Basic, "Minor plagal cadence"));
            suggestions.push_back(ChordSuggestion("V", SuggestionCategory::Basic, "Subdominant to dominant"));
            suggestions.push_back(ChordSuggestion("VII", SuggestionCategory::Basic, "Descending motion"));
        }
        else if (baseRoman == "V" || baseRoman == "v")
        {
            suggestions.push_back(ChordSuggestion("i", SuggestionCategory::Basic, "Authentic cadence in minor"));
            suggestions.push_back(ChordSuggestion("VI", SuggestionCategory::Basic, "Deceptive cadence"));
            suggestions.push_back(ChordSuggestion("iv", SuggestionCategory::Basic, "Retrogression"));
        }
        else if (baseRoman == "VI" || baseRoman == "vi")
        {
            suggestions.push_back(ChordSuggestion("VII", SuggestionCategory::Basic, "Andalusian progression"));
            suggestions.push_back(ChordSuggestion("III", SuggestionCategory::Basic, "Relative minor connection"));
            suggestions.push_back(ChordSuggestion("iv", SuggestionCategory::Basic, "To subdominant"));
            suggestions.push_back(ChordSuggestion("v", SuggestionCategory::Basic, "To dominant"));
        }
        else if (baseRoman == "VII" || baseRoman == "vii")
        {
            suggestions.push_back(ChordSuggestion("VI", SuggestionCategory::Basic, "Andalusian cadence continuation"));
            suggestions.push_back(ChordSuggestion("III", SuggestionCategory::Basic, "Descending thirds"));
            suggestions.push_back(ChordSuggestion("i", SuggestionCategory::Basic, "Resolution to tonic"));
        }
    }

    return suggestions;
}

std::vector<ChordSuggestion> ChordAnalyzer::getIntermediateSuggestions(const std::string& baseRoman,
    const std::string& quality,
    int keyRoot,
    bool keyIsMajor)
{
    std::vector<ChordSuggestion> suggestions;

    // Secondary dominants - any chord can have a dominant 7th to point to its target
    if (baseRoman == "I" || baseRoman == "i")
    {
        // Could go to secondary dominants
        suggestions.push_back(ChordSuggestion("III7", SuggestionCategory::Intermediate,
            "Secondary dominant to vi - III7→vi"));
        suggestions.push_back(ChordSuggestion("VI7", SuggestionCategory::Intermediate,
            "Secondary dominant to ii - VI7→ii"));
    }
    else if (baseRoman == "II" || baseRoman == "ii")
    {
        suggestions.push_back(ChordSuggestion("VI7", SuggestionCategory::Intermediate,
            "Secondary dominant of ii"));
    }
    else if (baseRoman == "III" || baseRoman == "iii")
    {
        // III7 is a secondary dominant to vi
        if (quality.find("7") != std::string::npos && quality.find("maj7") == std::string::npos)
        {
            suggestions.push_back(ChordSuggestion("vi", SuggestionCategory::Intermediate,
                "Resolution of secondary dominant III7→vi"));
        }
    }
    else if (baseRoman == "IV" || baseRoman == "iv")
    {
        suggestions.push_back(ChordSuggestion("I7", SuggestionCategory::Intermediate,
            "Secondary dominant to IV - I7→IV"));

        // Tritone substitution for V7
        if (keyIsMajor)
        {
            suggestions.push_back(ChordSuggestion("♭II7", SuggestionCategory::Intermediate,
                "Tritone sub for V7 - ♭II7→I"));
        }
    }
    else if (baseRoman == "V" || baseRoman == "v")
    {
        suggestions.push_back(ChordSuggestion("II7", SuggestionCategory::Intermediate,
            "Secondary dominant to V"));

        suggestions.push_back(ChordSuggestion("♯iv°7", SuggestionCategory::Intermediate,
            "Unique instance of a diminished chord subsituting as the tonic"));

    }
    else if (baseRoman == "VI" || baseRoman == "vi")
    {
       
            suggestions.push_back(ChordSuggestion("ii", SuggestionCategory::Intermediate,
                "Resolution of secondary dominant VI7 → ii"));

            suggestions.push_back(ChordSuggestion("vi", SuggestionCategory::Intermediate,
                "Resolution of secondary dominant VImaj7 → vi"));

    }

    // Tritone substitutions - for any dominant 7th
    if (quality == "7" || quality == "9" || quality == "7♭9" || quality == "7♯9")
    {
        // The tritone sub is always a ♭II of the target 
        // V is the most common subsitution however it is applicable for all dominants on all notes so....
        if (baseRoman == "V" || baseRoman == "v")
        {
            suggestions.push_back(ChordSuggestion("♭II7", SuggestionCategory::Intermediate,
                "Tritone substitution - replaces V7"));
        }

        //-----------------------------------------//
        //Big scope block for tritone subs!

        { 
            if (baseRoman == "I" || baseRoman == "i")
            {
                suggestions.push_back(ChordSuggestion("♯IV7", SuggestionCategory::Intermediate,
                    "Tritone substitution - replaces I7"));
            }

            if (baseRoman == "♯vii" || baseRoman == "♯VII")
            {
                suggestions.push_back(ChordSuggestion("IV7", SuggestionCategory::Intermediate,
                    "Tritone substitution - replaces ♯VII7"));
            }

            if (baseRoman == "vii" || baseRoman == "VII")
            {
                suggestions.push_back(ChordSuggestion("♭IV7", SuggestionCategory::Intermediate,
                    "Tritone substitution - replaces VII7"));
            }

            if (baseRoman == "♯vi" || baseRoman == "♯VI")
            {
                suggestions.push_back(ChordSuggestion("III7", SuggestionCategory::Intermediate,
                    "Tritone substitution - replaces ♯VI7"));
            }

            if (baseRoman == "vi" || baseRoman == "VI")
            {
                suggestions.push_back(ChordSuggestion("II7", SuggestionCategory::Intermediate,
                    "Tritone substitution - replaces VI7"));
            }

            if (baseRoman == "♯iv" || baseRoman == "♯IV")
            {
                suggestions.push_back(ChordSuggestion("I7", SuggestionCategory::Intermediate,
                    "Tritone substitution - replaces ♯IV7"));
            }

            if (baseRoman == "iv" || baseRoman == "IV")
            {
                suggestions.push_back(ChordSuggestion("♯VII7", SuggestionCategory::Intermediate,
                    "Tritone substitution - replaces IV7"));
            }

            if (baseRoman == "♯iii" || baseRoman == "♯III")
            {
                suggestions.push_back(ChordSuggestion("VII7", SuggestionCategory::Intermediate,
                    "Tritone substitution - replaces ♯III7"));
            }

            if (baseRoman == "iii" || baseRoman == "III")
            {
                suggestions.push_back(ChordSuggestion("♯VI7", SuggestionCategory::Intermediate,
                    "Tritone substitution - replaces III7"));
            }

            if (baseRoman == "ii" || baseRoman == "II")
            {
                suggestions.push_back(ChordSuggestion("VI7", SuggestionCategory::Intermediate,
                    "Tritone substitution - replaces II7"));
            }

            if (baseRoman == "♭ii" || baseRoman == "♭II")
            {
                suggestions.push_back(ChordSuggestion("V7", SuggestionCategory::Intermediate,
                    "Tritone substitution - replaces ♭II7"));
            }

        }
    }

    // bII going to I (tritone sub resolution)
    if (baseRoman == "♭II" || baseRoman == "♭ii")
    {
        suggestions.push_back(ChordSuggestion("I", SuggestionCategory::Intermediate,
            "Tritone sub resolution to tonic"));
    }

    return suggestions;
}


std::vector<ChordSuggestion> ChordAnalyzer::getAdvancedSuggestions(const std::string& baseRoman,
    int keyRoot,
    bool keyIsMajor)
{
    std::vector<ChordSuggestion> suggestions;

    // Chromatic mediants - chords a third apart with different quality
    if (baseRoman == "I" || baseRoman == "i")
    {
        if (keyIsMajor)
        {
            suggestions.push_back(ChordSuggestion("♭III", SuggestionCategory::Advanced,
                "Chromatic mediant - dramatic color change"));
            suggestions.push_back(ChordSuggestion("♭VI", SuggestionCategory::Advanced,
                "Chromatic mediant - dark coloring"));
        }
        else
        {
            suggestions.push_back(ChordSuggestion("♯iii", SuggestionCategory::Advanced,
                "Upper Chromatic mediant in minor, ♯III is also valid!"));
            suggestions.push_back(ChordSuggestion("♯vi", SuggestionCategory::Advanced,
                "Lower Chromatic mediant in minor, ♯VI is also plausible but rare."));
        }
    }
    else if (baseRoman == "IV" || baseRoman == "iv")
    {
        suggestions.push_back(ChordSuggestion("♭II", SuggestionCategory::Advanced,
            "Chromatic mediant relationship"));
    }
    else if (baseRoman == "V" || baseRoman == "v")
    {
        if (keyIsMajor)
        {
            suggestions.push_back(ChordSuggestion("♭III", SuggestionCategory::Advanced,
                "Upper Chromatic mediant from V in major, rare, experiment with it!"));
            suggestions.push_back(ChordSuggestion("VII", SuggestionCategory::Advanced,
                "Lower Chromatic mediant from V in major, rare, experiment with it!"));
        }
        else
        {
            suggestions.push_back(ChordSuggestion("♯VII", SuggestionCategory::Advanced,
                "Upper Chromatic mediant from V in minor, rare, experiment with it!"));
            suggestions.push_back(ChordSuggestion("III", SuggestionCategory::Advanced,
                "Lower Chromatic mediant from V in minor, not too rare as it is the relative major tonic, however, try using it as iii!"));
        }
    }

    return suggestions;
}

std::vector<ChordSuggestion> ChordAnalyzer::suggestNextChordsWithCategories(const ChordInfo& currentChord,
    int keyRoot,
    bool keyIsMajor)
{
    std::vector<ChordSuggestion> allSuggestions;

    if (!currentChord.isValid)
        return allSuggestions;

    // Extract base Roman numeral
    std::string baseRoman = currentChord.romanNumeral;
    size_t pos;
    if ((pos = baseRoman.find("7")) != std::string::npos) baseRoman = baseRoman.substr(0, pos);
    if ((pos = baseRoman.find("9")) != std::string::npos) baseRoman = baseRoman.substr(0, pos);
    if ((pos = baseRoman.find("11")) != std::string::npos) baseRoman = baseRoman.substr(0, pos);
    if ((pos = baseRoman.find("13")) != std::string::npos) baseRoman = baseRoman.substr(0, pos);
    if ((pos = baseRoman.find("sus")) != std::string::npos) baseRoman = baseRoman.substr(0, pos);
    if ((pos = baseRoman.find("°")) != std::string::npos) baseRoman = baseRoman.substr(0, pos);
    if ((pos = baseRoman.find("ø")) != std::string::npos) baseRoman = baseRoman.substr(0, pos);
    if ((pos = baseRoman.find("+")) != std::string::npos) baseRoman = baseRoman.substr(0, pos);
    if ((pos = baseRoman.find("add")) != std::string::npos) baseRoman = baseRoman.substr(0, pos);
    if ((pos = baseRoman.find("maj")) != std::string::npos) baseRoman = baseRoman.substr(0, pos);

    // Get suggestions from each category
    auto basicSugs = getBasicSuggestions(baseRoman, keyIsMajor);
    auto intermediateSugs = getIntermediateSuggestions(baseRoman, currentChord.quality, keyRoot, keyIsMajor);
    auto advancedSugs = getAdvancedSuggestions(baseRoman, keyRoot, keyIsMajor);

    // Combine all suggestions
    allSuggestions.insert(allSuggestions.end(), basicSugs.begin(), basicSugs.end());
    allSuggestions.insert(allSuggestions.end(), intermediateSugs.begin(), intermediateSugs.end());
    allSuggestions.insert(allSuggestions.end(), advancedSugs.begin(), advancedSugs.end());

    return allSuggestions;
}

//----------------------------------------------------------------------//

std::vector<std::string> ChordAnalyzer::generateChordExtensions(const std::string& baseRomanNumeral,
    const std::string& chordFunction,
    int keyRoot,
    bool keyIsMajor)
{
    std::vector<std::string> extensions;

    // Always include the basic form first
    extensions.push_back(baseRomanNumeral);

    // Determine if this is a major or minor chord based on case
    bool isLowerCase = !baseRomanNumeral.empty() && std::islower(baseRomanNumeral[0]);

    // Generate extensions based on harmonic function
    if (chordFunction == "Dominant")
    {
        // Dominant chords (V) commonly have 7ths and alterations
        extensions.push_back(baseRomanNumeral + "7");
        extensions.push_back(baseRomanNumeral + "9");
        extensions.push_back(baseRomanNumeral + "sus4");
        extensions.push_back(baseRomanNumeral + "7♭9");
        extensions.push_back(baseRomanNumeral + "7♯9");
    }
    else if (chordFunction == "Subdominant")
    {
        // Subdominant chords (IV) can have major 7ths or be dominant 7ths
        if (!isLowerCase)
        {
            extensions.push_back(baseRomanNumeral + "maj7");
            extensions.push_back(baseRomanNumeral + "7"); // Secondary dominant function
            extensions.push_back(baseRomanNumeral + "add9");
        }
        else
        {
            extensions.push_back(baseRomanNumeral + "7");
            extensions.push_back(baseRomanNumeral + "9");
        }
    }
    else if (chordFunction == "Supertonic")
    {
        // Supertonic chords (ii) commonly have 7ths
        extensions.push_back(baseRomanNumeral + "7");

        // In major keys, ii is minor, so m7 is common
        if (keyIsMajor && isLowerCase)
        {
            extensions.push_back(baseRomanNumeral + "9");
        }
    }
    else if (chordFunction == "Tonic")
    {
        // Tonic chords (I) can have major 7ths or added tones
        if (!isLowerCase)
        {
            extensions.push_back(baseRomanNumeral + "maj7");
            extensions.push_back(baseRomanNumeral + "add9");
            extensions.push_back(baseRomanNumeral + "6");
        }
        else
        {
            // Minor tonic
            extensions.push_back(baseRomanNumeral + "7");
            extensions.push_back(baseRomanNumeral + "add9");
            extensions.push_back(baseRomanNumeral + "6");
        }
    }
    else if (chordFunction == "Submediant")
    {
        // Submediant chords (vi) commonly have 7ths
        if (isLowerCase)
        {
            extensions.push_back(baseRomanNumeral + "7");
            extensions.push_back(baseRomanNumeral + "9");
        }
        else
        {
            extensions.push_back(baseRomanNumeral + "maj7");
        }
    }
    else if (chordFunction == "Mediant")
    {
        // Mediant chords (iii) can have 7ths
        if (isLowerCase)
        {
            extensions.push_back(baseRomanNumeral + "7");
        }
        else
        {
            extensions.push_back(baseRomanNumeral + "maj7");
        }
    }
    else if (chordFunction == "Leading Tone")
    {
        // Leading tone chords (vii°) are often diminished 7ths
        if (isLowerCase)
        {
            extensions.push_back(baseRomanNumeral + "°7");
            extensions.push_back(baseRomanNumeral + "ø7"); // Half-diminished
        }
    }

    return extensions;
}

std::vector<std::string> ChordAnalyzer::suggestNextChords(const ChordInfo& currentChord, int keyRoot, bool keyIsMajor)
{
    auto categorizedSuggestions = suggestNextChordsWithCategories(currentChord, keyRoot, keyIsMajor);
    std::vector<std::string> simpleSuggestions;

    for (const auto& sug : categorizedSuggestions)
    {
        simpleSuggestions.push_back(sug.romanNumeral);
    }

    return simpleSuggestions;
}
std::string ChordAnalyzer::getExtensionTooltip(const std::string& baseRomanNumeral,
    int keyRoot,
    bool keyIsMajor)
{
    // Get the function for this chord
    std::string function = getChordFunction(baseRomanNumeral, keyRoot, keyIsMajor);

    // Generate extensions
    std::vector<std::string> extensions = generateChordExtensions(baseRomanNumeral, function, keyRoot, keyIsMajor);

    // Build tooltip text
    std::string tooltip = "Extension variations:\n\n";

    for (size_t i = 0; i < extensions.size(); ++i)
    {
        tooltip += "• " + extensions[i];

        // Add brief description for each extension
        if (i == 0)
        {
            tooltip += " (basic form)";
        }
        else if (extensions[i].find("maj7") != std::string::npos)
        {
            tooltip += " (major 7th - dreamy, sophisticated)";
        }
        else if (extensions[i].find("7") != std::string::npos && extensions[i].find("♭9") != std::string::npos)
        {
            tooltip += " (altered dominant - tense, colorful)";
        }
        else if (extensions[i].find("7") != std::string::npos && extensions[i].find("m7") == std::string::npos)
        {
            tooltip += " (7th - adds tension)";
        }
        else if (extensions[i].find("m7") != std::string::npos)
        {
            tooltip += " (minor 7th - smooth, jazzy)";
        }
        else if (extensions[i].find("sus4") != std::string::npos)
        {
            tooltip += " (suspended - anticipation)";
        }
        else if (extensions[i].find("add9") != std::string::npos)
        {
            tooltip += " (added 9th - bright, open)";
        }
        else if (extensions[i].find("°7") != std::string::npos)
        {
            tooltip += " (diminished 7th - very unstable)";
        }
        else if (extensions[i].find("ø7") != std::string::npos)
        {
            tooltip += " (half-diminished - jazzy, leading)";
        }

        tooltip += "\n";
    }

    //tooltip += "\nHover to see options, click to select.";

    return tooltip;
}

std::string ChordAnalyzer::noteToString(int midiNote)
{
    return noteNames[midiNote % 12];
}

//==========================================================

std::string ChordAnalyzer::getTheoryTooltip(const ChordInfo& chord, int keyRoot, bool keyIsMajor)
{
    static ChordTooltips tooltips;  // Static instance for efficiency
    return tooltips.getChordTooltip(chord, keyRoot, keyIsMajor);
}