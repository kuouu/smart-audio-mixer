/*
  ==============================================================================

    TrackComponent.h
    Created: 7 Oct 2021 10:46:15pm
    Author:  tuiji da

  ==============================================================================
*/

#pragma once

#include "EditViewState.h"
#include "ClipComponent.h"
#include "RecordingClipComponent.h"
#include "MidiClipComponent.h"
#include "AudioClipComponent.h"
#include "../Utilities.h"

using namespace juce;
namespace te = tracktion_engine;

class TrackComponent : public Component,
                       private te::ValueTreeAllEventListener,
                       private FlaggedAsyncUpdater,
                       private ChangeListener
{
public:
    TrackComponent (EditViewState&, te::Track::Ptr);
    ~TrackComponent() override;
    
    void paint (Graphics& g) override;
    void mouseDown (const MouseEvent& e) override;
    void resized() override;

private:
    void changeListenerCallback (ChangeBroadcaster*) override;

    void valueTreeChanged() override {}
    
    void valueTreePropertyChanged (juce::ValueTree&, const juce::Identifier&) override;
    void valueTreeChildAdded (juce::ValueTree&, juce::ValueTree&) override;
    void valueTreeChildRemoved (juce::ValueTree&, juce::ValueTree&, int) override;
    void valueTreeChildOrderChanged (juce::ValueTree&, int, int) override;
    
    void handleAsyncUpdate() override;
    
    void buildClips();
    void buildRecordClips();
    
    EditViewState& editViewState;
    te::Track::Ptr track;
    
    OwnedArray<ClipComponent> clips;
    std::unique_ptr<RecordingClipComponent> recordingClip;
    
    bool updateClips = false, updatePositions = false, updateRecordClips = false;
};
