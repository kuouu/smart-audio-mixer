/*
  ==============================================================================

    PlayheadComponent.cpp
    Created: 7 Oct 2021 10:45:57pm
    Author:  tuiji da

  ==============================================================================
*/

#include "PlayheadComponent.h"

PlayheadComponent::PlayheadComponent (te::Edit& e , EditViewState& evs)
    : edit (e), editViewState (evs)
{
    startTimerHz (30);
}

void PlayheadComponent::paint (Graphics& g)
{
    double scrollX = editViewState.scrollX.get();
    int offset = editViewState.timeToX(scrollX, getWidth());
    g.setColour (Colours::yellow);
    g.drawRect (xPosition + offset, 0, 2, getHeight());
}

bool PlayheadComponent::hitTest (int x, int)
{
    double scrollX = editViewState.scrollX.get();
    int offset = editViewState.timeToX(scrollX, getWidth());
    if (std::abs (x - (xPosition + offset)) <= 3)
        return true;
    
    return false;
}

void PlayheadComponent::mouseDown (const MouseEvent&)
{
    edit.getTransport().setUserDragging (true);
}

void PlayheadComponent::mouseUp (const MouseEvent&)
{
    edit.getTransport().setUserDragging (false);
}

void PlayheadComponent::mouseDrag (const MouseEvent& e)
{
    double scrollX = editViewState.scrollX.get();
    double t = editViewState.xToTime (e.x, getWidth());
    edit.getTransport().setCurrentPosition (t - scrollX);
    timerCallback();
}

void PlayheadComponent::timerCallback()
{
    if (firstTimer)
    {
        // On Linux, don't set the mouse cursor until after the Component has appeared
        firstTimer = false;
        setMouseCursor (MouseCursor::LeftRightResizeCursor);
    }

    int newX = editViewState.timeToX (edit.getTransport().getCurrentPosition(), getWidth());
    if (newX != xPosition)
    {
        repaint (jmin (newX, xPosition) - 1, 0, jmax (newX, xPosition) - jmin (newX, xPosition) + 3, getHeight());
        xPosition = newX;
    }
    repaint();
}
