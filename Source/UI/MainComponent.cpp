#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
	settingsButton.onClick = [this] { EngineHelpers::showAudioDeviceSettings(engine); };
	pluginsButton.onClick = [this]
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
	};
	newEditButton.onClick = [&]
	{
		createOrLoadEdit();

		auto xmlName = File::getSpecialLocation(File::userDesktopDirectory).getFullPathName() + "\\edit.xml";
		//DBG(xmlName);
		File xmlFile(xmlName);

		auto state = edit->state;
		std::unique_ptr<juce::XmlElement> xml(state.createXml());
		xml->writeTo(xmlFile);
	};

	updatePlayButtonText();
	editNameLabel.setJustificationType(Justification::centred);
	Helpers::addAndMakeVisible(*this, { &settingsButton, &saveButton, &undoButton, &redoButton, &pluginsButton, &newTrackButton, &newAudioClipButton, &playPauseButton,
										 &newEditButton, &showEditButton, &deleteButton, &editNameLabel, &showWaveformButton });

	deleteButton.setEnabled(false);

	auto d = File::getSpecialLocation(File::tempDirectory).getChildFile("PluginDemo");
	d.createDirectory();

	auto f = Helpers::findRecentEdit(d);
	if (f.existsAsFile())
		createOrLoadEdit(f);
	else
		createOrLoadEdit(d.getNonexistentChildFile("Test", ".wav", false));

	selectionManager.addChangeListener(this);

	setupButtons();

	setSize(600, 400);
}

MainComponent::~MainComponent()
{
	te::EditFileOperations(*edit).save(true, true, false);
	engine.getTemporaryFileManager().getTempDirectory().deleteRecursively();
}

//==============================================================================
void MainComponent::paint(juce::Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{

	auto r = getLocalBounds();
	int w = r.getWidth() / 12;
	auto topR = r.removeFromTop(32);


	settingsButton.setBounds(topR.removeFromLeft(w).reduced(2));
	saveButton.setBounds(topR.removeFromLeft(w).reduced(2));
	undoButton.setBounds(topR.removeFromLeft(w).reduced(2));
	redoButton.setBounds(topR.removeFromLeft(w).reduced(2));
	pluginsButton.setBounds(topR.removeFromLeft(w).reduced(2));
	newTrackButton.setBounds(topR.removeFromLeft(w).reduced(2));
	newAudioClipButton.setBounds(topR.removeFromLeft(w).reduced(2));
	playPauseButton.setBounds(topR.removeFromLeft(w).reduced(2));
	newEditButton.setBounds(topR.removeFromLeft(w).reduced(2));
	showEditButton.setBounds(topR.removeFromLeft(w).reduced(2));
	deleteButton.setBounds(topR.removeFromLeft(w).reduced(2));
	showWaveformButton.setBounds(topR.removeFromLeft(w).reduced(2));
	topR = r.removeFromTop(32);
	editNameLabel.setBounds(topR);



	if (editComponent != nullptr)
		editComponent->setBounds(r);
}

void  MainComponent::setupButtons()
{
	saveButton.onClick = [this]
	{
		te::EditFileOperations(*edit).save(true, true, false);
		engine.getTemporaryFileManager().getTempDirectory().deleteRecursively();
	};
	undoButton.onClick = [this]
	{
		edit->undo();
	};
	redoButton.onClick = [this]
	{
		edit->redo();
	};
	playPauseButton.onClick = [this]
	{
		EngineHelpers::togglePlay(*edit);
	};
	newTrackButton.onClick = [this]
	{
		edit->ensureNumberOfAudioTracks(getAudioTracks(*edit).size() + 1);
	};
	newAudioClipButton.onClick = [this]
	{
		EngineHelpers::browseForAudioFile(engine, [this](const File& f) { setFile(f); });
	};
	deleteButton.onClick = [this]
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
		FileChooser fc("New Edit", File::getSpecialLocation(File::userDocumentsDirectory), "*.wav");
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
	showEditButton.onClick = [this, editFile]
	{
		te::EditFileOperations(*edit).save(true, true, false);
		editFile.revealToUser();
	};

	te::EditFileOperations(*edit).save(true, true, false);

	enableAllInputs();

	editComponent = std::make_unique<EditComponent>(*edit, selectionManager);
	editComponent->getEditViewState().showFooters = true;
	editComponent->getEditViewState().showMidiDevices = true;
	editComponent->getEditViewState().showWaveDevices = true;

	addAndMakeVisible(*editComponent);
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
	if (auto clip = EngineHelpers::loadAudioFileAsClip(*edit, f))
	{
		clip->setAutoTempo(false);
		clip->setAutoPitch(false);
		clip->setTimeStretchMode(te::TimeStretcher::defaultMode);
		auto sel = selectionManager.getSelectedObject(0);
		if (auto track = dynamic_cast<te::AudioTrack*> (sel))
		{
			track->addClip(clip);
		}
	}
}
