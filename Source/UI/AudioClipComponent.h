/*
  ==============================================================================

    AudioClipComponent.h
    Created: 7 Oct 2021 11:32:50pm
    Author:  tuiji da

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "EditViewState.h"
#include "ClipComponent.h"

using namespace juce;
namespace te = tracktion_engine;

class AudioClipComponent : public ClipComponent
{
public:
    AudioClipComponent (EditViewState&, te::Clip::Ptr);
    
    te::WaveAudioClip* getWaveAudioClip() { return dynamic_cast<te::WaveAudioClip*> (clip.get()); }
    
    void paint (Graphics& g) override;
    
private:
    void updateThumbnail();
    void drawWaveform (Graphics& g, te::AudioClipBase& c, te::SmartThumbnail& thumb, Colour colour,
                       int left, int right, int y, int h, int xOffset);
    void drawChannels (Graphics& g, te::SmartThumbnail& thumb, Rectangle<int> area, bool useHighRes,
                       te::EditTimeRange time, bool useLeft, bool useRight,
                       float leftGain, float rightGain);

    std::unique_ptr<te::SmartThumbnail> thumbnail;
};
