#include "PluginProcessor.h"
#include "PluginEditor.h"

MakeItHappenOTTProcessor::MakeItHappenOTTProcessor()
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
          .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
          .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
      ),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
}

MakeItHappenOTTProcessor::~MakeItHappenOTTProcessor()
{
}

const juce::String MakeItHappenOTTProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MakeItHappenOTTProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool MakeItHappenOTTProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool MakeItHappenOTTProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double MakeItHappenOTTProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MakeItHappenOTTProcessor::getNumPrograms()
{
    return 1;
}

int MakeItHappenOTTProcessor::getCurrentProgram()
{
    return 0;
}

void MakeItHappenOTTProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String MakeItHappenOTTProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void MakeItHappenOTTProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void MakeItHappenOTTProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Initialize multiband crossover filters
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 2;

    // Low band: everything below 250Hz
    lowPassLow.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    lowPassLow.setCutoffFrequency(250.0f);
    lowPassLow.prepare(spec);

    // Mid band: 250Hz to 2000Hz
    highPassMid.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    highPassMid.setCutoffFrequency(250.0f);
    highPassMid.prepare(spec);

    lowPassMid.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    lowPassMid.setCutoffFrequency(2000.0f);
    lowPassMid.prepare(spec);

    // High band: everything above 2000Hz
    highPassHigh.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    highPassHigh.setCutoffFrequency(2000.0f);
    highPassHigh.prepare(spec);

    // Allocate band buffers
    lowBandBuffer.setSize(2, samplesPerBlock);
    midBandBuffer.setSize(2, samplesPerBlock);
    highBandBuffer.setSize(2, samplesPerBlock);

    // Reset envelope followers
    for (int i = 0; i < 2; ++i)
    {
        lowEnvelope[i] = 0.0f;
        midEnvelope[i] = 0.0f;
        highEnvelope[i] = 0.0f;
    }
}

void MakeItHappenOTTProcessor::releaseResources()
{
    // Release resources here
}

bool MakeItHappenOTTProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}

void MakeItHappenOTTProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    const int numSamples = buffer.getNumSamples();
    const float sampleRate = static_cast<float>(getSampleRate());

    // Get parameters
    float depthPercent = apvts.getRawParameterValue("depth")->load(); // Now 0-100
    float depth = depthPercent / 100.0f; // Convert to 0-1 for mixing
    float inputGainDb = apvts.getRawParameterValue("inputGain")->load();
    float outputGainDb = apvts.getRawParameterValue("outputGain")->load();
    float time = apvts.getRawParameterValue("time")->load();

    float inputGain = juce::Decibels::decibelsToGain(inputGainDb);
    float outputGain = juce::Decibels::decibelsToGain(outputGainDb);

    // Update metering
    this->depthPercent.store(depthPercent);
    this->timePercent.store(time);
    this->gainMatchEnabled.store(apvts.getRawParameterValue("gainMatch")->load() > 0.5f);

    // Calculate input level (before processing)
    float inputLevel = 0.0f;
    for (int ch = 0; ch < totalNumInputChannels; ++ch)
    {
        float channelLevel = buffer.getRMSLevel(ch, 0, numSamples);
        inputLevel = juce::jmax(inputLevel, channelLevel);
    }
    inputLevelDb.store(juce::Decibels::gainToDecibels(inputLevel + 0.00001f));

    // Apply input gain
    buffer.applyGain(inputGain);

    // Low band parameters
    float lowThresholdDown = apvts.getRawParameterValue("lowThreshDown")->load();
    float lowRatioDown = apvts.getRawParameterValue("lowRatioDown")->load();
    float lowThresholdUp = apvts.getRawParameterValue("lowThreshUp")->load();
    float lowRatioUp = apvts.getRawParameterValue("lowRatioUp")->load();
    float lowAttack = apvts.getRawParameterValue("lowAttack")->load();
    float lowRelease = apvts.getRawParameterValue("lowRelease")->load();
    float lowGain = juce::Decibels::decibelsToGain(apvts.getRawParameterValue("lowGain")->load());

    // Mid band parameters
    float midThresholdDown = apvts.getRawParameterValue("midThreshDown")->load();
    float midRatioDown = apvts.getRawParameterValue("midRatioDown")->load();
    float midThresholdUp = apvts.getRawParameterValue("midThreshUp")->load();
    float midRatioUp = apvts.getRawParameterValue("midRatioUp")->load();
    float midAttack = apvts.getRawParameterValue("midAttack")->load();
    float midRelease = apvts.getRawParameterValue("midRelease")->load();
    float midGain = juce::Decibels::decibelsToGain(apvts.getRawParameterValue("midGain")->load());

    // High band parameters
    float highThresholdDown = apvts.getRawParameterValue("highThreshDown")->load();
    float highRatioDown = apvts.getRawParameterValue("highRatioDown")->load();
    float highThresholdUp = apvts.getRawParameterValue("highThreshUp")->load();
    float highRatioUp = apvts.getRawParameterValue("highRatioUp")->load();
    float highAttack = apvts.getRawParameterValue("highAttack")->load();
    float highRelease = apvts.getRawParameterValue("highRelease")->load();
    float highGain = juce::Decibels::decibelsToGain(apvts.getRawParameterValue("highGain")->load());

    // Store dry signal for mixing
    juce::AudioBuffer<float> dryBuffer(buffer.getNumChannels(), numSamples);
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        dryBuffer.copyFrom(ch, 0, buffer, ch, 0, numSamples);

    // Split into bands using Linkwitz-Riley crossover
    lowBandBuffer.makeCopyOf(buffer);
    midBandBuffer.makeCopyOf(buffer);
    highBandBuffer.makeCopyOf(buffer);

    // Apply crossover filters
    juce::dsp::AudioBlock<float> lowBlock(lowBandBuffer);
    juce::dsp::AudioBlock<float> midBlock(midBandBuffer);
    juce::dsp::AudioBlock<float> highBlock(highBandBuffer);

    juce::dsp::ProcessContextReplacing<float> lowContext(lowBlock);
    juce::dsp::ProcessContextReplacing<float> midContext(midBlock);
    juce::dsp::ProcessContextReplacing<float> highContext(highBlock);

    // Low band: just low-pass
    lowPassLow.process(lowContext);

    // Mid band: band-pass (high-pass then low-pass)
    highPassMid.process(midContext);
    lowPassMid.process(midContext);

    // High band: just high-pass
    highPassHigh.process(highContext);

    // Process each band with OTT compression using envelope followers
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        // LOW BAND
        auto* lowData = lowBandBuffer.getWritePointer(channel);
        for (int sample = 0; sample < numSamples; ++sample)
        {
            float input = lowData[sample];

            // Update envelope follower
            processEnvelope(lowEnvelope[channel], input, lowAttack, lowRelease, sampleRate);

            // Calculate gain reduction based on envelope
            float envelopeDb = juce::Decibels::gainToDecibels(lowEnvelope[channel] + 0.00001f);
            float gainReduction = 1.0f;

            // Downward compression
            if (envelopeDb > lowThresholdDown)
            {
                float diff = envelopeDb - lowThresholdDown;
                float reducedDb = lowThresholdDown + (diff / lowRatioDown);
                gainReduction *= juce::Decibels::decibelsToGain(reducedDb - envelopeDb);
            }

            // Upward compression (expansion)
            if (envelopeDb < lowThresholdUp)
            {
                float diff = lowThresholdUp - envelopeDb;
                float boostedDb = envelopeDb + (diff * (1.0f - 1.0f / lowRatioUp));
                gainReduction *= juce::Decibels::decibelsToGain(boostedDb - envelopeDb);
            }

            lowData[sample] = input * gainReduction * lowGain;
        }

        // MID BAND
        auto* midData = midBandBuffer.getWritePointer(channel);
        for (int sample = 0; sample < numSamples; ++sample)
        {
            float input = midData[sample];

            processEnvelope(midEnvelope[channel], input, midAttack, midRelease, sampleRate);

            float envelopeDb = juce::Decibels::gainToDecibels(midEnvelope[channel] + 0.00001f);
            float gainReduction = 1.0f;

            if (envelopeDb > midThresholdDown)
            {
                float diff = envelopeDb - midThresholdDown;
                float reducedDb = midThresholdDown + (diff / midRatioDown);
                gainReduction *= juce::Decibels::decibelsToGain(reducedDb - envelopeDb);
            }

            if (envelopeDb < midThresholdUp)
            {
                float diff = midThresholdUp - envelopeDb;
                float boostedDb = envelopeDb + (diff * (1.0f - 1.0f / midRatioUp));
                gainReduction *= juce::Decibels::decibelsToGain(boostedDb - envelopeDb);
            }

            midData[sample] = input * gainReduction * midGain;
        }

        // HIGH BAND
        auto* highData = highBandBuffer.getWritePointer(channel);
        for (int sample = 0; sample < numSamples; ++sample)
        {
            float input = highData[sample];

            processEnvelope(highEnvelope[channel], input, highAttack, highRelease, sampleRate);

            float envelopeDb = juce::Decibels::gainToDecibels(highEnvelope[channel] + 0.00001f);
            float gainReduction = 1.0f;

            if (envelopeDb > highThresholdDown)
            {
                float diff = envelopeDb - highThresholdDown;
                float reducedDb = highThresholdDown + (diff / highRatioDown);
                gainReduction *= juce::Decibels::decibelsToGain(reducedDb - envelopeDb);
            }

            if (envelopeDb < highThresholdUp)
            {
                float diff = highThresholdUp - envelopeDb;
                float boostedDb = envelopeDb + (diff * (1.0f - 1.0f / highRatioUp));
                gainReduction *= juce::Decibels::decibelsToGain(boostedDb - envelopeDb);
            }

            highData[sample] = input * gainReduction * highGain;
        }
    }

    // Apply stereo width to each band
    float lowWidth = apvts.getRawParameterValue("lowWidth")->load();
    float midWidth = apvts.getRawParameterValue("midWidth")->load();
    float highWidth = apvts.getRawParameterValue("highWidth")->load();

    applyStereoWidth(lowBandBuffer, lowWidth);
    applyStereoWidth(midBandBuffer, midWidth);
    applyStereoWidth(highBandBuffer, highWidth);

    // Calculate band levels for spectrum display
    float lowLevel = 0.0f;
    float midLevel = 0.0f;
    float highLevel = 0.0f;
    for (int ch = 0; ch < totalNumInputChannels; ++ch)
    {
        lowLevel = juce::jmax(lowLevel, lowBandBuffer.getRMSLevel(ch, 0, numSamples));
        midLevel = juce::jmax(midLevel, midBandBuffer.getRMSLevel(ch, 0, numSamples));
        highLevel = juce::jmax(highLevel, highBandBuffer.getRMSLevel(ch, 0, numSamples));
    }
    lowBandLevel.store(lowLevel);
    midBandLevel.store(midLevel);
    highBandLevel.store(highLevel);

    // Check solo states
    bool lowSolo = apvts.getRawParameterValue("lowSolo")->load() > 0.5f;
    bool midSolo = apvts.getRawParameterValue("midSolo")->load() > 0.5f;
    bool highSolo = apvts.getRawParameterValue("highSolo")->load() > 0.5f;
    bool anySolo = lowSolo || midSolo || highSolo;

    // Sum the bands back together (respecting solo)
    buffer.clear();
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        // If any solo is active, only add soloed bands
        if (anySolo)
        {
            if (lowSolo)
                buffer.addFrom(channel, 0, lowBandBuffer, channel, 0, numSamples);
            if (midSolo)
                buffer.addFrom(channel, 0, midBandBuffer, channel, 0, numSamples);
            if (highSolo)
                buffer.addFrom(channel, 0, highBandBuffer, channel, 0, numSamples);
        }
        else
        {
            // No solo active, add all bands
            buffer.addFrom(channel, 0, lowBandBuffer, channel, 0, numSamples);
            buffer.addFrom(channel, 0, midBandBuffer, channel, 0, numSamples);
            buffer.addFrom(channel, 0, highBandBuffer, channel, 0, numSamples);
        }
    }

    // Calculate RMS of wet signal before mixing (for gain match)
    float wetRMS = 0.0f;
    if (apvts.getRawParameterValue("gainMatch")->load() > 0.5f)
    {
        for (int ch = 0; ch < totalNumInputChannels; ++ch)
        {
            float channelRMS = buffer.getRMSLevel(ch, 0, numSamples);
            wetRMS = juce::jmax(wetRMS, channelRMS);
        }
    }

    // Apply depth (wet/dry mix)
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* wetData = buffer.getWritePointer(channel);
        auto* dryData = dryBuffer.getReadPointer(channel);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            wetData[sample] = dryData[sample] * (1.0f - depth) + wetData[sample] * depth;
        }
    }

    // Apply gain match compensation if enabled
    if (apvts.getRawParameterValue("gainMatch")->load() > 0.5f)
    {
        // Calculate RMS of dry signal
        float dryRMS = 0.0f;
        for (int ch = 0; ch < totalNumInputChannels; ++ch)
        {
            float channelRMS = dryBuffer.getRMSLevel(ch, 0, numSamples);
            dryRMS = juce::jmax(dryRMS, channelRMS);
        }

        // Apply compensation to match dry signal level
        if (wetRMS > 0.00001f && dryRMS > 0.00001f)
        {
            float compensationGain = dryRMS / wetRMS;
            buffer.applyGain(compensationGain);
        }
    }

    // Apply output gain
    buffer.applyGain(outputGain);

    // Calculate output level (after processing)
    float outputLevel = 0.0f;
    for (int ch = 0; ch < totalNumInputChannels; ++ch)
    {
        float channelLevel = buffer.getRMSLevel(ch, 0, numSamples);
        outputLevel = juce::jmax(outputLevel, channelLevel);
    }
    outputLevelDb.store(juce::Decibels::gainToDecibels(outputLevel + 0.00001f));

    // Update upward/downward percentages
    // UPWARD knob controls lowRatioUp, DOWNWARD knob controls highRatioUp
    // Ratio goes from 1-20, map to 0-100%
    upwardPercent.store(((lowRatioUp - 1.0f) / 19.0f) * 100.0f);  // Map 1-20 to 0-100%
    downwardPercent.store(((highRatioUp - 1.0f) / 19.0f) * 100.0f);
}

