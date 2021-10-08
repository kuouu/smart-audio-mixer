/*
  ==============================================================================

    PluginEditor.h
    Created: 8 Oct 2021 3:09:45am
    Author:  tuiji da

  ==============================================================================
*/

#pragma once
class PluginEditor : public Component
{
public:
    virtual bool allowWindowResizing() = 0;
    virtual ComponentBoundsConstrainer* getBoundsConstrainer() = 0;
};
