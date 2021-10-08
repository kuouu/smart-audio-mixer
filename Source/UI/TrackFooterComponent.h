/*
  ==============================================================================

    TrackFooterComponent.h
    Created: 7 Oct 2021 10:46:44pm
    Author:  tuiji da

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "EditViewState.h"
#include "PluginComponent.h"
#include "../Utilities.h"

using namespace juce;
namespace te = tracktion_engine;

class TrackFooterComponent : public Component,
                             private FlaggedAsyncUpdater,
                             private te::ValueTreeAllEventListener
{
public:
    TrackFooterComponent (EditViewState&, te::Track::Ptr);
    ~TrackFooterComponent() override;
    
    void paint (Graphics&) override;
    void mouseDown (const MouseEvent&) override;
    void resized() override;
    
private:
    void valueTreeChanged() override {}
    void valueTreeChildAdded (juce::ValueTree&, juce::ValueTree&) override;
    void valueTreeChildRemoved (juce::ValueTree&, juce::ValueTree&, int) override;
    void valueTreeChildOrderChanged (juce::ValueTree&, int, int) override;

    void handleAsyncUpdate() override;
    
    void buildPlugins();
    
    EditViewState& editViewState;
    te::Track::Ptr track;
    
    TextButton addButton {"+"};
    OwnedArray<PluginComponent> plugins;
    
    bool updatePlugins = false;
};
