#pragma once
#include "EditComponent.h"
#include "./PluginWindow.h"
#include "./AudioProcessorEditorContentComp.h"
#include "../Utilities.h"
#include "./EngineHelper.h"
#include "./ExtendedUIBehaviour.h"

namespace te = tracktion_engine;

class MainComponent  : public Component,
                       private ChangeListener
{
public:
        //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    te::Engine engine { ProjectInfo::projectName, std::make_unique<ExtendedUIBehaviour>(), nullptr };
    te::SelectionManager selectionManager { engine };
    std::unique_ptr<te::Edit> edit;
    std::unique_ptr<EditComponent> editComponent;

    juce::TextButton settingsButton { "Settings" },
                     pluginsButton { "Plugins" },
                     newEditButton { "New" },
                     playPauseButton { "Play" },
                     showEditButton { "Show Edit" },
                     newTrackButton { "New Track" },
                     newAudioClipButton {"New Audio Clip"},
                     deleteButton { "Delete" };
    
    juce::Label editNameLabel { "No Edit Loaded" };
    
    juce::ToggleButton showWaveformButton { "Show Waveforms" };

    void setupButtons();
    
    void updatePlayButtonText();
    
    void createOrLoadEdit (File editFile = {});
    
    void enableAllInputs();
    
    void changeListenerCallback (ChangeBroadcaster* source) override;
    
    void setFile (const File&);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
