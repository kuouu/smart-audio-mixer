/*
  ==============================================================================

    TrackHeaderComponent.h
    Created: 7 Oct 2021 10:46:31pm
    Author:  tuiji da

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "EditViewState.h"

using namespace juce;
namespace te = tracktion_engine;

class TrackHeaderComponent : public Component,
                             private te::ValueTreeAllEventListener
{
public:
    TrackHeaderComponent (EditViewState&, te::Track::Ptr);
    ~TrackHeaderComponent() override;
    
    void paint (Graphics& g) override;
    void mouseDown (const MouseEvent& e) override;
    void resized() override;
    
private:
    void valueTreeChanged() override {}
    void valueTreePropertyChanged (juce::ValueTree&, const juce::Identifier&) override;
    
    EditViewState& editViewState;
    te::Track::Ptr track;
    
    ValueTree inputsState;
    Label trackName;
    TextButton armButton {"A"}, muteButton {"M"}, soloButton {"S"}, inputButton {"I"};
};
