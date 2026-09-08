#include "ChordalLookAndFeel.h"

ChordalLookAndFeel::ChordalLookAndFeel()
{
    // Load custom fonts
    loadCustomFonts();

    // Set up the color scheme using JUCE's LookAndFeel color IDs
    // This sets default colors for all components

    //==============================================================================
    // General Colors
    //==============================================================================
    setColour(juce::ResizableWindow::backgroundColourId, colorSoftGreen);

    //==============================================================================
    // Label Colors
    //==============================================================================
    setColour(juce::Label::textColourId, colorTextPrimary);
    setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    setColour(juce::Label::outlineColourId, juce::Colours::transparentBlack);

    //==============================================================================
    // TextButton Colors
    //==============================================================================
    setColour(juce::TextButton::buttonColourId, colorTealMedium);
    setColour(juce::TextButton::textColourOffId, colorWhite);
    setColour(juce::TextButton::textColourOnId, colorWhite);
    setColour(juce::TextButton::buttonOnColourId, colorTeal);

    //==============================================================================
    // ComboBox Colors
    //==============================================================================
    setColour(juce::ComboBox::backgroundColourId, colorWhite);
    setColour(juce::ComboBox::textColourId, colorTextPrimary);
    setColour(juce::ComboBox::outlineColourId, colorTealMedium);
    setColour(juce::ComboBox::buttonColourId, colorTealMedium);
    setColour(juce::ComboBox::arrowColourId, colorWhite);

    // ComboBox popup menu
    setColour(juce::PopupMenu::backgroundColourId, colorWhite);
    setColour(juce::PopupMenu::textColourId, colorTextPrimary);
    setColour(juce::PopupMenu::headerTextColourId, colorTeal);
    setColour(juce::PopupMenu::highlightedBackgroundColourId, colorTealLight);
    setColour(juce::PopupMenu::highlightedTextColourId, colorWhite);

    //==============================================================================
    // ScrollBar Colors (for ComboBox dropdown)
    //==============================================================================
    setColour(juce::ScrollBar::backgroundColourId, colorLightGray);
    setColour(juce::ScrollBar::thumbColourId, colorTealLight);
    setColour(juce::ScrollBar::trackColourId, colorSoftGreen);

    //==============================================================================
    // TooltipWindow Colors
    //==============================================================================
    setColour(juce::TooltipWindow::backgroundColourId, colorTealMedium);
    setColour(juce::TooltipWindow::textColourId, colorWhite);
    setColour(juce::TooltipWindow::outlineColourId, colorTeal);
}

ChordalLookAndFeel::~ChordalLookAndFeel()
{
}

//==============================================================================
// Font Loading
//==============================================================================

void ChordalLookAndFeel::loadCustomFonts()
{
    
#ifdef SOURCE_SERIF_FONT_DATA
    sourceSerifTypeface = juce::Typeface::createSystemTypefaceFor(
        BinaryData::SourceSerif4Regular_ttf,
        BinaryData::SourceSerif4Regular_ttfSize
    );
#endif

#ifdef MONA_SANS_FONT_DATA
    monaSansTypeface = juce::Typeface::createSystemTypefaceFor(
        BinaryData::MonaSansRegular_ttf,
        BinaryData::MonaSansRegular_ttfSize
    );
#endif

    // This will attempt to find fonts by name if they're installed on the system
    if (sourceSerifTypeface == nullptr)
    {
        juce::StringArray fontNames = juce::Font::findAllTypefaceNames();

        // Look for Source Serif variants
        for (const auto& name : fontNames)
        {
            if (name.containsIgnoreCase("Source Serif"))
            {
                sourceSerifTypeface = juce::Typeface::createSystemTypefaceFor(name.toRawUTF8(), name.length());
                break;
            }
        }

        // Fallback to Georgia (similar serif font)
        if (sourceSerifTypeface == nullptr)
        {
            sourceSerifTypeface = juce::Font("Georgia", 14, juce::Font::plain).getTypefacePtr();
        }
    }

    if (monaSansTypeface == nullptr)
    {
        juce::StringArray fontNames = juce::Font::findAllTypefaceNames();

        // Look for Mona Sans variants
        for (const auto& name : fontNames)
        {
            if (name.containsIgnoreCase("Mona Sans"))
            {
                monaSansTypeface = juce::Typeface::createSystemTypefaceFor(name.toRawUTF8(), name.length());
                break;
            }
        }

        // Fallback to system sans-serif
        if (monaSansTypeface == nullptr)
        {
            monaSansTypeface = juce::Font(juce::Font::getDefaultSansSerifFontName(), 14, juce::Font::plain).getTypefacePtr();
        }
    }

    // Debug output
    DBG("ChordalLookAndFeel: Source Serif font loaded: " + juce::String(sourceSerifTypeface != nullptr ? "YES" : "NO"));
    DBG("ChordalLookAndFeel: Mona Sans font loaded: " + juce::String(monaSansTypeface != nullptr ? "YES" : "NO"));
}

