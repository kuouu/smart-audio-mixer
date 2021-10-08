/*
  ==============================================================================

    MidiClipComponent.h
    Created: 7 Oct 2021 11:33:08pm
    Author:  tuiji da

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "EditViewState.h"
#include "ClipComponent.h"

using namespace juce;
namespace te = tracktion_engine;

class MidiClipComponent : public ClipComponent
{
public:
    MidiClipComponent (EditViewState&, te::Clip::Ptr);
    
    te::MidiClip* getMidiClip() { return dynamic_cast<te::MidiClip*> (clip.get()); }
    
    void paint (Graphics& g) override;
};
