/*
  ==============================================================================

    Utilities.h
    Created: 8 Oct 2021 3:22:39pm
    Author:  tuiji da

  ==============================================================================
*/

#pragma once

namespace te = tracktion_engine;

//==============================================================================
namespace Helpers
{
    static inline void addAndMakeVisible (Component& parent, const Array<Component*>& children)
    {
        for (auto c : children)
            parent.addAndMakeVisible (c);
    }

    static inline String getStringOrDefault (const String& stringToTest, const String& stringToReturnIfEmpty)
    {
        return stringToTest.isEmpty() ? stringToReturnIfEmpty : stringToTest;
    }
    
    static inline File findRecentEdit (const File& dir)
    {
        auto files = dir.findChildFiles (File::findFiles, false, "*.tracktionedit");
        
        if (files.size() > 0)
        {
            files.sort();
            return files.getLast();
        }
        
        return {};
    }
}

//==============================================================================
namespace PlayHeadHelpers
{
    // Quick-and-dirty function to format a timecode string
    static inline String timeToTimecodeString (double seconds)
    {
        auto millisecs = roundToInt (seconds * 1000.0);
        auto absMillisecs = std::abs (millisecs);

        return String::formatted ("%02d:%02d:%02d.%03d",
                                  millisecs / 3600000,
                                  (absMillisecs / 60000) % 60,
                                  (absMillisecs / 1000)  % 60,
                                  absMillisecs % 1000);
    }

    // Quick-and-dirty function to format a bars/beats string
    static inline String quarterNotePositionToBarsBeatsString (double quarterNotes, int numerator, int denominator)
    {
        if (numerator == 0 || denominator == 0)
            return "1|1|000";

        auto quarterNotesPerBar = ((double) numerator * 4.0 / (double) denominator);
        auto beats  = (fmod (quarterNotes, quarterNotesPerBar) / quarterNotesPerBar) * numerator;

        auto bar    = ((int) quarterNotes) / quarterNotesPerBar + 1;
        auto beat   = ((int) beats) + 1;
        auto ticks  = ((int) (fmod (beats, 1.0) * 960.0 + 0.5));

        return String::formatted ("%d|%d|%03d", bar, beat, ticks);
    }

    // Returns a textual description of a CurrentPositionInfo
    static inline String getTimecodeDisplay (const AudioPlayHead::CurrentPositionInfo& pos)
    {
        MemoryOutputStream displayText;

        displayText << String (pos.bpm, 2) << " bpm, "
                    << pos.timeSigNumerator << '/' << pos.timeSigDenominator
                    << "  -  " << timeToTimecodeString (pos.timeInSeconds)
                    << "  -  " << quarterNotePositionToBarsBeatsString (pos.ppqPosition,
                                                                        pos.timeSigNumerator,
                                                                        pos.timeSigDenominator);

        if (pos.isRecording)
            displayText << "  (recording)";
        else if (pos.isPlaying)
            displayText << "  (playing)";
        else
            displayText << "  (stopped)";

        return displayText.toString();
    }
}

//==============================================================================

//==============================================================================
class FlaggedAsyncUpdater : public AsyncUpdater
{
public:
    //==============================================================================
    void markAndUpdate (bool& flag)     { flag = true; triggerAsyncUpdate(); }
    
    bool compareAndReset (bool& flag) noexcept
    {
        if (! flag)
            return false;
        
        flag = false;
        return true;
    }
};

//==============================================================================
struct Thumbnail    : public Component
{
    Thumbnail (te::TransportControl& tc)
        : transport (tc)
    {
        cursorUpdater.setCallback ([this]
                                   {
                                       updateCursorPosition();

                                       if (smartThumbnail.isGeneratingProxy() || smartThumbnail.isOutOfDate())
                                           repaint();
                                   });
        cursor.setFill (findColour (Label::textColourId));
        addAndMakeVisible (cursor);
    }

    void setFile (const te::AudioFile& file)
    {
        smartThumbnail.setNewFile (file);
        cursorUpdater.startTimerHz (25);
        repaint();
    }

    void paint (Graphics& g) override
    {
        auto r = getLocalBounds();
        const auto colour = findColour (Label::textColourId);

        if (smartThumbnail.isGeneratingProxy())
        {
            g.setColour (colour.withMultipliedBrightness (0.9f));
            g.drawText ("Creating proxy: " + String (roundToInt (smartThumbnail.getProxyProgress() * 100.0f)) + "%",
                        r, Justification::centred);

        }
        else
        {
            const float brightness = smartThumbnail.isOutOfDate() ? 0.4f : 0.66f;
            g.setColour (colour.withMultipliedBrightness (brightness));
            smartThumbnail.drawChannels (g, r, true, { 0.0, smartThumbnail.getTotalLength() }, 1.0f);
        }
    }

    void mouseDown (const MouseEvent& e) override
    {
        transport.setUserDragging (true);
        mouseDrag (e);
    }

    void mouseDrag (const MouseEvent& e) override
    {
        jassert (getWidth() > 0);
        const float proportion = e.position.x / getWidth();
        transport.position = proportion * transport.getLoopRange().getLength();
    }

    void mouseUp (const MouseEvent&) override
    {
        transport.setUserDragging (false);
    }

private:
    te::TransportControl& transport;
    te::SmartThumbnail smartThumbnail { transport.engine, te::AudioFile (transport.engine), *this, nullptr };
    DrawableRectangle cursor;
    te::LambdaTimer cursorUpdater;

    void updateCursorPosition()
    {
        const double loopLength = transport.getLoopRange().getLength();
        const double proportion = loopLength == 0.0 ? 0.0 : transport.getCurrentPosition() / loopLength;

        auto r = getLocalBounds().toFloat();
        const float x = r.getWidth() * float (proportion);
        cursor.setRectangle (r.withWidth (2.0f).withX (x));
    }
};
