/*
  ==============================================================================

    EditViewState.h
    Created: 7 Oct 2021 11:04:42pm
    Author:  tuiji da

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
using namespace juce;
namespace te = tracktion_engine;

namespace IDs
{
    #define DECLARE_ID(name)  const juce::Identifier name (#name);
    DECLARE_ID (EDITVIEWSTATE)
    DECLARE_ID (showGlobalTrack)
    DECLARE_ID (showMarkerTrack)
    DECLARE_ID (showChordTrack)
    DECLARE_ID (showMidiDevices)
    DECLARE_ID (showWaveDevices)
    DECLARE_ID (viewX1)
    DECLARE_ID (viewX2)
    DECLARE_ID (viewY)
    DECLARE_ID (scrollX)
    DECLARE_ID (drawWaveforms)
    DECLARE_ID (showHeaders)
    DECLARE_ID (showFooters)
    DECLARE_ID (showArranger)
    #undef DECLARE_ID
}


class EditViewState
{
public:
    EditViewState (te::Edit& e, te::SelectionManager& s)
        : edit (e), selectionManager (s)
    {
        state = edit.state.getOrCreateChildWithName (IDs::EDITVIEWSTATE, nullptr);
        
        auto um = &edit.getUndoManager();
        
        showGlobalTrack.referTo (state, IDs::showGlobalTrack, um, false);
        showMarkerTrack.referTo (state, IDs::showMarkerTrack, um, false);
        showChordTrack.referTo (state, IDs::showChordTrack, um, false);
        showArrangerTrack.referTo (state, IDs::showArranger, um, false);
        drawWaveforms.referTo (state, IDs::drawWaveforms, um, true);
        showHeaders.referTo (state, IDs::showHeaders, um, true);
        showFooters.referTo (state, IDs::showFooters, um, false);
        showMidiDevices.referTo (state, IDs::showMidiDevices, um, false);
        showWaveDevices.referTo (state, IDs::showWaveDevices, um, true);

        viewX1.referTo (state, IDs::viewX1, um, 0);
        viewX2.referTo (state, IDs::viewX2, um, 60);
        viewY.referTo (state, IDs::viewY, um, 0);
        scrollX.referTo (state, IDs::scrollX, um, 0.0);
    }
    
    int timeToX (double time, int width) const
    {
        return roundToInt (((time - viewX1) * width) / (viewX2 - viewX1));
    }
    
    double xToTime (int x, int width) const
    {
        return (double (x) / width) * (viewX2 - viewX1) + viewX1;
    }
    
    double beatToTime (double b) const
    {
        auto& ts = edit.tempoSequence;
        return ts.beatsToTime (b);
    }
    
    void setScrollX(double offset)
    {
        scrollX = scrollX + offset;
        if(scrollX > 0.0)
            scrollX = 0.0;
    }

    te::Edit& edit;
    te::SelectionManager& selectionManager;
    
    CachedValue<bool> showGlobalTrack, showMarkerTrack, showChordTrack, showArrangerTrack,
                      drawWaveforms, showHeaders, showFooters, showMidiDevices, showWaveDevices;
    
    CachedValue<double> viewX1, viewX2, viewY, scrollX;
    
    ValueTree state;
};
