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
    
    drawLabels(g);
}

void TimeBar::drawLabels (Graphics& g)
{
    const int headerWidth = editViewState.showHeaders ? 150 : 0;
    const int footerWidth = editViewState.showFooters ? 150 : 0;
    const int width = getWidth() - headerWidth - footerWidth;
    const int interval = editViewState.timeToX(1, width);
    double scrollX = editViewState.scrollX.get();
    double offset = editViewState.timeToX(scrollX, width);
    int zoomedInterval = interval;
    while(zoomedInterval < 30) zoomedInterval *= 5;
    for (int xLabel = headerWidth + offset
         ; xLabel < getWidth() - footerWidth
         ; xLabel += zoomedInterval){
        g.setColour (Colours::white);
        g.drawRect (xLabel, 30, 1, 20);
        int time = (xLabel - headerWidth - offset) / interval;
        int sec = time % 60;
        int min = time / 60;
        g.drawText((min?String(min) + ":":"") + String(sec),
                   xLabel, 0, zoomedInterval, 20,
                   Justification::topLeft);
    }
}
