/*
  ==============================================================================

    EditComponent.h
    Created: 7 Oct 2021 9:59:59pm
    Author:  tuiji da

  ==============================================================================
*/

#pragma once
#include "PlayheadComponent.h"
#include "TrackComponent.h"
#include "TrackFooterComponent.h"
#include "TrackHeaderComponent.h"
#include "TimeBar.h"

namespace te = tracktion_engine;

class EditComponent : public juce::Component,
                      private te::ValueTreeAllEventListener,
                      private FlaggedAsyncUpdater,
                      private juce::ChangeListener
{
public:
    EditComponent (te::Edit&, te::SelectionManager&);
    ~EditComponent() override;
    
    EditViewState& getEditViewState()   { return editViewState; }
    void mouseMagnify (const MouseEvent&, float scaleFactor) override;
    
private:
    void valueTreeChanged() override {}
    
    void valueTreePropertyChanged (juce::ValueTree&, const juce::Identifier&) override;
    void valueTreeChildAdded (juce::ValueTree&, juce::ValueTree&) override;
    void valueTreeChildRemoved (juce::ValueTree&, juce::ValueTree&, int) override;
    void valueTreeChildOrderChanged (juce::ValueTree&, int, int) override;
    
    void handleAsyncUpdate() override;
    void resized() override;
    
    void changeListenerCallback (ChangeBroadcaster*) override { repaint(); }

    
    void buildTracks();
    
    te::Edit& edit;
    
    EditViewState editViewState;
    
    PlayheadComponent playhead {edit, editViewState};
    OwnedArray<TrackComponent> tracks;
    OwnedArray<TrackHeaderComponent> headers;
    OwnedArray<TrackFooterComponent> footers;
    TimeBar timebar {edit, editViewState};
    
    bool updateTracks = false, updateZoom = false;
};
