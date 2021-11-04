/*
  ==============================================================================

    TimeBar.cpp
    Created: 31 Oct 2021 1:54:15pm
    Author:  郭又宗

  ==============================================================================
*/

#include "TimeBar.h"

TimeBar::TimeBar (te::Edit& e , EditViewState& evs)
    : edit (e), editViewState (evs)
{
}

void TimeBar::paint (Graphics& g)
{
    const int trackHeight = 50, trackGap = 2;
    const int headerWidth = editViewState.showHeaders ? 150 : 0;
    const int footerWidth = editViewState.showFooters ? 150 : 0;
    // header
    g.setColour (Colours::green);
    g.drawRect (0, 0, headerWidth - trackGap, trackHeight);
    // body
    g.setColour (Colours::blue);
    g.drawRect (headerWidth, 0, getWidth() - headerWidth - footerWidth, trackHeight);
    drawLabels(g);
    // footer
    g.setColour (Colours::orange);
    g.drawRect (getWidth() - footerWidth + trackGap, 0, footerWidth - trackGap, trackHeight);
}

void TimeBar::drawLabels (Graphics& g)
{
    const int headerWidth = editViewState.showHeaders ? 150 : 0;
    const int footerWidth = editViewState.showFooters ? 150 : 0;
    for (int xLabel = headerWidth
         ; xLabel < getWidth() - footerWidth
         ; xLabel += editViewState.timeToX(1, getWidth())){
        g.setColour (Colours::white);
        g.drawRect (xLabel, 30, 1, 20);
    }
}
