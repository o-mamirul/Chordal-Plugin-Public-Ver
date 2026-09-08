#include "TokenGenerator.h"

TokenGenerator::TokenGenerator()
{
}

juce::String TokenGenerator::generateAuthToken(const juce::String& email, const juce::String& password)
{
    juce::int64 timestamp = getCurrentTimestamp();

    // Construct token payload: Email:Password:Timestamp
    // NOTE: Password is sent in plaintext within the token
    // TOKEN MUST BE TRANSMITTED OVER HTTPS ONLY!
    juce::String payload = email + ":" + password + ":" + juce::String(timestamp);

    // Convert to UTF-8 bytes
    juce::MemoryBlock payloadBytes;
    payloadBytes.append(payload.toRawUTF8(), payload.getNumBytesAsUTF8());

    // Encode as Base64
    juce::String token = juce::Base64::toBase64(payloadBytes.getData(), payloadBytes.getSize());

    return token;
}

juce::String TokenGenerator::generateAuthURL(const juce::String& email,
    const juce::String& password,
    const juce::String& baseUrl)
{

    juce::String token = generateAuthToken(email, password);

    // URL-encode the email and token for safe transmission
    juce::String encodedEmail = urlEncode(email);
    juce::String encodedToken = urlEncode(token);

    // Construct the full URL
    juce::String url = baseUrl;

    // Ensure baseUrl doesn't end with slash
    if (url.endsWithChar('/'))
        url = url.dropLastCharacters(1);

    // Add the endpoint and parameters
    url += "/TokenLogin.aspx?email=" + encodedEmail + "&token=" + encodedToken;

    return url;
}

bool TokenGenerator::launchBrowserAuthentication(const juce::String& email,
    const juce::String& password,
    const juce::String& baseUrl)
{
    // Generate the authentication URL with actual password
    juce::String authUrl = generateAuthURL(email, password, baseUrl);

    DBG("Launching browser authentication...");
    DBG("Auth URL: " + authUrl);

    // Launch the URL in the default browser
    juce::URL url(authUrl);
    bool success = url.launchInDefaultBrowser();

    if (success)
    {
        DBG("Browser launched successfully");
    }
    else
    {
        DBG("Failed to launch browser");
    }

    return success;
}

juce::int64 TokenGenerator::getCurrentTimestamp()
{
    // Get current time in milliseconds since epoch
    juce::int64 timeInMillis = juce::Time::currentTimeMillis();

    // Convert to seconds
    juce::int64 timeInSeconds = timeInMillis / 1000;

    return timeInSeconds;
}

juce::String TokenGenerator::urlEncode(const juce::String& text)
{
    juce::String result;

    const char* hex = "0123456789ABCDEF";

    for (int i = 0; i < text.length(); ++i)
    {
        char c = text[i];

        // Characters that don't need encoding
        if ((c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') ||
            c == '-' || c == '_' || c == '.' || c == '~')
        {
            result += c;
        }
        else
        {
            // Percent-encode other characters
            result += '%';
            result += hex[(c >> 4) & 0x0F];
            result += hex[c & 0x0F];
        }
    }

    return result;
}