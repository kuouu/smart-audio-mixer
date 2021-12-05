#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    #if JUCE_MAC
        MenuBarModel::setMacMainMenu (this);
    #endif

	updatePlayButtonText();
	editNameLabel.setJustificationType(Justification::centred);
	Helpers::addAndMakeVisible(*this, { &playPauseButton, &deleteButton, &editNameLabel, &showWaveformButton });

	deleteButton.setEnabled(false);

	auto d = File::getSpecialLocation(File::userDesktopDirectory).getChildFile("saved");
	d.createDirectory();

	auto f = Helpers::findRecentEdit(d);
	if (f.existsAsFile())
		createOrLoadEdit(f);
	else
		createOrLoadEdit(d.getNonexistentChildFile("Test", ".tracktionedit", false));

	selectionManager.addChangeListener(this);

	setupButtons();

	setSize(600, 400);
}

MainComponent::~MainComponent()
{
	te::EditFileOperations(*edit).save(true, true, false);
	engine.getTemporaryFileManager().getTempDirectory().deleteRecursively();
    #if JUCE_MAC
        MenuBarModel::setMacMainMenu (nullptr);
    #endif
}

//==============================================================================
void MainComponent::paint(juce::Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{

	auto r = getLocalBounds();
	int w = r.getWidth() / 3;
	auto topR = r.removeFromTop(40);
    juce::FlexBox fb;
    fb.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
    fb.alignContent = juce::FlexBox::AlignContent::center;  
    fb.items.addArray ( { 
        juce::FlexItem(deleteButton).withMinWidth (50.0f).withMinHeight (30.0f), 
        juce::FlexItem(playPauseButton).withMinWidth (50.0f).withMinHeight (30.0f), 
        juce::FlexItem(showWaveformButton).withMinWidth (50.0f).withMinHeight (30.0f), 
    } );
    fb.performLayout(topR.reduced(2));

	topR = r.removeFromTop(32);
	editNameLabel.setBounds(topR);

	if (editComponent != nullptr)
		editComponent->setBounds(r);
}

PopupMenu MainComponent::getMenuForIndex (int menuIndex, const String& /*menuName*/)
{
    PopupMenu menu;
    auto lambda = []() { std::cout << "Hello, Lambda" << std::endl; };

    if (menuIndex == 0)
    {
        menu.addItem ("Save As", [this]{ saveAsWav(); });
        menu.addItem ("Save Project", [this]{ /* TODO */ });
        menu.addSeparator();
        menu.addItem ("New Track", [this]{
            edit->ensureNumberOfAudioTracks(getAudioTracks(*edit).size() + 1);
        });
        menu.addItem ("New Audio Clip", [this]{
            EngineHelpers::browseForAudioFile(engine, [this](const File& f) { setFile(f); });
        });
        menu.addItem ("New Project", [&]{ newEdit(); });
        menu.addSeparator();
        menu.addItem ("Show Project Path", [this]{
            auto d = File::getSpecialLocation(File::userDesktopDirectory).getChildFile("saved");
	        d.createDirectory();
            auto f = Helpers::findRecentEdit(d);
            if (f.existsAsFile()) f.revealToUser();
        });
        menu.addSeparator();
        menu.addItem ("Import Project", [this]{
            EngineHelpers::browseForEditFile(engine, [this](const File& f) { createOrLoadEdit(f); });
        });
    }
    else if (menuIndex == 1)
    {
        menu.addItem ("Undo", [this]{ edit->undo(); });
        menu.addItem ("Redo", [this]{ edit->redo(); });
    }
    else if (menuIndex == 2)
    {
        menu.addItem ("Settings", [this] { 
            EngineHelpers::showAudioDeviceSettings(engine); 
        });
        menu.addItem ("Plugins", [this]{ showPlugins(); });
    }

    return menu;
}

void  MainComponent::setupButtons()
{
	playPauseButton.onClick = [this]
	{
		EngineHelpers::togglePlay(*edit);
	};
	deleteButton.onClick = [this]
	{
		deleteSelectedObj();
	};
	showWaveformButton.onClick = [this]
	{
		auto& evs = editComponent->getEditViewState();
		evs.drawWaveforms = !evs.drawWaveforms.get();
		showWaveformButton.setToggleState(evs.drawWaveforms, dontSendNotification);
	};
}


void MainComponent::updatePlayButtonText()
{
	if (edit != nullptr)
		playPauseButton.setButtonText(edit->getTransport().isPlaying() ? "Stop" : "Play");
}

void MainComponent::createOrLoadEdit(File editFile)
{
	if (editFile == File())
	{
		FileChooser fc("New Edit", File::getSpecialLocation(File::userDesktopDirectory), "*.tracktionedit");
		if (fc.browseForFileToSave(true))
			editFile = fc.getResult();
		else
			return;
	}

	selectionManager.deselectAll();
	editComponent = nullptr;

	if (editFile.existsAsFile())
		edit = te::loadEditFromFile(engine, editFile);
	else
		edit = te::createEmptyEdit(engine, editFile);

	edit->playInStopEnabled = true;

	auto& transport = edit->getTransport();
	transport.addChangeListener(this);

	editNameLabel.setText(editFile.getFileNameWithoutExtension(), dontSendNotification);

	te::EditFileOperations(*edit).save(true, true, false);

	enableAllInputs();

	editComponent = std::make_unique<EditComponent>(*edit, selectionManager);
	editComponent->getEditViewState().showFooters = true;
	editComponent->getEditViewState().showMidiDevices = true;
	editComponent->getEditViewState().showWaveDevices = true;

	addAndMakeVisible(*editComponent);
    resized();
}

void MainComponent::enableAllInputs()
{
	auto& dm = engine.getDeviceManager();

	for (int i = 0; i < dm.getNumMidiInDevices(); i++)
	{
		if (auto mip = dm.getMidiInDevice(i))
		{
			mip->setEndToEndEnabled(true);
			mip->setEnabled(true);
		}
	}

	for (int i = 0; i < dm.getNumWaveInDevices(); i++)
		if (auto wip = dm.getWaveInDevice(i))
			wip->setStereoPair(false);

	for (int i = 0; i < dm.getNumWaveInDevices(); i++)
	{
		if (auto wip = dm.getWaveInDevice(i))
		{
			wip->setEndToEnd(true);
			wip->setEnabled(true);
		}
	}

	edit->getTransport().ensureContextAllocated();
}

void MainComponent::changeListenerCallback(ChangeBroadcaster* source)
{
	if (edit != nullptr && source == &edit->getTransport())
	{
		updatePlayButtonText();
	}
	else if (source == &selectionManager)
	{
		auto sel = selectionManager.getSelectedObject(0);
		deleteButton.setEnabled(dynamic_cast<te::Clip*> (sel) != nullptr
			|| dynamic_cast<te::Track*> (sel) != nullptr
			|| dynamic_cast<te::Plugin*> (sel));
	}
}

void MainComponent::setFile(const File& f)
{
    auto sel = selectionManager.getSelectedObject(0);
    auto track = dynamic_cast<te::AudioTrack*> (sel);
    auto trackNum = track ? track->getAudioTrackNumber() : 0;
	if (auto clip = EngineHelpers::loadAudioFileAsClip(*edit, f, trackNum))
	{
		clip->setAutoTempo(false);
		clip->setAutoPitch(false);
		clip->setTimeStretchMode(te::TimeStretcher::defaultMode);
	}
}

void MainComponent::deleteSelectedObj ()
{
    auto sel = selectionManager.getSelectedObject(0);
    if (auto clip = dynamic_cast<te::Clip*> (sel))
    {
        clip->removeFromParentTrack();
    }
    else if (auto track = dynamic_cast<te::Track*> (sel))
    {
        if (!(track->isMarkerTrack() || track->isTempoTrack() || track->isChordTrack()))
            edit->deleteTrack(track);
    }
    else if (auto plugin = dynamic_cast<te::Plugin*> (sel))
    {
        plugin->deleteFromParent();
    }
}

void MainComponent::saveAsWav ()
{
    FileChooser chooser{ "enter file name to render..."
        , File::getSpecialLocation(File::userDesktopDirectory)
        , engine.getAudioFileFormatManager().readFormatManager.getWildcardForAllFormats()};
    if (chooser.browseForFileToSave(true))
    {
        File file = chooser.getResult();
        BigInteger tracksToDo;
        int trackID = 0;
        for (const auto& t : te::getAllTracks(*edit))
            tracksToDo.setBit(trackID++);

        te::EditTimeRange edrange{ 0.0, edit->getLength() };

        if (te::Renderer::renderToFile("My Render Task", file, *edit, edrange, tracksToDo, true, {}, false))
            DBG("render successed!!");
        else
            DBG("render failed...");
    }
}

void MainComponent::showPlugins ()
{
    DialogWindow::LaunchOptions o;
    o.dialogTitle = TRANS("Plugins");
    o.dialogBackgroundColour = Colours::black;
    o.escapeKeyTriggersCloseButton = true;
    o.useNativeTitleBar = true;
    o.resizable = true;
    o.useBottomRightCornerResizer = true;

    auto v = new PluginListComponent(engine.getPluginManager().pluginFormatManager,
        engine.getPluginManager().knownPluginList,
        engine.getTemporaryFileManager().getTempFile("PluginScanDeadMansPedal"),
        te::getApplicationSettings());
    v->setSize(800, 600);
    o.content.setOwned(v);
    o.launchAsync();
}

void MainComponent::newEdit ()
{
    createOrLoadEdit();

    auto xmlName = File::getSpecialLocation(File::userDesktopDirectory).getFullPathName() + "\\edit.xml";
    //DBG(xmlName);
    File xmlFile(xmlName);

    auto state = edit->state;
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    xml->writeTo(xmlFile);
}
