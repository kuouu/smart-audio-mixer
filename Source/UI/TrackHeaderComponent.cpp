/*
  ==============================================================================

    TrackHeaderComponent.cpp
    Created: 7 Oct 2021 10:46:31pm
    Author:  tuiji da

  ==============================================================================
*/

#include "TrackHeaderComponent.h"
#include "../Utilities.h"
#include "./EngineHelper.h"

TrackHeaderComponent::TrackHeaderComponent (EditViewState& evs, te::Track::Ptr t)
    : editViewState (evs), track (t)
{
    Helpers::addAndMakeVisible (*this, { &trackName, &armButton, &muteButton, &soloButton, &inputButton });
    
    armButton.setColour (TextButton::buttonOnColourId, Colours::red);
    muteButton.setColour (TextButton::buttonOnColourId, Colours::red);
    soloButton.setColour (TextButton::buttonOnColourId, Colours::green);

    trackName.setText (t->getName(), dontSendNotification);
    
    if (auto at = dynamic_cast<te::AudioTrack*> (track.get()))
    {
        inputButton.onClick = [this, at]
        {
            PopupMenu m;
            
            if (EngineHelpers::trackHasInput (*at))
            {
                bool ticked = EngineHelpers::isInputMonitoringEnabled (*at);
                m.addItem (1000, "Input Monitoring", true, ticked);
                m.addSeparator();
            }
            
            if (editViewState.showWaveDevices)
            {
                int id = 1;
                for (auto instance : at->edit.getAllInputDevices())
                {
                    if (instance->getInputDevice().getDeviceType() == te::InputDevice::waveDevice)
                    {
                        bool ticked = instance->getTargetTracks().getFirst() == at;
                        m.addItem (id++, instance->getInputDevice().getName(), true, ticked);
                    }
                }
            }
            
            if (editViewState.showMidiDevices)
            {
                m.addSeparator();
                
                int id = 100;
                for (auto instance : at->edit.getAllInputDevices())
                {
                    if (instance->getInputDevice().getDeviceType() == te::InputDevice::physicalMidiDevice)
                    {
                        bool ticked = instance->getTargetTracks().getFirst() == at;
                        m.addItem (id++, instance->getInputDevice().getName(), true, ticked);
                    }
                }
            }

            int res = m.show();

            if (res == 1000)
            {
                EngineHelpers::enableInputMonitoring (*at, ! EngineHelpers::isInputMonitoringEnabled (*at));
            }
            else if (res >= 100)
            {
                int id = 100;
                for (auto instance : at->edit.getAllInputDevices())
                {
                    if (instance->getInputDevice().getDeviceType() == te::InputDevice::physicalMidiDevice)
                    {
                        if (id == res)
                            instance->setTargetTrack (*at, 0, true);
                        id++;
                    }
                }
            }
            else if (res >= 1)
            {
                int id = 1;
                for (auto instance : at->edit.getAllInputDevices())
                {
                    if (instance->getInputDevice().getDeviceType() == te::InputDevice::waveDevice)
                    {
                        if (id == res)
                            instance->setTargetTrack (*at, 0, true);
                        id++;
                    }
                }
            }
        };
        armButton.onClick = [this, at]
        {
            EngineHelpers::armTrack (*at, ! EngineHelpers::isTrackArmed (*at));
            armButton.setToggleState (EngineHelpers::isTrackArmed (*at), dontSendNotification);
        };
        muteButton.onClick = [at] { at->setMute (! at->isMuted (false)); };
        soloButton.onClick = [at] { at->setSolo (! at->isSolo (false)); };
        
        armButton.setToggleState (EngineHelpers::isTrackArmed (*at), dontSendNotification);
    }
    else
    {
        armButton.setVisible (false);
        muteButton.setVisible (false);
        soloButton.setVisible (false);
    }
    
    track->state.addListener (this);
    inputsState = track->edit.state.getChildWithName (te::IDs::INPUTDEVICES);
    inputsState.addListener (this);
    
    valueTreePropertyChanged (track->state, te::IDs::mute);
    valueTreePropertyChanged (track->state, te::IDs::solo);
    valueTreePropertyChanged (inputsState, te::IDs::targetIndex);
}

TrackHeaderComponent::~TrackHeaderComponent()
{
    track->state.removeListener (this);
}

void TrackHeaderComponent::valueTreePropertyChanged (juce::ValueTree& v, const juce::Identifier& i)
{
    if (te::TrackList::isTrack (v))
    {
        if (i == te::IDs::mute)
            muteButton.setToggleState ((bool)v[i], dontSendNotification);
        else if (i == te::IDs::solo)
            soloButton.setToggleState ((bool)v[i], dontSendNotification);
    }
    else if (v.hasType (te::IDs::INPUTDEVICES)
             || v.hasType (te::IDs::INPUTDEVICE)
             || v.hasType (te::IDs::INPUTDEVICEDESTINATION))
    {
        if (auto at = dynamic_cast<te::AudioTrack*> (track.get()))
        {
            armButton.setEnabled (EngineHelpers::trackHasInput (*at));
            armButton.setToggleState (EngineHelpers::isTrackArmed (*at), dontSendNotification);
        }
    }
}

void TrackHeaderComponent::paint (Graphics& g)
{
    g.setColour (Colours::grey);
    g.fillRect (getLocalBounds().withTrimmedRight (2));
    
    if (editViewState.selectionManager.isSelected (track.get()))
    {
        g.setColour (Colours::red);
        g.drawRect (getLocalBounds().withTrimmedRight (-4), 2);
    }
}

void TrackHeaderComponent::mouseDown (const MouseEvent&)
{
    editViewState.selectionManager.selectOnly (track.get());
}

void TrackHeaderComponent::resized()
{
    auto r = getLocalBounds().reduced (4);
    trackName.setBounds (r.removeFromTop (r.getHeight() / 2));
    
    int w = r.getHeight();
    inputButton.setBounds (r.removeFromLeft (w));
    r.removeFromLeft (2);
    armButton.setBounds (r.removeFromLeft (w));
    r.removeFromLeft (2);
    muteButton.setBounds (r.removeFromLeft (w));
    r.removeFromLeft (2);
    soloButton.setBounds (r.removeFromLeft (w));
    r.removeFromLeft (2);
}
