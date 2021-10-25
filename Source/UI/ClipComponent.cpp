/*
  ==============================================================================

    ClipComponent.cpp
    Created: 7 Oct 2021 11:08:49pm
    Author:  tuiji da

  ==============================================================================
*/

#include "ClipComponent.h"
#include "TrackComponent.h"

ClipComponent::ClipComponent (EditViewState& evs, te::Clip::Ptr c)
    : editViewState (evs), clip (c)
{
}

void ClipComponent::paint (Graphics& g)
{
    g.fillAll (clip->getColour().withAlpha (0.5f));
    g.setColour (Colours::black);
    g.drawRect (getLocalBounds());
    
    if (editViewState.selectionManager.isSelected (clip.get()))
    {
        g.setColour (Colours::red);
        g.drawRect (getLocalBounds(), 2);
    }
}

void ClipComponent::mouseDown (const MouseEvent&)
{
    editViewState.selectionManager.selectOnly (clip.get());
    posX = clip.get()->getPosition().getStart();
}

void ClipComponent::mouseUp (const MouseEvent&)
{
    posX = clip.get()->getPosition().getStart();
}

void ClipComponent::mouseDrag (const MouseEvent& e)
{
    auto len = getWidth();
    auto distance = e.getDistanceFromDragStartX();
    auto newX = posX + editViewState.xToTime(distance, len) * 0.2;
    std::cout << posX << " + " << distance / len << " = " << newX << std::endl;
    clip.get()->setStart(newX, 0, 1);
}
