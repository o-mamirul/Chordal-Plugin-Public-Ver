#pragma once

#include <JuceHeader.h>
#include <string>


class TokenGenerator
{
public:
    TokenGenerator();
    ~TokenGenerator() = default;


    static juce::String generateAuthToken(const juce::String& email, const juce::String& password);


    static juce::String generateAuthURL(const juce::String& email, 
                                       const juce::String& password,
                                       const juce::String& baseUrl = "https://web.com");


    static bool launchBrowserAuthentication(const juce::String& email,
                                           const juce::String& password,
                                           const juce::String& baseUrl = "https://web.com");


    static juce::int64 getCurrentTimestamp();


    static juce::String urlEncode(const juce::String& text);

private:
    
};
