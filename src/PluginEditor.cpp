#include "PluginProcessor.h"
#include "PluginEditor.h"

MakeItHappenOTTEditor::MakeItHappenOTTEditor(MakeItHappenOTTProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(600, 560); // Wider for better spacing
    setLookAndFeel(&ottLookAndFeel);

    // Start timer for UI updates (30 fps)
    startTimerHz(30);

    // Setup global sliders (top 4 knobs)
    setupSlider(depthSlider, "");
    depthSlider.getProperties().set("bandColour", "depth");
    addAndMakeVisible(depthSlider);
    depthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "depth", depthSlider);

    setupSlider(timeSlider, "");
    timeSlider.getProperties().set("bandColour", "depth");
    addAndMakeVisible(timeSlider);
    timeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "time", timeSlider);

    setupSlider(inputGainSlider, "");
    inputGainSlider.getProperties().set("bandColour", "depth");
    addAndMakeVisible(inputGainSlider);
    inputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "inputGain", inputGainSlider);

    setupSlider(outputGainSlider, "");
    outputGainSlider.getProperties().set("bandColour", "depth");
    addAndMakeVisible(outputGainSlider);
    outputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "outputGain", outputGainSlider);

    // Setup gain match toggle button
    gainMatchButton.setButtonText("GAIN MATCH");
    gainMatchButton.setColour(juce::ToggleButton::textColourId, juce::Colour(0xffaaaaaa));
    gainMatchButton.setColour(juce::ToggleButton::tickColourId, juce::Colour(0xff00ff88));
    gainMatchButton.setColour(juce::ToggleButton::tickDisabledColourId, juce::Colour(0xff444444));
    addAndMakeVisible(gainMatchButton);
    gainMatchAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.apvts, "gainMatch", gainMatchButton);

    depthLabel.setText("DEPTH", juce::dontSendNotification);
    depthLabel.setJustificationType(juce::Justification::centred);
    depthLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    depthLabel.setColour(juce::Label::textColourId, juce::Colour(0xffaaaaaa));
    addAndMakeVisible(depthLabel);

    // LOW BAND (Cyan color)
    lowBandLabel.setText("LOW", juce::dontSendNotification);
    lowBandLabel.setJustificationType(juce::Justification::centred);
    lowBandLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    lowBandLabel.setColour(juce::Label::textColourId, juce::Colour(0xff00d4ff)); // Cyan
    addAndMakeVisible(lowBandLabel);

    setupSlider(lowThreshDownSlider, "");
    lowThreshDownSlider.getProperties().set("bandColour", "low");
    setupSlider(lowRatioDownSlider, "");
    lowRatioDownSlider.getProperties().set("bandColour", "low");
    setupSlider(lowThreshUpSlider, "");
    lowThreshUpSlider.getProperties().set("bandColour", "low");
    setupSlider(lowRatioUpSlider, "");
    lowRatioUpSlider.getProperties().set("bandColour", "low");
    setupSlider(lowAttackSlider, "");
    lowAttackSlider.getProperties().set("bandColour", "low");
    setupSlider(lowReleaseSlider, "");
    lowReleaseSlider.getProperties().set("bandColour", "low");
    setupSlider(lowGainSlider, "");
    lowGainSlider.getProperties().set("bandColour", "low");

    addAndMakeVisible(lowThreshDownSlider);
    addAndMakeVisible(lowRatioDownSlider);
    addAndMakeVisible(lowThreshUpSlider);
    addAndMakeVisible(lowRatioUpSlider);
    addAndMakeVisible(lowAttackSlider);
    addAndMakeVisible(lowReleaseSlider);
    addAndMakeVisible(lowGainSlider);

    lowThreshDownAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "lowThreshDown", lowThreshDownSlider);
    lowRatioDownAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "lowRatioDown", lowRatioDownSlider);
    lowThreshUpAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "lowThreshUp", lowThreshUpSlider);
    lowRatioUpAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "lowRatioUp", lowRatioUpSlider);
    lowAttackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "lowAttack", lowAttackSlider);
    lowReleaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "lowRelease", lowReleaseSlider);
    lowGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "lowGain", lowGainSlider);

    setupSlider(lowWidthSlider, "");
    lowWidthSlider.getProperties().set("bandColour", "low");
    addAndMakeVisible(lowWidthSlider);
    lowWidthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "lowWidth", lowWidthSlider);

    addAndMakeVisible(lowSoloButton);
    lowSoloAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.apvts, "lowSolo", lowSoloButton);

    // MID BAND (Green color)
    midBandLabel.setText("MID", juce::dontSendNotification);
    midBandLabel.setJustificationType(juce::Justification::centred);
    midBandLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    midBandLabel.setColour(juce::Label::textColourId, juce::Colour(0xff00ff88)); // Green
    addAndMakeVisible(midBandLabel);

    setupSlider(midThreshDownSlider, "");
    midThreshDownSlider.getProperties().set("bandColour", "mid");
    setupSlider(midRatioDownSlider, "");
    midRatioDownSlider.getProperties().set("bandColour", "mid");
    setupSlider(midThreshUpSlider, "");
    midThreshUpSlider.getProperties().set("bandColour", "mid");
    setupSlider(midRatioUpSlider, "");
    midRatioUpSlider.getProperties().set("bandColour", "mid");
    setupSlider(midAttackSlider, "");
    midAttackSlider.getProperties().set("bandColour", "mid");
    setupSlider(midReleaseSlider, "");
    midReleaseSlider.getProperties().set("bandColour", "mid");
    setupSlider(midGainSlider, "");
    midGainSlider.getProperties().set("bandColour", "mid");

    addAndMakeVisible(midThreshDownSlider);
    addAndMakeVisible(midRatioDownSlider);
    addAndMakeVisible(midThreshUpSlider);
    addAndMakeVisible(midRatioUpSlider);
    addAndMakeVisible(midAttackSlider);
    addAndMakeVisible(midReleaseSlider);
    addAndMakeVisible(midGainSlider);

    midThreshDownAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "midThreshDown", midThreshDownSlider);
    midRatioDownAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "midRatioDown", midRatioDownSlider);
    midThreshUpAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "midThreshUp", midThreshUpSlider);
    midRatioUpAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "midRatioUp", midRatioUpSlider);
    midAttackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "midAttack", midAttackSlider);
    midReleaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "midRelease", midReleaseSlider);
    midGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "midGain", midGainSlider);

    setupSlider(midWidthSlider, "");
    midWidthSlider.getProperties().set("bandColour", "mid");
    addAndMakeVisible(midWidthSlider);
    midWidthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "midWidth", midWidthSlider);

    addAndMakeVisible(midSoloButton);
    midSoloAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.apvts, "midSolo", midSoloButton);

    // HIGH BAND (Orange/Red color)
    highBandLabel.setText("HIGH", juce::dontSendNotification);
    highBandLabel.setJustificationType(juce::Justification::centred);
    highBandLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    highBandLabel.setColour(juce::Label::textColourId, juce::Colour(0xffff6600)); // Orange
    addAndMakeVisible(highBandLabel);

    setupSlider(highThreshDownSlider, "");
    highThreshDownSlider.getProperties().set("bandColour", "high");
    setupSlider(highRatioDownSlider, "");
    highRatioDownSlider.getProperties().set("bandColour", "high");
    setupSlider(highThreshUpSlider, "");
    highThreshUpSlider.getProperties().set("bandColour", "high");
    setupSlider(highRatioUpSlider, "");
    highRatioUpSlider.getProperties().set("bandColour", "high");
    setupSlider(highAttackSlider, "");
    highAttackSlider.getProperties().set("bandColour", "high");
    setupSlider(highReleaseSlider, "");
    highReleaseSlider.getProperties().set("bandColour", "high");
    setupSlider(highGainSlider, "");
    highGainSlider.getProperties().set("bandColour", "high");

    addAndMakeVisible(highThreshDownSlider);
    addAndMakeVisible(highRatioDownSlider);
    addAndMakeVisible(highThreshUpSlider);
    addAndMakeVisible(highRatioUpSlider);
    addAndMakeVisible(highAttackSlider);
    addAndMakeVisible(highReleaseSlider);
    addAndMakeVisible(highGainSlider);

    highThreshDownAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "highThreshDown", highThreshDownSlider);
    highRatioDownAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "highRatioDown", highRatioDownSlider);
    highThreshUpAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "highThreshUp", highThreshUpSlider);
    highRatioUpAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "highRatioUp", highRatioUpSlider);
    highAttackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "highAttack", highAttackSlider);
    highReleaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "highRelease", highReleaseSlider);
    highGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "highGain", highGainSlider);

    setupSlider(highWidthSlider, "");
    highWidthSlider.getProperties().set("bandColour", "high");
    addAndMakeVisible(highWidthSlider);
    highWidthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "highWidth", highWidthSlider);

    addAndMakeVisible(highSoloButton);
    highSoloAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.apvts, "highSolo", highSoloButton);

    // Try to load custom artwork if available
    // To use custom artwork:
    // 1. Create a folder called "assets" in the plugin directory
    // 2. Add a "background.png" file (900x470 pixels recommended)
    // 3. Add a "knob.png" filmstrip file (64x6400 pixels for 100 frames recommended)

    juce::File assetsFolder = juce::File::getSpecialLocation(juce::File::currentExecutableFile)
        .getParentDirectory().getChildFile("assets");

    // Try to load background image
    juce::File bgFile = assetsFolder.getChildFile("background.png");
    if (bgFile.existsAsFile())
    {
        backgroundImage = juce::ImageCache::getFromFile(bgFile);
    }

    // Try to load knob filmstrip
    juce::File knobFile = assetsFolder.getChildFile("knob.png");
    if (knobFile.existsAsFile())
    {
        juce::Image knobImage = juce::ImageCache::getFromFile(knobFile);
        if (knobImage.isValid())
        {
            ottLookAndFeel.setKnobImage(knobImage, 100); // 100 frames
        }
    }
}

