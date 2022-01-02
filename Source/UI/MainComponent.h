#pragma once
#include "EditComponent.h"
#include "./PluginWindow.h"
#include "./AudioProcessorEditorContentComp.h"
#include "../Utilities.h"
#include "./EngineHelper.h"
#include "./ExtendedUIBehaviour.h"

namespace te = tracktion_engine;

class MainComponent : public Component,
    public MenuBarModel,
    private ChangeListener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    StringArray getMenuBarNames() override
    {
        return { "File", "Edit", "Option" };
    }

    PopupMenu getMenuForIndex (int menuIndex, const String& /*menuName*/) override;

    void menuItemSelected (int /*menuItemID*/, int /*topLevelMenuIndex*/) override {}
    void menuBarActivated (bool) override {}

private:
    te::Engine engine{ ProjectInfo::projectName, std::make_unique<ExtendedUIBehaviour>(), nullptr };
    te::SelectionManager selectionManager{ engine };
    std::unique_ptr<te::Edit> edit;
    std::unique_ptr<EditComponent> editComponent;

    juce::UndoManager undoManager;
    ApplicationCommandManager commandManager;

    juce::TextButton playPauseButton{ "Play" },
        newEditButton{ "New Edit" },
        deleteButton{ "Delete" };

    juce::Label editNameLabel{ "No Edit Loaded" };

    juce::ToggleButton showWaveformButton{ "Show Waveforms" };

    std::unique_ptr<MenuBarComponent> menuBar;

    void setupButtons();

    void updatePlayButtonText();

    void createOrLoadEdit(File editFile = {});

    void enableAllInputs();

    void changeListenerCallback(ChangeBroadcaster* source) override;

    void setFile(const File&);

    void deleteSelectedObj();

    void saveAsWav();

    void showPlugins();

    void newEdit();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
