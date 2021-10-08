/*
  ==============================================================================

    ClipComponent.cpp
    Created: 7 Oct 2021 11:08:49pm
    Author:  tuiji da

  ==============================================================================
*/

#include "ClipComponent.h"

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
}
