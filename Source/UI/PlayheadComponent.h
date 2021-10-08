/*
  ==============================================================================

    PlayheadComponent.h
    Created: 7 Oct 2021 10:45:57pm
    Author:  tuiji da

  ==============================================================================
*/

#pragma once
#include "EditViewState.h"

using namespace juce;
namespace te = tracktion_engine;

class PlayheadComponent : public Component,
                          private Timer
{
public:
    PlayheadComponent (te::Edit&, EditViewState&);
    
    void paint (Graphics& g) override;
    bool hitTest (int x, int y) override;
    void mouseDrag (const MouseEvent&) override;
    void mouseDown (const MouseEvent&) override;
    void mouseUp (const MouseEvent&) override;

private:
    void timerCallback() override;
    
    te::Edit& edit;
    EditViewState& editViewState;
    
    int xPosition = 0;
    bool firstTimer = true;
};