MakeItHappenOTTEditor::~MakeItHappenOTTEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void MakeItHappenOTTEditor::timerCallback()
{
    // Trigger repaint for real-time meter and spectrum updates
    repaint();
}

void MakeItHappenOTTEditor::setupSlider(juce::Slider& slider, const juce::String& suffix)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    slider.setTextValueSuffix(suffix);
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffcccccc));
    slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff0f0f0f));
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff333333));
}

void MakeItHappenOTTEditor::paint(juce::Graphics& g)
{
    // Draw background image if available, otherwise use dark background
    if (backgroundImage.isValid())
    {
        g.drawImage(backgroundImage, getLocalBounds().toFloat(),
                   juce::RectanglePlacement::fillDestination);
    }
    else
    {
        g.fillAll(juce::Colour(0xff1a1a1a));
    }

    // === TOP SECTION - Knob labels (under knobs) with values ===
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xffaaaaaa));

    int knobLabelY = 85;
    int topSpacing = (getWidth() - (70 * 4)) / 5;

    // DEPTH label with % value
    float depthVal = audioProcessor.depthPercent.load();
    g.drawText("DEPTH " + juce::String((int)depthVal) + "%", topSpacing, knobLabelY, 70, 12, juce::Justification::centred);

    // TIME label with % value
    float timeVal = audioProcessor.timePercent.load();
    g.drawText("TIME " + juce::String((int)timeVal) + "%", topSpacing * 2 + 70, knobLabelY, 70, 12, juce::Justification::centred);

    g.drawText("IN GAIN", topSpacing * 3 + 140, knobLabelY, 70, 12, juce::Justification::centred);
    g.drawText("OUT GAIN", topSpacing * 4 + 210, knobLabelY, 70, 12, juce::Justification::centred);

    // Title below knobs
    g.setColour(juce::Colour(0xff00d4ff));
    g.setFont(juce::Font(24.0f, juce::Font::bold));
    g.drawText("MAKE IT HAPPEN OTT", 0, 105, getWidth(), 25, juce::Justification::centred);

    // === METER DISPLAY SECTION ===
    int meterY = 140;

    g.setColour(juce::Colour(0xff0a0a0a));
    g.fillRect(10, meterY, getWidth() - 20, 30);

    g.setFont(juce::Font(9.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff888888));
    g.drawText("dB", 20, meterY + 3, 30, 10, juce::Justification::left);
    g.drawText("%", getWidth() - 50, meterY + 3, 30, 10, juce::Justification::left);

    g.setFont(juce::Font(16.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff00ff88));

    // Display actual meter values
    float outputDb = audioProcessor.outputLevelDb.load();
    float depthPct = audioProcessor.depthPercent.load();
    g.drawText(juce::String(outputDb, 1), 20, meterY + 12, 80, 15, juce::Justification::left);
    g.drawText(juce::String((int)depthPct), getWidth() - 50, meterY + 12, 40, 15, juce::Justification::left);

    // === H/M/L BAND SECTION ===
    int bandY = 185;
    int bandHeight = 75;

    // Background
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRect(10, bandY, getWidth() - 20, bandHeight * 3 + 10);

    // Band labels and spectrum displays
    const juce::Colour bandColors[] = {juce::Colour(0xffff6600), juce::Colour(0xff00ff88), juce::Colour(0xff00d4ff)};
    const char* bandLabels[] = {"H", "M", "L"};
    const float bandLevels[] = {audioProcessor.highBandLevel.load(), audioProcessor.midBandLevel.load(), audioProcessor.lowBandLevel.load()};

    for (int i = 0; i < 3; i++)
    {
        int yPos = bandY + 5 + (i * bandHeight);

        // Band letter
        g.setFont(juce::Font(14.0f, juce::Font::bold));
        g.setColour(bandColors[i]);
        g.drawText(bandLabels[i], 20, yPos + 25, 20, 20, juce::Justification::centred);

        // Spectrum display background
        g.setColour(juce::Colour(0xff0a0a0a));
        g.fillRect(50, yPos + 5, 180, 60);

        // Spectrum bars - only show if there's actual audio level
        float level = bandLevels[i];
        if (level > 0.001f) // Only draw if there's meaningful audio
        {
            g.setColour(bandColors[i].withAlpha(0.4f));
            // Scale level to bar heights (0-50 pixels)
            int maxBarHeight = (int)(level * 500.0f); // Scale up the level
            maxBarHeight = juce::jmin(maxBarHeight, 50); // Clamp to max 50px

            for (int x = 0; x < 30; x++)
            {
                // Create a pseudo-spectrum effect by varying heights slightly
                int variation = juce::Random::getSystemRandom().nextInt(10) - 5;
                int barHeight = juce::jmax(5, juce::jmin(maxBarHeight + variation, 50));
                g.fillRect(52 + x * 6, yPos + 65 - barHeight, 4, barHeight);
            }
        }
    }

    // Draw knob labels above H/M/L section
    g.setFont(juce::Font(8.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff888888));
    int labelX = 245;
    g.drawText("THRESH", labelX, bandY - 5, 55, 10, juce::Justification::centred);
    g.drawText("RATIO", labelX + 60, bandY - 5, 55, 10, juce::Justification::centred);
    g.drawText("GAIN", labelX + 120, bandY - 5, 55, 10, juce::Justification::centred);
    g.drawText("WIDTH", labelX + 180, bandY - 5, 55, 10, juce::Justification::centred);

    // === BOTTOM SECTION ===
    int bottomY = 420;

    g.setColour(juce::Colour(0xff2a2a2a));
    g.drawLine(10.0f, (float)bottomY, (float)(getWidth() - 10), (float)bottomY, 1.5f);

    // Bottom labels under knobs
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff00ff88));
    g.drawText("UPWARD", 70, bottomY + 85, 80, 12, juce::Justification::centred);

    g.setColour(juce::Colour(0xffff6600));
    g.drawText("DOWNWARD", getWidth() - 150, bottomY + 85, 80, 12, juce::Justification::centred);

    // Value boxes
    g.setColour(juce::Colour(0xff0a0a0a));
    g.fillRect(80, bottomY + 100, 60, 22);
    g.fillRect(getWidth() - 140, bottomY + 100, 60, 22);

    g.setFont(juce::Font(13.0f, juce::Font::bold));
    g.setColour(juce::Colours::white);

    // Display actual upward/downward percentages
    float upwardPct = audioProcessor.upwardPercent.load();
    float downwardPct = audioProcessor.downwardPercent.load();
    g.drawText(juce::String((int)upwardPct) + "%", 80, bottomY + 100, 60, 22, juce::Justification::centred);
    g.drawText(juce::String((int)downwardPct) + "%", getWidth() - 140, bottomY + 100, 60, 22, juce::Justification::centred);
}

