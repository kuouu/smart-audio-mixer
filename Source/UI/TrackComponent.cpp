/*
  ==============================================================================

    TrackComponent.cpp
    Created: 7 Oct 2021 10:46:15pm
    Author:  tuiji da

  ==============================================================================
*/

#include "TrackComponent.h"
#include "../Utilities.h"

TrackComponent::TrackComponent (EditViewState& evs, te::Track::Ptr t)
    : editViewState (evs), track (t)
{
    track->state.addListener (this);
    track->edit.getTransport().addChangeListener (this);
    
    markAndUpdate (updateClips);
}

TrackComponent::~TrackComponent()
{
    track->state.removeListener (this);
    track->edit.getTransport().removeChangeListener (this);
}

void TrackComponent::paint (Graphics& g)
{
    g.fillAll (Colours::grey);
    
    if (editViewState.selectionManager.isSelected (track.get()))
    {
        g.setColour (Colours::red);
        
        auto rc = getLocalBounds();
        if (editViewState.showHeaders) rc = rc.withTrimmedLeft (-4);
        if (editViewState.showFooters) rc = rc.withTrimmedRight (-4);

        g.drawRect (rc, 2);
    }
}

void TrackComponent::mouseDown (const MouseEvent&)
{
    editViewState.selectionManager.selectOnly (track.get());
}

void TrackComponent::changeListenerCallback (ChangeBroadcaster*)
{
    markAndUpdate (updateRecordClips);
}

void TrackComponent::valueTreePropertyChanged (juce::ValueTree& v, const juce::Identifier& i)
{
    if (te::Clip::isClipState (v))
    {
        if (i == te::IDs::start
            || i == te::IDs::length)
        {
            markAndUpdate (updatePositions);
        }
    }
}

void TrackComponent::valueTreeChildAdded (juce::ValueTree&, juce::ValueTree& c)
{
    if (te::Clip::isClipState (c))
        markAndUpdate (updateClips);
}

void TrackComponent::valueTreeChildRemoved (juce::ValueTree&, juce::ValueTree& c, int)
{
    if (te::Clip::isClipState (c))
        markAndUpdate (updateClips);
}

void TrackComponent::valueTreeChildOrderChanged (juce::ValueTree& v, int a, int b)
{
    if (te::Clip::isClipState (v.getChild (a)))
        markAndUpdate (updatePositions);
    else if (te::Clip::isClipState (v.getChild (b)))
        markAndUpdate (updatePositions);
}

void TrackComponent::handleAsyncUpdate()
{
    if (compareAndReset (updateClips))
        buildClips();
    if (compareAndReset (updatePositions))
        resized();
    if (compareAndReset (updateRecordClips))
        buildRecordClips();
}

void TrackComponent::resized()
{
    for (auto cc : clips)
    {
        auto& c = cc->getClip();
        auto pos = c.getPosition();
        int x1 = editViewState.timeToX (pos.getStart(), getWidth());
        int x2 = editViewState.timeToX (pos.getEnd(), getWidth());
        
        cc->setBounds (x1, 0, x2 - x1, getHeight());
    }
}

void TrackComponent::buildClips()
{
    clips.clear();
    
    if (auto ct = dynamic_cast<te::ClipTrack*> (track.get()))
    {
        for (auto c : ct->getClips())
        {
            ClipComponent* cc = nullptr;
            
            if (dynamic_cast<te::WaveAudioClip*> (c))
                cc = new AudioClipComponent (editViewState, c);
            else if (dynamic_cast<te::MidiClip*> (c))
                cc = new MidiClipComponent (editViewState, c);
            else
                cc = new ClipComponent (editViewState, c);
            
            clips.add (cc);
            addAndMakeVisible (cc);
        }
    }
    
    resized();
}

void TrackComponent::buildRecordClips()
{
    bool needed = false;
    
    if (track->edit.getTransport().isRecording())
    {
        for (auto in : track->edit.getAllInputDevices())
        {
            if (in->isRecordingActive() && track == in->getTargetTracks().getFirst())
            {
                needed = true;
                break;
            }
        }
    }
    
    if (needed)
    {
        recordingClip = std::make_unique<RecordingClipComponent> (track, editViewState);
        addAndMakeVisible (*recordingClip);
    }
    else
    {
        recordingClip = nullptr;
    }
}
