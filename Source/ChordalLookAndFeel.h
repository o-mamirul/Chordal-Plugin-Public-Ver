#pragma once

#include <JuceHeader.h>

class ChordalLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ChordalLookAndFeel();
    ~ChordalLookAndFeel() override;

    //==============================================================================
    // Font Overrides
    //==============================================================================

    juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override;
    juce::Font getComboBoxFont(juce::ComboBox&) override;
    juce::Font getLabelFont(juce::Label&) override;

    // Custom font getters
    juce::Font getSourceSerifFont(float height, int styleFlags = juce::Font::plain);
    juce::Font getMonaSansFont(float height, int styleFlags = juce::Font::plain);

    //==============================================================================
    // Component Drawing Overrides
    //==============================================================================

    void drawButtonBackground(juce::Graphics&, juce::Button&, const juce::Colour& backgroundColour,
        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    void drawComboBox(juce::Graphics&, int width, int height, bool isButtonDown,
        int buttonX, int buttonY, int buttonW, int buttonH,
        juce::ComboBox& box) override;

    void drawLabel(juce::Graphics&, juce::Label&) override;

    //==============================================================================
    // Color Palette
    //==============================================================================

    // Base Colors
    juce::Colour colorWhite = juce::Colour(0xffffffff);
    juce::Colour colorOffWhite = juce::Colour(0xfffcfffe);
    juce::Colour colorSoftGreen = juce::Colour(0xffc2e1d4);
    juce::Colour colorLightGray = juce::Colour(0xfff8f9fa);

    // Accent Colors - Teal/Green
    juce::Colour colorTeal = juce::Colour(0xff177364);
    juce::Colour colorTealMedium = juce::Colour(0xff1f8e6f);
    juce::Colour colorTealLight = juce::Colour(0xff60afa3);

    // Accent Color - Pink
    juce::Colour colorAccentPink = juce::Colour(0xffFB4466);
    juce::Colour colorPinkLight = juce::Colour(0xffff456b);

    // Text Colors
    juce::Colour colorTextPrimary = juce::Colour(0xff221f56);
    juce::Colour colorTextSecondary = juce::Colour(0xff16181d);
    juce::Colour colorTextMuted = juce::Colour(0xff6c757d);
    juce::Colour colorTextLight = juce::Colour(0xff8e9aaf);

    // Suggestion Button Colors
    juce::Colour colorBasicBlue = juce::Colour(0xff0066CC);
    juce::Colour colorIntermediateOrange = juce::Colour(0xffFFAA00);
    juce::Colour colorAdvancedRed = juce::Colour(0xffCC0033);

private:
    //==============================================================================
    // Custom Font Members
    //==============================================================================

    juce::Typeface::Ptr sourceSerifTypeface;
    juce::Typeface::Ptr monaSansTypeface;

    void loadCustomFonts();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChordalLookAndFeel)
};