void MakeItHappenOTTEditor::resized()
{
    int knobSize = 55;
    int topKnobSize = 70;

    // === TOP ROW - 4 KNOBS ===
    int topY = 10;
    int topSpacing = (getWidth() - (topKnobSize * 4)) / 5;

    depthSlider.setBounds(topSpacing, topY, topKnobSize, topKnobSize);
    timeSlider.setBounds(topSpacing * 2 + topKnobSize, topY, topKnobSize, topKnobSize);
    inputGainSlider.setBounds(topSpacing * 3 + topKnobSize * 2, topY, topKnobSize, topKnobSize);
    outputGainSlider.setBounds(topSpacing * 4 + topKnobSize * 3, topY, topKnobSize, topKnobSize);

    // Hide text boxes for top knobs
    depthSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    timeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    inputGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    outputGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

    // === H/M/L BAND SECTION - 3 knobs per band on right side ===
    int bandY = 185;
    int bandHeight = 75;
    int bandKnobsX = 245; // Start position for knobs

    // HIGH BAND (top row)
    highThreshDownSlider.setBounds(bandKnobsX, bandY + 10, knobSize, knobSize);
    highRatioDownSlider.setBounds(bandKnobsX + knobSize + 5, bandY + 10, knobSize, knobSize);
    highThreshUpSlider.setBounds(bandKnobsX + (knobSize + 5) * 2, bandY + 10, knobSize, knobSize);
    highWidthSlider.setBounds(bandKnobsX + (knobSize + 5) * 3, bandY + 10, knobSize, knobSize);
    highSoloButton.setBounds(45, bandY + 25, 30, 30);  // More space from left, larger button

    // MID BAND (middle row)
    midThreshDownSlider.setBounds(bandKnobsX, bandY + bandHeight + 10, knobSize, knobSize);
    midRatioDownSlider.setBounds(bandKnobsX + knobSize + 5, bandY + bandHeight + 10, knobSize, knobSize);
    midThreshUpSlider.setBounds(bandKnobsX + (knobSize + 5) * 2, bandY + bandHeight + 10, knobSize, knobSize);
    midWidthSlider.setBounds(bandKnobsX + (knobSize + 5) * 3, bandY + bandHeight + 10, knobSize, knobSize);
    midSoloButton.setBounds(45, bandY + bandHeight + 25, 30, 30);

    // LOW BAND (bottom row)
    lowThreshDownSlider.setBounds(bandKnobsX, bandY + bandHeight * 2 + 10, knobSize, knobSize);
    lowRatioDownSlider.setBounds(bandKnobsX + knobSize + 5, bandY + bandHeight * 2 + 10, knobSize, knobSize);
    lowThreshUpSlider.setBounds(bandKnobsX + (knobSize + 5) * 2, bandY + bandHeight * 2 + 10, knobSize, knobSize);
    lowWidthSlider.setBounds(bandKnobsX + (knobSize + 5) * 3, bandY + bandHeight * 2 + 10, knobSize, knobSize);
    lowSoloButton.setBounds(45, bandY + bandHeight * 2 + 25, 30, 30);

    // Hide text boxes for band knobs
    highThreshDownSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    highRatioDownSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    highThreshUpSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    highWidthSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    midThreshDownSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    midRatioDownSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    midThreshUpSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    midWidthSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    lowThreshDownSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    lowRatioDownSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    lowThreshUpSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    lowWidthSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

    // === BOTTOM SECTION - UPWARD and DOWNWARD knobs ===
    int bottomY = 430;

    lowRatioUpSlider.setBounds(75, bottomY, knobSize, knobSize);
    highRatioUpSlider.setBounds(getWidth() - 135, bottomY, knobSize, knobSize);

    // Hide text boxes for bottom knobs
    lowRatioUpSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    highRatioUpSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

    // Hide unused sliders/labels
    lowReleaseSlider.setVisible(false);
    midReleaseSlider.setVisible(false);
    highReleaseSlider.setVisible(false);
    highGainSlider.setVisible(false);
    lowBandLabel.setVisible(false);
    midBandLabel.setVisible(false);
    highBandLabel.setVisible(false);
    midRatioUpSlider.setVisible(false);
    highAttackSlider.setVisible(false);
    depthLabel.setVisible(false);

    // Position gain match toggle button (centered between the two bottom knobs)
    int buttonWidth = 100;
    int buttonHeight = 24;
    int buttonX = (getWidth() - buttonWidth) / 2;
    gainMatchButton.setBounds(buttonX, bottomY + 45, buttonWidth, buttonHeight);
}
