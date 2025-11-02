cmake_minimum_required(VERSION 3.24)
project(MakeItHappenOTT VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Fetch JUCE framework
include(FetchContent)
FetchContent_Declare(JUCE
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG 7.0.12)
FetchContent_MakeAvailable(JUCE)

# Check for VST3 SDK (optional but recommended)
if(NOT DEFINED ENV{VST3_SDK_DIR})
  message(WARNING "VST3_SDK_DIR not set. Plugin will still build but some features may be limited.")
endif()

juce_add_plugin(MakeItHappenOTT
    COMPANY_NAME "DosComp"
    BUNDLE_ID com.doscomp.makeithappenott
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE DOSC
    PLUGIN_CODE MAKE
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "MakeItHappenOTT")

target_sources(MakeItHappenOTT PRIVATE
    src/PluginProcessor.cpp
    src/PluginProcessor.h
    src/PluginEditor.cpp
    src/PluginEditor.h)

target_compile_definitions(MakeItHappenOTT PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0)

target_link_libraries(MakeItHappenOTT PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp)

# Optional: Add public compile definitions
target_compile_definitions(MakeItHappenOTT PUBLIC
    JUCE_ALSA=0
    JUCE_DIRECTSOUND=0
    JUCE_DISABLE_CAUTIOUS_PARAMETER_ID_CHECKING=1
    JUCE_PLUGINHOST_VST=0
    JUCE_PLUGINHOST_VST3=0
    JUCE_PLUGINHOST_LADSPA=0)
