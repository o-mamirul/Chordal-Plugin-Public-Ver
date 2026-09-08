#include "ChordTooltips.h"


ChordTooltips::ChordTooltips()
{
    initializeChordQualityExplanations();
    initializeRomanNumeralExplanations();
    initializeFunctionExplanations();
    initializeProgressionExplanations();
}

void ChordTooltips::initializeChordQualityExplanations()
{
    chordQualityExplanations[""] = "A major triad containing the root, major third, and perfect fifth. This creates a bright, stable sound.";

    chordQualityExplanations["m"] = "A minor triad containing the root, minor third, and perfect fifth. The minor third gives it a darker, more melancholic character.";

    chordQualityExplanations["7"] = "A dominant seventh chord with root, major third, perfect fifth, and minor seventh. Creates tension that wants to resolve, commonly used in blues and jazz.";

    chordQualityExplanations["maj7"] = "A major seventh chord with root, major third, perfect fifth, and major seventh. Creates a dreamy, sophisticated sound often used in jazz and contemporary music.";

    chordQualityExplanations["m7"] = "A minor seventh chord with root, minor third, perfect fifth, and minor seventh. Softer than a dominant 7th, often used in soul, jazz, and R&B.";

    chordQualityExplanations["°"] = "A diminished triad with root, minor third, and diminished fifth. Creates a tense, unstable sound that typically resolves downward.";

    chordQualityExplanations["°7"] = "A diminished seventh chord - fully diminished with four notes each a minor third apart. Extremely unstable and can resolve to many different chords.";

    chordQualityExplanations["m7♭5"] = "A half-diminished seventh chord with root, minor third, diminished fifth, and minor seventh. Less tense than fully diminished, commonly used in jazz.";

    chordQualityExplanations["aug"] = "An augmented triad with root, major third, and augmented fifth. Creates an unstable, 'floating' sound due to its symmetrical structure.";

    chordQualityExplanations["aug7"] = "An augmented seventh chord combining the tension of both augmented triad and dominant seventh. Highly unstable and colorful.";

    chordQualityExplanations["sus2"] = "A suspended second chord replacing the third with the second. Creates an open, unresolved sound that wants to move to a major or minor chord.";

    chordQualityExplanations["sus4"] = "A suspended fourth chord replacing the third with the fourth. Common in rock and pop, creates anticipation for resolution to major or minor.";
}

void ChordTooltips::initializeRomanNumeralExplanations()
{
    // Major key explanations
    romanNumeralExplanations["I"] = "The tonic chord - the home base of the key. Provides stability and resolution.";
    romanNumeralExplanations["ii"] = "The supertonic chord - commonly leads to V or vi. Often used in ii-V-I progressions.";
    romanNumeralExplanations["iii"] = "The mediant chord - shares notes with both I and vi. Less common but useful for smooth voice leading.";
    romanNumeralExplanations["IV"] = "The subdominant chord - provides a sense of departure from home. Very common in popular music.";
    romanNumeralExplanations["V"] = "The dominant chord - creates strong tension that resolves to I. The most important chord for establishing key.";
    romanNumeralExplanations["vi"] = "The submediant chord - the relative minor. Often used as a deceptive resolution or to start progressions.";
    romanNumeralExplanations["vii°"] = "The leading tone chord - built on the 7th scale degree. Strongly pulls toward the tonic.";

    // Add variations with 7ths
    romanNumeralExplanations["I7"] = "The tonic with added 7th - less common but adds color. Can function as a dominant of IV.";
    romanNumeralExplanations["ii7"] = "Very common in jazz - the ii7 in ii7-V7-I progressions. Smooth voice leading to V7.";
    romanNumeralExplanations["V7"] = "The dominant seventh - the most important tension chord. Strongly resolves to I.";

    // Minor key specific
    romanNumeralExplanations["i"] = "The tonic minor chord - home base in minor keys. More melancholic than major tonic.";
    romanNumeralExplanations["III"] = "The relative major - very important in minor keys. Often feels like a bright departure.";
    romanNumeralExplanations["VI"] = "The flat six - creates a distinctive minor key sound. Common in progressions like i-VI-VII.";
    romanNumeralExplanations["VII"] = "The flat seven - naturally major in minor keys. Often used in rock and modal progressions.";
}

void ChordTooltips::initializeFunctionExplanations()
{
    functionExplanations["Tonic"] = "The tonic function represents home, stability, and rest. These chords provide resolution and establish the key center.";

    functionExplanations["Subdominant"] = "The subdominant function creates a sense of departure from home without strong tension. It moves away from tonic but doesn't demand immediate resolution.";

    functionExplanations["Dominant"] = "The dominant function creates tension that strongly wants to resolve back to tonic. This is the driving force of functional harmony.";

    functionExplanations["Supertonic"] = "The supertonic function often acts as a stepping stone to dominant chords, commonly found in ii-V-I progressions.";

    functionExplanations["Mediant"] = "The mediant function shares notes with tonic, creating smooth connections. Less common but useful for voice leading.";

    functionExplanations["Submediant"] = "The submediant function can act as a substitute for tonic or lead to subdominant chords. Important in deceptive cadences.";

    functionExplanations["Leading Tone"] = "The leading tone function creates strong pull toward tonic due to the half-step resolution of the leading tone.";
}

