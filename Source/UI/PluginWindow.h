#pragma once
#include <JuceHeader.h>
#include "PluginEditor.h"
#include "AudioProcessorEditorContentComp.h"
//=============================================================================
class PluginWindow : public juce::DocumentWindow
{
public:
    PluginWindow (tracktion_engine::Plugin&);
    ~PluginWindow() override;

    static std::unique_ptr<Component> create (tracktion_engine::Plugin&);

    void show();

    void setEditor (std::unique_ptr<PluginEditor>);
    PluginEditor* getEditor() const         { return editor.get(); }
    
    void recreateEditor();
    void recreateEditorAsync();

    static bool isDPIAware(tracktion_engine::Plugin&)
    {
        // You should keep a DB of if plugins are DPI aware or not and recall that value
        // here. You should let the user toggle the value if the plugin appears tiny
        return true;
    }


private:
    void moved() override;
    void userTriedToCloseWindow() override          { plugin.windowState->closeWindowExplicitly(); }
    void closeButtonPressed() override              { userTriedToCloseWindow(); }
    float getDesktopScaleFactor() const override    { return 1.0f; }

    std::unique_ptr<PluginEditor> createContentComp();

    std::unique_ptr<PluginEditor> editor;
    
    tracktion_engine::Plugin& plugin;
    tracktion_engine::PluginWindowState& windowState;
};

//==============================================================================
#if JUCE_LINUX
 constexpr bool shouldAddPluginWindowToDesktop = false;
#else
 constexpr bool shouldAddPluginWindowToDesktop = true;
#endif
