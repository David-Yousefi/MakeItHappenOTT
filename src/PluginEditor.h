#pragma once
#include "PluginProcessor.h"

// Custom Solo Button Component with vector "S"
class SoloButton : public juce::ToggleButton
{
public:
    SoloButton(juce::Colour activeColour) : activeColour(activeColour) {}

    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = getLocalBounds().toFloat();

        // Draw "S" as vector art
        juce::Path sPath;

        // Create "S" shape (simplified curve)
        float width = bounds.getWidth();
        float height = bounds.getHeight();
        float centerX = width * 0.5f;
        float centerY = height * 0.5f;

        // Top curve of S
        sPath.startNewSubPath(centerX + width * 0.2f, centerY - height * 0.3f);
        sPath.quadraticTo(centerX - width * 0.25f, centerY - height * 0.3f,
                          centerX - width * 0.25f, centerY - height * 0.1f);
        sPath.quadraticTo(centerX - width * 0.25f, centerY + height * 0.05f,
                          centerX + width * 0.1f, centerY + height * 0.05f);

        // Bottom curve of S
        sPath.quadraticTo(centerX + width * 0.25f, centerY + height * 0.05f,
                          centerX + width * 0.25f, centerY + height * 0.2f);
        sPath.quadraticTo(centerX + width * 0.25f, centerY + height * 0.35f,
                          centerX - width * 0.2f, centerY + height * 0.35f);

        // Set color based on toggle state
        if (getToggleState())
            g.setColour(activeColour);
        else
            g.setColour(juce::Colour(0xff444444)); // Gray when off

        g.strokePath(sPath, juce::PathStrokeType(2.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

private:
    juce::Colour activeColour;
};

// Custom LookAndFeel for OTT-style interface
class OTTLookAndFeel : public juce::LookAndFeel_V4
{
public:
    OTTLookAndFeel()
    {
        setColour(juce::Slider::thumbColourId, juce::Colour(0xffcccccc));
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff4a9eff));
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff2a2a2a));
        setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
        setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff1a1a1a));
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    }

    // Set a filmstrip image for knobs (optional)
    // Filmstrip should be vertical with each frame showing different knob position
    void setKnobImage(juce::Image image, int numFrames)
    {
        knobFilmStrip = image;
        filmStripFrames = numFrames;
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPosProportional, float rotaryStartAngle,
                         float rotaryEndAngle, juce::Slider& slider) override
    {
        // If we have a filmstrip image, use it
        if (knobFilmStrip.isValid() && filmStripFrames > 0)
        {
            const int frameIndex = (int)(sliderPosProportional * (filmStripFrames - 1));
            const int frameHeight = knobFilmStrip.getHeight() / filmStripFrames;
            const int frameWidth = knobFilmStrip.getWidth();

            g.drawImage(knobFilmStrip,
                       x, y, width, height,
                       0, frameIndex * frameHeight,
                       frameWidth, frameHeight);
        }
        else
        {
            // Otherwise draw the custom knob
            auto radius = (float)juce::jmin(width / 2, height / 2) - 8.0f;
            auto centreX = (float)x + (float)width * 0.5f;
            auto centreY = (float)y + (float)height * 0.5f;
            auto rx = centreX - radius;
            auto ry = centreY - radius;
            auto rw = radius * 2.0f;
            auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

            // Draw outer ring
            g.setColour(juce::Colour(0xff2a2a2a));
            g.fillEllipse(rx, ry, rw, rw);

            // Draw inner circle
            auto innerRadius = radius * 0.75f;
            g.setColour(juce::Colour(0xff1a1a1a));
            g.fillEllipse(centreX - innerRadius, centreY - innerRadius, innerRadius * 2.0f, innerRadius * 2.0f);

            // Draw arc for value
            juce::Path valueArc;
            valueArc.addCentredArc(centreX, centreY, radius * 0.85f, radius * 0.85f,
                                  0.0f, rotaryStartAngle, angle, true);

            auto bandColour = slider.getProperties()["bandColour"].toString();
            juce::Colour arcColour = juce::Colour(0xff4a9eff); // Default blue
            if (bandColour == "low") arcColour = juce::Colour(0xff00d4ff);
            else if (bandColour == "mid") arcColour = juce::Colour(0xff00ff88);
            else if (bandColour == "high") arcColour = juce::Colour(0xffff6600);

            g.setColour(arcColour);
            g.strokePath(valueArc, juce::PathStrokeType(3.5f));

            // Draw pointer
            juce::Path pointer;
            auto pointerLength = radius * 0.5f;
            auto pointerThickness = 3.0f;
            pointer.addRectangle(-pointerThickness * 0.5f, -radius * 0.65f, pointerThickness, pointerLength);
            pointer.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
            g.setColour(juce::Colours::white);
            g.fillPath(pointer);
        }
    }

private:
    juce::Image knobFilmStrip;
    int filmStripFrames = 0;
};

class MakeItHappenOTTEditor : public juce::AudioProcessorEditor,
                               private juce::Timer
{
public:
    MakeItHappenOTTEditor(MakeItHappenOTTProcessor&);
    ~MakeItHappenOTTEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    MakeItHappenOTTProcessor& audioProcessor;
    OTTLookAndFeel ottLookAndFeel;

    // Optional background image (can be loaded from resources or file)
    juce::Image backgroundImage;

    // Global controls (top 4 knobs)
    juce::Slider depthSlider;
    juce::Slider timeSlider;
    juce::Slider inputGainSlider;
    juce::Slider outputGainSlider;
    juce::Label depthLabel;

    // Gain match toggle
    juce::ToggleButton gainMatchButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> gainMatchAttachment;

    // Low band controls
    juce::Slider lowThreshDownSlider, lowRatioDownSlider, lowThreshUpSlider, lowRatioUpSlider;
    juce::Slider lowAttackSlider, lowReleaseSlider, lowGainSlider, lowWidthSlider;
    SoloButton lowSoloButton{juce::Colour(0xff00d4ff)};  // Cyan
    juce::Label lowBandLabel;

    // Mid band controls
    juce::Slider midThreshDownSlider, midRatioDownSlider, midThreshUpSlider, midRatioUpSlider;
    juce::Slider midAttackSlider, midReleaseSlider, midGainSlider, midWidthSlider;
    SoloButton midSoloButton{juce::Colour(0xff00ff88)};  // Green
    juce::Label midBandLabel;

    // High band controls
    juce::Slider highThreshDownSlider, highRatioDownSlider, highThreshUpSlider, highRatioUpSlider;
    juce::Slider highAttackSlider, highReleaseSlider, highGainSlider, highWidthSlider;
    SoloButton highSoloButton{juce::Colour(0xffff6600)};  // Orange
    juce::Label highBandLabel;

    // Parameter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> depthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> timeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputGainAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowThreshDownAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowRatioDownAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowThreshUpAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowRatioUpAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowAttackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowReleaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowWidthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> lowSoloAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> midThreshDownAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> midRatioDownAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> midThreshUpAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> midRatioUpAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> midAttackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> midReleaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> midGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> midWidthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> midSoloAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highThreshDownAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highRatioDownAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highThreshUpAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highRatioUpAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highAttackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highReleaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highWidthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> highSoloAttachment;

    // Helper function to setup sliders
    void setupSlider(juce::Slider& slider, const juce::String& suffix);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MakeItHappenOTTEditor)
};
