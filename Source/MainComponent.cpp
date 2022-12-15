/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"
#include <array>

//==============================================================================
MainComponent::MainComponent() : 
	m_table_passowner1(*this),
	m_table_passowner2(*this)
{
	addAndMakeVisible(m_table_broadcaster1);
	m_table_broadcaster1.addListener(this);
	// Set component names for the involved components so can show human readable sender object names
	// when the callbacks happen
	m_table_broadcaster1.setName("broadcasterlistbox1"); 
	addAndMakeVisible(m_table_broadcaster2);
	m_table_broadcaster2.addListener(this);
	m_table_broadcaster2.setName("broadcasterlistbox2");
	
	// Callback used for ListBoxExample_StdFunction instances
	auto callback = [this](ListBoxExample_StdFunction* sender, int whichrow, bool wasDoubleClicked)
	{
		handleListBoxEvent(sender->getName(), whichrow, wasDoubleClicked);
	};
	
	addAndMakeVisible(m_tablestdfunction1);
	m_tablestdfunction1.onRowSelected = callback; // set the callback
	m_tablestdfunction1.setName("std::function_listbox1");
	addAndMakeVisible(m_tablestdfunction2);
	m_tablestdfunction2.onRowSelected = callback; // set the callback
	m_tablestdfunction2.setName("std::function_listbox2");

	addAndMakeVisible(m_tablevalue1);
	m_tablevalue1.listValue.addListener(this);
	m_tablevalue1.setName("valuelistbox1");
	addAndMakeVisible(m_tablevalue2);
	m_tablevalue2.listValue.addListener(this);
	m_tablevalue2.setName("valuelistbox2");

	addAndMakeVisible(m_table_passowner1);
	m_table_passowner1.setName("passowner_listbox1");
	addAndMakeVisible(m_table_passowner2);
	m_table_passowner2.setName("passowner_listbox2");

	addAndMakeVisible(m_table_changebroadcaster1);
	m_table_changebroadcaster1.addChangeListener(this);
	m_table_changebroadcaster1.setName("changebroadcaster_listbox1");
	addAndMakeVisible(m_table_changebroadcaster2);
	m_table_changebroadcaster2.addChangeListener(this);
	m_table_changebroadcaster2.setName("changebroadcaster_listbox2");

	addAndMakeVisible(m_infolabel);

	setSize(1000, 400);
}

MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
	// Just do some (bad) layouting for the listboxes
	juce::FlexBox flex(juce::FlexBox::Direction::row,
		juce::FlexBox::Wrap::wrap, juce::FlexBox::AlignContent::center, juce::FlexBox::AlignItems::stretch, juce::FlexBox::JustifyContent::flexStart);
	flex.items.add(juce::FlexItem(100.0, 300.0, m_table_broadcaster1));
	flex.items.add(juce::FlexItem(100.0, 100.0, m_table_broadcaster2));
	flex.items.add(juce::FlexItem(100.0, 100.0, m_tablestdfunction1));
	flex.items.add(juce::FlexItem(100.0, 100.0, m_tablestdfunction2));
	flex.items.add(juce::FlexItem(100.0, 100.0, m_tablevalue1));
	flex.items.add(juce::FlexItem(100.0, 100.0, m_tablevalue2));
	flex.items.add(juce::FlexItem(100.0, 100.0, m_table_passowner1));
	flex.items.add(juce::FlexItem(100.0, 100.0, m_table_passowner2));
	flex.items.add(juce::FlexItem(100.0, 100.0, m_table_changebroadcaster1));
	flex.items.add(juce::FlexItem(100.0, 100.0, m_table_changebroadcaster2));
	flex.performLayout(getLocalBounds());
	m_infolabel.setBounds(0, getHeight() - 30, getWidth(), 29);
}

void MainComponent::changeListenerCallback(juce::ChangeBroadcaster * cb)
{
	ListBoxExample_ChangeBroadcaster* listbox = dynamic_cast<ListBoxExample_ChangeBroadcaster*>(cb);
	if (listbox != nullptr) // Check that the ChangeBroadcaster is our listbox thing
	{
		handleListBoxEvent(listbox->getName(), listbox->selectedRow, listbox->wasDoubleClicked);
	}
	// Alternative implementation, but hmm, no, this really should not be used.
	/*
	if (cb == &m_table_changebroadcaster1)
		handleListBoxEvent(m_table_changebroadcaster1.getName(), m_table_changebroadcaster1.selectedRow, m_table_changebroadcaster1.wasDoubleClicked);
	if (cb == &m_table_changebroadcaster2)
		handleListBoxEvent(m_table_changebroadcaster2.getName(), m_table_changebroadcaster2.selectedRow, m_table_changebroadcaster2.wasDoubleClicked);
	*/
}

void MainComponent::listBoxRowSelected(ListBoxExample_Broadcaster* sender, int whichrow, bool wasDoubleClicked)
{
	handleListBoxEvent(sender->getName(), whichrow, wasDoubleClicked);
}

void MainComponent::valueChanged(juce::Value & value)
{
#if JUCE_CXX17_IS_AVAILABLE
	auto[senderName, row, doubleClicked] = tuple_from_var_array<juce::String, int, bool>(value);
#else
	// If can't use c++17 structured bindings, need to get the elements out individually like this from the Value
	juce::String senderName = value.getValue()[0];
	int row = value.getValue()[1];
	bool doubleClicked = value.getValue()[2];
#endif
	handleListBoxEvent(senderName, row, doubleClicked);
}

void MainComponent::handleListBoxEvent(juce::String senderName, int row, bool wasDoubleClicked)
{
	// Finally, we somehow got here with the needed infos
	if (wasDoubleClicked)
		m_infolabel.setText("Row " + juce::String(row) + " was double clicked in : " + senderName, juce::dontSendNotification);
	else
		m_infolabel.setText("Row " + juce::String(row) + " was clicked in : " + senderName, juce::dontSendNotification);
}

void ListBoxExample_PassOwner::listBoxItemDoubleClicked(int row, const juce::MouseEvent &)
{
	m_maincomponent.handleListBoxEvent(getName(), row, true);
}

void ListBoxExample_PassOwner::listBoxItemClicked(int row, const juce::MouseEvent &)
{
	m_maincomponent.handleListBoxEvent(getName(), row, false);
}
