#pragma once

#include <JuceHeader.h>
#include "ChordAnalyzer.h"
#include <string>
#include <map>
#include <vector>

class ChordTooltips
{
public:
    ChordTooltips();
    ~ChordTooltips() = default;

    // Main tooltip generation methods
    std::string getChordTooltip(const ChordInfo& chord, int keyRoot, bool keyIsMajor);
    std::string getRomanNumeralTooltip(const std::string& romanNumeral, bool keyIsMajor);
    std::string getFunctionTooltip(const std::string& function, const std::string& romanNumeral);
    std::string getProgressionTooltip(const std::vector<std::string>& suggestions, const ChordInfo& currentChord);

    // Individual explanation methods
    std::string explainChordQuality(const std::string& quality);
    std::string explainRomanNumeral(const std::string& romanNumeral);
    std::string explainHarmonicFunction(const std::string& function);
    std::string explainProgression(const std::string& fromChord, const std::string& toChord);

private:
    // Theory explanation databases
    std::map<std::string, std::string> chordQualityExplanations;
    std::map<std::string, std::string> romanNumeralExplanations;
    std::map<std::string, std::string> functionExplanations;
    std::map<std::string, std::string> progressionExplanations;

    // Initialization methods
    void initializeChordQualityExplanations();
    void initializeRomanNumeralExplanations();
    void initializeFunctionExplanations();
    void initializeProgressionExplanations();

    // Helper methods
    std::string getKeyName(int keyRoot, bool isMajor);
    std::string formatTooltip(const std::string& title, const std::string& explanation,
        const std::string& example = "");
    std::vector<std::string> getChordTones(const std::string& chordName);
};