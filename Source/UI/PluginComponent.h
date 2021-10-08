/*
  ==============================================================================

    PluginComponent.h
    Created: 7 Oct 2021 11:28:53pm
    Author:  tuiji da

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "EditViewState.h"

using namespace juce;
namespace te = tracktion_engine;

class PluginComponent : public TextButton
{
public:
    PluginComponent (EditViewState&, te::Plugin::Ptr);
    ~PluginComponent() override;
    
    using TextButton::clicked;
    void clicked (const ModifierKeys& modifiers) override;
    
private:
    EditViewState& editViewState;
    te::Plugin::Ptr plugin;
};