void ChordTooltips::initializeProgressionExplanations()
{
    // Common progressions
    progressionExplanations["I-vi"] = "A gentle move from tonic to its relative minor. Creates emotional depth while maintaining stability.";
    progressionExplanations["I-IV"] = "The classic tonic to subdominant motion. Foundation of countless songs across all genres.";
    progressionExplanations["I-V"] = "From home to dominant - creates expectation for return. Often part of longer progressions.";
    progressionExplanations["vi-IV"] = "From minor to major subdominant. Popular in contemporary music for its emotional lift.";
    progressionExplanations["ii-V"] = "The classic preparation-to-dominant motion. Essential in jazz and common in all styles.";
    progressionExplanations["V-I"] = "The authentic cadence - the strongest resolution in tonal music. Defines the key and provides closure.";
    progressionExplanations["IV-V"] = "Subdominant to dominant - builds tension and momentum toward resolution.";
    progressionExplanations["vi-ii"] = "A smooth connection often used to approach ii-V-I progressions.";
}

//=============================================================================//
//=============================================================================//

std::string ChordTooltips::getChordTooltip(const ChordInfo& chord, int keyRoot, bool keyIsMajor)
{
    if (!chord.isValid)
        return "No chord detected. Play at least 2 notes simultaneously to analyze harmony.";

    std::string title = "Chord: " + chord.chordName;
    std::string explanation = explainChordQuality(chord.quality);

    // Add key context
    std::string keyContext = "\n\nIn " + getKeyName(keyRoot, keyIsMajor) + ": " +
        chord.romanNumeral + " (" + chord.function + ")";

    // Add chord tones
    auto tones = getChordTones(chord.chordName);
    std::string chordTones = "\n\nChord tones: ";
    for (size_t i = 0; i < tones.size(); ++i)
    {
        chordTones += tones[i];
        if (i < tones.size() - 1) chordTones += ", ";
    }

    return formatTooltip(title, explanation + keyContext + chordTones);
}

std::string ChordTooltips::getRomanNumeralTooltip(const std::string& romanNumeral, bool keyIsMajor)
{
    std::string baseRoman = romanNumeral;

    // Remove extensions for lookup
    size_t pos;
    if ((pos = baseRoman.find("7")) != std::string::npos) baseRoman = baseRoman.substr(0, pos);
    if ((pos = baseRoman.find("sus")) != std::string::npos) baseRoman = baseRoman.substr(0, pos);
    if ((pos = baseRoman.find("°")) != std::string::npos) baseRoman = baseRoman.substr(0, pos);
    if ((pos = baseRoman.find("ø")) != std::string::npos) baseRoman = baseRoman.substr(0, pos);
    if ((pos = baseRoman.find("+")) != std::string::npos) baseRoman = baseRoman.substr(0, pos);

    auto it = romanNumeralExplanations.find(baseRoman);
    if (it != romanNumeralExplanations.end())
    {
        return formatTooltip("Roman Numeral: " + romanNumeral, it->second);
    }

    return formatTooltip("Roman Numeral: " + romanNumeral, "This chord degree in the current key context.");
}

std::string ChordTooltips::getFunctionTooltip(const std::string& function, const std::string& romanNumeral)
{
    auto it = functionExplanations.find(function);
    if (it != functionExplanations.end())
    {
        return formatTooltip("Harmonic Function: " + function,
            it->second + "\n\nThis " + romanNumeral + " chord serves this function in the current key.");
    }

    return formatTooltip("Harmonic Function: " + function, "The role this chord plays in the harmonic progression.");
}

std::string ChordTooltips::getProgressionTooltip(const std::vector<std::string>& suggestions, const ChordInfo& currentChord)
{
    if (suggestions.empty())
        return "No progression suggestions available.";

    std::string tooltip = "Suggested next chords based on " + currentChord.romanNumeral + ":\n\n";

    for (const auto& suggestion : suggestions)
    {
        tooltip += "• " + suggestion + " - ";

        // Add brief explanation for each suggestion
        std::string progressionKey = currentChord.romanNumeral + "-" + suggestion;
        auto it = progressionExplanations.find(progressionKey);
        if (it != progressionExplanations.end())
        {
            tooltip += it->second;
        }
        else
        {
            tooltip += "Common progression choice";
        }
        tooltip += "\n";
    }

    return tooltip;
}

//=============================================================================//
//=============================================================================//

std::string ChordTooltips::explainChordQuality(const std::string& quality)
{
    auto it = chordQualityExplanations.find(quality);
    if (it != chordQualityExplanations.end())
    {
        return it->second;
    }
    return "A chord with unique harmonic characteristics.";
}

std::string ChordTooltips::getKeyName(int keyRoot, bool isMajor)
{
    const std::vector<std::string> noteNames = {
        "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
    };

    return noteNames[keyRoot] + (isMajor ? " major" : " minor");
}

std::string ChordTooltips::formatTooltip(const std::string& title, const std::string& explanation, const std::string& example)
{
    std::string formatted = title + "\n\n" + explanation;
    if (!example.empty())
    {
        formatted += "\n\nExample: " + example;
    }
    return formatted;
}

std::vector<std::string> ChordTooltips::getChordTones(const std::string& chordName)
{
    // Extract root note and quality
    std::string rootNote = chordName.substr(0, 1);

    if (chordName.length() > 1 && (chordName[1] == '#' || chordName[1] == 'b'))
    {
        rootNote += chordName[1];
    }

    std::vector<std::string> tones;
    tones.push_back(rootNote + " (Root)");

    // This is simplified - you could make it more sophisticated
    std::string quality = chordName.substr(rootNote.length());

    if (quality == "" || quality.find("maj") != std::string::npos)
    {
        tones.push_back("Major 3rd");
        tones.push_back("Perfect 5th");
    }
    else if (quality.find("m") != std::string::npos && quality.find("maj") == std::string::npos)
    {
        tones.push_back("Minor 3rd");
        tones.push_back("Perfect 5th");
    }

    if (quality.find("7") != std::string::npos)
    {
        if (quality.find("maj7") != std::string::npos)
            tones.push_back("Major 7th");
        else
            tones.push_back("Minor 7th");
    }

    return tones;
}