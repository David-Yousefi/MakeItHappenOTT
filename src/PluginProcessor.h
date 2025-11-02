#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class MakeItHappenOTTProcessor : public juce::AudioProcessor
{
public:
    MakeItHappenOTTProcessor();
    ~MakeItHappenOTTProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Audio Parameters Tree
    juce::AudioProcessorValueTreeState apvts;

    // Metering values (atomic for thread safety) - public for UI access
    std::atomic<float> inputLevelDb{0.0f};
    std::atomic<float> outputLevelDb{0.0f};
    std::atomic<float> depthPercent{50.0f};
    std::atomic<float> timePercent{100.0f};
    std::atomic<float> upwardPercent{50.0f};
    std::atomic<float> downwardPercent{50.0f};
    std::atomic<bool> gainMatchEnabled{false};

    // Band levels for spectrum display (RMS per band)
    std::atomic<float> lowBandLevel{0.0f};
    std::atomic<float> midBandLevel{0.0f};
    std::atomic<float> highBandLevel{0.0f};

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Multiband crossover filters (Linkwitz-Riley 4th order)
    juce::dsp::LinkwitzRileyFilter<float> lowPassLow, highPassLow;   // Low band
    juce::dsp::LinkwitzRileyFilter<float> lowPassMid, highPassMid;   // Mid band
    juce::dsp::LinkwitzRileyFilter<float> lowPassHigh, highPassHigh; // High band

    // Audio buffers for each band
    juce::AudioBuffer<float> lowBandBuffer, midBandBuffer, highBandBuffer;

    // Compressor helper function
    void processEnvelope(float& envelope, float input, float attack, float release, float sampleRate);

    // Stereo width processing (Mid-Side)
    void applyStereoWidth(juce::AudioBuffer<float>& buffer, float widthPercent);

    // Envelope followers for each band
    float lowEnvelope[2] = {0.0f, 0.0f};  // L/R
    float midEnvelope[2] = {0.0f, 0.0f};
    float highEnvelope[2] = {0.0f, 0.0f};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MakeItHappenOTTProcessor)
};
