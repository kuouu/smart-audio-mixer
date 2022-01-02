/*
  ==============================================================================

    EngineHelper.h
    Created: 8 Oct 2021 2:59:04am
    Author:  tuiji da

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

namespace EngineHelpers
{
    tracktion_engine::Project::Ptr createTempProject(tracktion_engine::Engine& engine);
    void showAudioDeviceSettings(tracktion_engine::Engine& engine); ///< Show the the audio device settings for selecting a new device model + hardware
    void browseForAudioFile(tracktion_engine::Engine& engine, std::function<void(const juce::File&)> fileChosenCallback);
    void browseForEditFile(tracktion_engine::Engine& engine, std::function<void(const juce::File&)> fileChosenCallback);
    void removeAllClips(tracktion_engine::AudioTrack& track);
    tracktion_engine::AudioTrack* getOrInsertAudioTrackAt(tracktion_engine::Edit& edit, int index);
    tracktion_engine::WaveAudioClip::Ptr loadAudioFileAsClip(tracktion_engine::Edit& edit, const juce::File& file, int);
    template<typename ClipType> typename ClipType::Ptr loopAroundClip(ClipType& clip);
    void stop(tracktion_engine::Edit& edit); ///< stops the engine and rewind to start
    void togglePlay(tracktion_engine::Edit& edit); ///< Play or pause the engine
    void toggleRecord(tracktion_engine::Edit& edit); ///< Record a new clip if the current track is armed
    void armTrack(tracktion_engine::AudioTrack& t, bool arm, int position = 0); ///<  Arm a track for recording @see toggleRecord()
    bool isTrackArmed(tracktion_engine::AudioTrack& t, int position = 0);
    bool isInputMonitoringEnabled(tracktion_engine::AudioTrack& t, int position = 0);
    void enableInputMonitoring(tracktion_engine::AudioTrack& t, bool im, int position = 0);
    bool trackHasInput(tracktion_engine::AudioTrack& t, int position = 0);
    std::unique_ptr<juce::KnownPluginList::PluginTree> createPluginTree(tracktion_engine::Engine& engine);
}