juce::Font ChordalLookAndFeel::getSourceSerifFont(float height, int styleFlags)
{
    if (sourceSerifTypeface != nullptr)
        return juce::Font(sourceSerifTypeface).withHeight(height).withStyle(styleFlags);
    else
        return juce::Font("Georgia", height, styleFlags);
}

juce::Font ChordalLookAndFeel::getMonaSansFont(float height, int styleFlags)
{
    if (monaSansTypeface != nullptr)
        return juce::Font(monaSansTypeface).withHeight(height).withStyle(styleFlags);
    else
        return juce::Font(juce::Font::getDefaultSansSerifFontName(), height, styleFlags);
}

//==============================================================================
// Font Overrides
//==============================================================================

juce::Font ChordalLookAndFeel::getTextButtonFont(juce::TextButton&, int buttonHeight)
{
    return getMonaSansFont(juce::jmin(15.0f, buttonHeight * 0.6f), juce::Font::plain);
}

juce::Font ChordalLookAndFeel::getComboBoxFont(juce::ComboBox&)
{
    return getMonaSansFont(14.0f, juce::Font::plain);
}

juce::Font ChordalLookAndFeel::getLabelFont(juce::Label& label)
{
    // Check if this is a "main" label (like roman numerals) based on font size
    auto currentFont = label.getFont();

    // Use Source Serif for large labels (main display elements)
    if (currentFont.getHeight() >= 20.0f)
    {
        return getSourceSerifFont(currentFont.getHeight(), currentFont.getStyleFlags());
    }
    // Use Mona Sans for smaller labels (explanatory text)
    else
    {
        return getMonaSansFont(currentFont.getHeight(), currentFont.getStyleFlags());
    }
}

//==============================================================================
// Drawing Overrides
//==============================================================================

void ChordalLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
    const juce::Colour& backgroundColour,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);
    auto cornerSize = 4.0f;

    auto baseColour = backgroundColour;

    if (shouldDrawButtonAsDown)
        baseColour = baseColour.darker(0.2f);
    else if (shouldDrawButtonAsHighlighted)
        baseColour = baseColour.brighter(0.1f);

    // Draw button background with rounded corners
    g.setColour(baseColour);
    g.fillRoundedRectangle(bounds, cornerSize);

    // Draw subtle border
    g.setColour(baseColour.darker(0.3f));
    g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
}

void ChordalLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height,
    bool isButtonDown,
    int buttonX, int buttonY, int buttonW, int buttonH,
    juce::ComboBox& box)
{
    auto cornerSize = 4.0f;
    juce::Rectangle<int> boxBounds(0, 0, width, height);

    // Draw background
    g.setColour(box.findColour(juce::ComboBox::backgroundColourId));
    g.fillRoundedRectangle(boxBounds.toFloat().reduced(0.5f, 0.5f), cornerSize);

    // Draw outline
    g.setColour(box.findColour(juce::ComboBox::outlineColourId));
    g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f, 0.5f), cornerSize, 1.0f);

    // Draw arrow button area
    juce::Rectangle<int> arrowZone(buttonX, buttonY, buttonW, buttonH);

    g.setColour(box.findColour(juce::ComboBox::buttonColourId)
        .withAlpha(isButtonDown ? 0.9f : 0.8f));
    g.fillRect(arrowZone);

    // Draw arrow
    auto arrowX = arrowZone.getCentreX();
    auto arrowY = arrowZone.getCentreY();

    juce::Path path;
    path.addTriangle(arrowX - 4.0f, arrowY - 2.0f,
        arrowX + 4.0f, arrowY - 2.0f,
        arrowX, arrowY + 3.0f);

    g.setColour(box.findColour(juce::ComboBox::arrowColourId));
    g.fillPath(path);
}

void ChordalLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
    g.fillAll(label.findColour(juce::Label::backgroundColourId));

    if (!label.isBeingEdited())
    {
        auto alpha = label.isEnabled() ? 1.0f : 0.5f;
        const juce::Font font(getLabelFont(label));

        g.setColour(label.findColour(juce::Label::textColourId).withMultipliedAlpha(alpha));
        g.setFont(font);

        auto textArea = getLabelBorderSize(label).subtractedFrom(label.getLocalBounds());

        g.drawFittedText(label.getText(), textArea, label.getJustificationType(),
            juce::jmax(1, (int)(textArea.getHeight() / font.getHeight())),
            label.getMinimumHorizontalScale());

        g.setColour(label.findColour(juce::Label::outlineColourId).withMultipliedAlpha(alpha));
        g.drawRect(label.getLocalBounds());
    }
}
