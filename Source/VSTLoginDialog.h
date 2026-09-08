#pragma once

#include <JuceHeader.h>

class VSTLoginDialog : public juce::Component
{
public:
    VSTLoginDialog()
    {
        // Email field
        addAndMakeVisible(emailLabel);
        emailLabel.setText("Email:", juce::dontSendNotification);
        emailLabel.setJustificationType(juce::Justification::centredLeft);

        addAndMakeVisible(emailField);
        emailField.setTextToShowWhenEmpty("Enter your email", juce::Colours::grey);

        // Password field
        addAndMakeVisible(passwordLabel);
        passwordLabel.setText("Password:", juce::dontSendNotification);
        passwordLabel.setJustificationType(juce::Justification::centredLeft);

        addAndMakeVisible(passwordField);
        passwordField.setPasswordCharacter('*');
        passwordField.setTextToShowWhenEmpty("Enter your password", juce::Colours::grey);

        // Login button
        addAndMakeVisible(loginButton);
        loginButton.setButtonText("Login");
        loginButton.onClick = [this] {
            if (onLoginAttempt)
                onLoginAttempt(emailField.getText(), passwordField.getText());
            };

        // Cancel button
        addAndMakeVisible(cancelButton);
        cancelButton.setButtonText("Cancel");
        cancelButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xffFF8877));
        cancelButton.onClick = [this] {
            if (onCancel)
                onCancel();
            };

        // Status label
        addAndMakeVisible(statusLabel);
        statusLabel.setJustificationType(juce::Justification::centred);
        statusLabel.setColour(juce::Label::textColourId, juce::Colours::red);

        setSize(1550, 1550);
    }

    void resized() override
    {
        auto bounds = getLocalBounds().reduced(20);

        emailLabel.setBounds(bounds.removeFromTop(20));
        bounds.removeFromTop(5);
        emailField.setBounds(bounds.removeFromTop(25));
        bounds.removeFromTop(15);

        passwordLabel.setBounds(bounds.removeFromTop(20));
        bounds.removeFromTop(5);
        passwordField.setBounds(bounds.removeFromTop(25));
        bounds.removeFromTop(15);

        statusLabel.setBounds(bounds.removeFromTop(20));
        bounds.removeFromTop(10);

        auto buttonArea = bounds.removeFromTop(30);
        loginButton.setBounds(buttonArea.removeFromLeft(buttonArea.getWidth() / 2).reduced(5, 0));
        cancelButton.setBounds(buttonArea.reduced(5, 0));
    }

    void setStatus(const juce::String& message)
    {
        statusLabel.setText(message, juce::dontSendNotification);
    }

    void clearFields()
    {
        emailField.clear();
        passwordField.clear();
        statusLabel.setText("", juce::dontSendNotification);
    }

    // Callbacks
    std::function<void(juce::String, juce::String)> onLoginAttempt;
    std::function<void()> onCancel;

private:
    juce::Label emailLabel;
    juce::TextEditor emailField;
    juce::Label passwordLabel;
    juce::TextEditor passwordField;
    juce::TextButton loginButton;
    juce::TextButton cancelButton;
    juce::Label statusLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VSTLoginDialog)
};