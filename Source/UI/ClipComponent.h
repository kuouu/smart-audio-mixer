/*
  ==============================================================================

    ClipComponent.h
    Created: 7 Oct 2021 11:08:49pm
    Author:  tuiji da

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "EditViewState.h"

using namespace juce;
namespace te = tracktion_engine;

class ClipComponent : public Component
{
public:
    ClipComponent (EditViewState&, te::Clip::Ptr);
    
    void paint (Graphics& g) override;
    void mouseDown (const MouseEvent& e) override;
    
    te::Clip& getClip() { return *clip; }
    
protected:
    EditViewState& editViewState;
    te::Clip::Ptr clip;
};
