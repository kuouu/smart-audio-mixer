/*
  ==============================================================================

    TimeBar.h
    Created: 31 Oct 2021 1:54:15pm
    Author:  郭又宗

  ==============================================================================
*/

#pragma once
#include "EditViewState.h"

using namespace juce;
namespace te = tracktion_engine;

class TimeBar : public Component
{
public:
    TimeBar (te::Edit&, EditViewState&);
    
    void paint (Graphics& g) override;

private:
    void drawLabels (Graphics& g);
    
    te::Edit& edit;
    EditViewState& editViewState;
};