bool MakeItHappenOTTProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* MakeItHappenOTTProcessor::createEditor()
{
    return new MakeItHappenOTTEditor(*this);
}

void MakeItHappenOTTProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MakeItHappenOTTProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessorValueTreeState::ParameterLayout MakeItHappenOTTProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Global parameters
    layout.add(std::make_unique<juce::AudioParameterFloat>("depth", "Depth (%)",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 50.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("inputGain", "Input Gain (dB)",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("outputGain", "Output Gain (dB)",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("time", "Time (%)",
        juce::NormalisableRange<float>(0.0f, 1000.0f, 1.0f), 100.0f));
    layout.add(std::make_unique<juce::AudioParameterBool>("gainMatch", "Gain Match", false));

    // LOW BAND PARAMETERS
    layout.add(std::make_unique<juce::AudioParameterFloat>("lowThreshDown", "Low Thresh Down (dB)",
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f), -20.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("lowRatioDown", "Low Ratio Down",
        juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f), 3.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("lowThreshUp", "Low Thresh Up (dB)",
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f), -40.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("lowRatioUp", "Low Ratio Up",
        juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f), 2.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("lowAttack", "Low Attack (ms)",
        juce::NormalisableRange<float>(0.1f, 100.0f, 0.1f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("lowRelease", "Low Release (ms)",
        juce::NormalisableRange<float>(10.0f, 1000.0f, 1.0f), 100.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("lowGain", "Low Gain (dB)",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("lowWidth", "Low Width (%)",
        juce::NormalisableRange<float>(0.0f, 200.0f, 1.0f), 100.0f));
    layout.add(std::make_unique<juce::AudioParameterBool>("lowSolo", "Low Solo", false));

    // MID BAND PARAMETERS
    layout.add(std::make_unique<juce::AudioParameterFloat>("midThreshDown", "Mid Thresh Down (dB)",
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f), -20.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("midRatioDown", "Mid Ratio Down",
        juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f), 3.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("midThreshUp", "Mid Thresh Up (dB)",
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f), -40.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("midRatioUp", "Mid Ratio Up",
        juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f), 2.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("midAttack", "Mid Attack (ms)",
        juce::NormalisableRange<float>(0.1f, 100.0f, 0.1f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("midRelease", "Mid Release (ms)",
        juce::NormalisableRange<float>(10.0f, 1000.0f, 1.0f), 100.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("midGain", "Mid Gain (dB)",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("midWidth", "Mid Width (%)",
        juce::NormalisableRange<float>(0.0f, 200.0f, 1.0f), 100.0f));
    layout.add(std::make_unique<juce::AudioParameterBool>("midSolo", "Mid Solo", false));

    // HIGH BAND PARAMETERS
    layout.add(std::make_unique<juce::AudioParameterFloat>("highThreshDown", "High Thresh Down (dB)",
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f), -20.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("highRatioDown", "High Ratio Down",
        juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f), 3.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("highThreshUp", "High Thresh Up (dB)",
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f), -40.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("highRatioUp", "High Ratio Up",
        juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f), 2.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("highAttack", "High Attack (ms)",
        juce::NormalisableRange<float>(0.1f, 100.0f, 0.1f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("highRelease", "High Release (ms)",
        juce::NormalisableRange<float>(10.0f, 1000.0f, 1.0f), 100.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("highGain", "High Gain (dB)",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("highWidth", "High Width (%)",
        juce::NormalisableRange<float>(0.0f, 200.0f, 1.0f), 100.0f));
    layout.add(std::make_unique<juce::AudioParameterBool>("highSolo", "High Solo", false));

    return layout;
}

// Envelope follower function
void MakeItHappenOTTProcessor::processEnvelope(float& envelope, float input, float attack, float release, float sampleRate)
{
    float inputLevel = std::abs(input);

    // Convert attack/release from milliseconds to coefficient
    float attackCoeff = std::exp(-1.0f / (attack * 0.001f * sampleRate));
    float releaseCoeff = std::exp(-1.0f / (release * 0.001f * sampleRate));

    if (inputLevel > envelope)
        envelope = attackCoeff * envelope + (1.0f - attackCoeff) * inputLevel;
    else
        envelope = releaseCoeff * envelope + (1.0f - releaseCoeff) * inputLevel;
}

// Stereo width processing using Mid-Side technique
void MakeItHappenOTTProcessor::applyStereoWidth(juce::AudioBuffer<float>& buffer, float widthPercent)
{
    if (buffer.getNumChannels() < 2)
        return; // Only works with stereo

    const int numSamples = buffer.getNumSamples();
    float width = widthPercent / 100.0f; // Convert 0-200% to 0-2.0

    auto* leftData = buffer.getWritePointer(0);
    auto* rightData = buffer.getWritePointer(1);

    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Convert to Mid-Side
        float mid = (leftData[sample] + rightData[sample]) * 0.5f;
        float side = (leftData[sample] - rightData[sample]) * 0.5f;

        // Apply width to side signal
        side *= width;

        // Convert back to Left-Right
        leftData[sample] = mid + side;
        rightData[sample] = mid - side;
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MakeItHappenOTTProcessor();
}
