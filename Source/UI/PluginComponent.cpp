/*
  ==============================================================================

    PluginComponent.cpp
    Created: 7 Oct 2021 11:28:53pm
    Author:  tuiji da

  ==============================================================================
*/

#include "PluginComponent.h"
#include "../Utilities.h"

PluginComponent::PluginComponent (EditViewState& evs, te::Plugin::Ptr p)
    : editViewState (evs), plugin (p)
{
    setButtonText (plugin->getName().substring (0, 1));
}

PluginComponent::~PluginComponent()
{
}

void PluginComponent::clicked (const ModifierKeys& modifiers)
{
    editViewState.selectionManager.selectOnly (plugin.get());
    if (modifiers.isPopupMenu())
    {
        PopupMenu m;
        m.addItem ("Delete", [this] { plugin->deleteFromParent(); });
        m.showAt (this);
    }
    else
    {
        plugin->showWindowExplicitly();
    }
}
