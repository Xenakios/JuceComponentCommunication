#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

/*
Base class that has the ListBox component and implements drawing some bogus data into the listbox.
*/
class ListBoxExample_Base : public juce::Component, private juce::ListBoxModel
{
public:
	ListBoxExample_Base()
	{
		m_listbox.setModel(this);
		addAndMakeVisible(m_listbox);
	}
	
protected:
	juce::ListBox m_listbox;
	int getNumRows() override
	{
		return 100;
	}
	void paintListBoxItem(int rowNumber, juce::Graphics & g, int width, int height, bool rowIsSelected) override
	{
		if (rowIsSelected)
		{
			g.fillAll(juce::Colours::blue);
		}
		else
		{
			g.fillAll(juce::Colours::black);
		}
		g.setColour(juce::Colours::white);
		g.drawText(juce::String(rowNumber), 0, 0, width, height, juce::Justification::centredLeft);
	}
	void resized() override
	{
		m_listbox.setBounds(0, 0, getWidth(), getHeight());
	}
};

/*
A complicated way. A pair of broadcaster/listener classes is used like with the many JUCE provided
Components. The benefit to this approach is that many listeners can be listening at once and some
safety is provided thanks to the ListenerList.
*/

class ListBoxExample_Broadcaster;

/* 
Classes wanting to listen for the ListBoxExample_Broadcaster need to subclass this and implement
the listBoxRowSelected method.
*/
class ListBoxExampleListener
{
public:
	virtual ~ListBoxExampleListener() {}
	virtual void listBoxRowSelected(ListBoxExample_Broadcaster* sender, int whichrow, bool wasDoubleClicked) = 0;
};

class ListBoxExample_Broadcaster : public ListBoxExample_Base
{
public:
	ListBoxExample_Broadcaster() : ListBoxExample_Base()
	{
	}
	void addListener(ListBoxExampleListener* l) { m_listeners.add(l); }
	void removeListener(ListBoxExampleListener* l) { m_listeners.remove(l); }
private:
	juce::ListenerList<ListBoxExampleListener> m_listeners;
	void listBoxItemDoubleClicked(int row, const juce::MouseEvent&) override
	{
		sendMessagesToListeners(row, true);
	}
	void listBoxItemClicked(int row, const juce::MouseEvent&) override
	{
		sendMessagesToListeners(row, false);
	}
	void sendMessagesToListeners(int row, bool wasDoubleClicked)
	{
		auto callback = [this, row, wasDoubleClicked](ListBoxExampleListener& l)
		{
			l.listBoxRowSelected(this, row, wasDoubleClicked);
		};
		m_listeners.call(callback);
	}
};

/*
std::functions can only notify one listener but are a pretty good simple solution to the problem.
The interested object can just assign a C++11 lambda on the onRowSelected member variable.
There are possible object lifetime issues with this if the lambda captures a reference or a pointer 
to an object that may be destroyed before this listbox implementation has done its callback.
*/
class ListBoxExample_StdFunction : public ListBoxExample_Base
{
public:
	ListBoxExample_StdFunction() : ListBoxExample_Base()
	{

	}
	std::function<void(ListBoxExample_StdFunction*, int, bool)> onRowSelected;
private:
	void listBoxItemDoubleClicked(int row, const juce::MouseEvent&) override
	{
		if (onRowSelected) // check that the callback function has been set
			onRowSelected(this, row, true);
	}
	void listBoxItemClicked(int row, const juce::MouseEvent&) override
	{
		if (onRowSelected) // check that the callback function has been set
			onRowSelected(this, row, false);
	}
};

/*
In order to hold multiple pieces of data in a Juce Value, Array<var> needs to be used.
Here are some helper functions to deal with that.
*/

/*
Makes an Array<var> from multiple values. The Ts types need to be compatible with Juce var.
Things like Juce String, int, double, bool will work.
*/ 
template<typename... Ts>
inline juce::var make_var_array(Ts... values)
{
	return juce::Array<juce::var>(values...);
}

#if JUCE_CXX17_IS_AVAILABLE
/*
Makes an std::tuple out of a Juce var that holds an array. Handy when C++17 structured bindings are available.
Otherwise the array elements need to be manually gotten out of the var instance.
*/
template<typename... Ts>
inline std::tuple<Ts...> tuple_from_var_array(juce::var src)
{
	// Check source is a var containing an array and that the size matches with the number of types requested
	jassert(src.isArray() && src.size() == sizeof...(Ts));
	std::tuple<Ts...> result;
	int i = 0;
	std::apply([&i,&src](auto&&... xs) 
	{
		([src, &i](auto& x) { x = src[i]; ++i; }(xs), ...);
	},result);
	return result;
}
#endif

/*
Juce's Value objects can be listened for changes but they unfortunately don't directly support
passing multiple variables of interest. They can however hold an Array<var> and that can be (ab)used
for our purposes.
*/
class ListBoxExample_Value : public ListBoxExample_Base
{
public:
	ListBoxExample_Value() : ListBoxExample_Base()
	{}
	void listBoxItemDoubleClicked(int row, const juce::MouseEvent&) override
	{
		listValue = make_var_array(getName(), row, true);
	}
	void listBoxItemClicked(int row, const juce::MouseEvent&) override
	{
		listValue = make_var_array(getName(), row, false);
	}
	juce::Value listValue;
};

/*
Classic lazy way which couples the classes together. Only a MainComponent can receive the notifications this way.
Methods that need to use the owner object need to go into a separate .cpp file that needs to include this header file.
*/
class MainComponent; // forward declare here because the actual MainComponent declaration becomes later in the header

class ListBoxExample_PassOwner : public ListBoxExample_Base
{
public:
	ListBoxExample_PassOwner(MainComponent& mc) : ListBoxExample_Base(), m_maincomponent(mc) 
	{}
	// Because of forward declaration for MainComponent, the implementations need to go into the .cpp file
	void listBoxItemDoubleClicked(int row, const juce::MouseEvent&) override;
	void listBoxItemClicked(int row, const juce::MouseEvent&) override;
private:
	MainComponent& m_maincomponent;
};

/*
Kind of icky implementation. ChangeBroadcaster is used to do the notifications that something happened.
The listener class can get the needed data by dynamic_casting the ChangeBroadcaster pointer
into a pointer to an object of this class so that the wasDoubleClicked and selectedRow member variables can be read.
Or, the changeListenerCallback implementation can check if the passed pointer is equal to some component's address and
dispatch as required.
*/
class ListBoxExample_ChangeBroadcaster : public ListBoxExample_Base, public juce::ChangeBroadcaster
{
public:
	ListBoxExample_ChangeBroadcaster() : ListBoxExample_Base()
	{
		
	}
	void listBoxItemDoubleClicked(int row, const juce::MouseEvent&) override
	{
		wasDoubleClicked = true;
		selectedRow = row;
		sendChangeMessage();
	}
	void listBoxItemClicked(int row, const juce::MouseEvent&) override
	{
		wasDoubleClicked = false;
		selectedRow = row;
		sendChangeMessage();
	}
	int selectedRow = -1;
	bool wasDoubleClicked = false;
};

class MainComponent   : public juce::Component,
	public ListBoxExampleListener,
	public juce::Value::Listener,
	public juce::ChangeListener
{
public:
    MainComponent();
    ~MainComponent();
	
	void paint (juce::Graphics&) override;
    void resized() override;
	
	// Handles callbacks from ChangeBroadcaster
	void changeListenerCallback(juce::ChangeBroadcaster* cb) override;
	// Handles callbacks from a ListBoxExample_Broadcaster
	void listBoxRowSelected(ListBoxExample_Broadcaster* sender, int whichrow, bool wasDoubleClicked) override;
	// Handles callbacks from a Value in the ListBoxExample_Value
	void valueChanged(juce::Value& value) override;
	// This can be directly called by ListBoxExample_PassOwner instances because they know about MainComponent
	void handleListBoxEvent(juce::String name, int row, bool wasDoubleClicked);
private:
	ListBoxExample_Broadcaster m_table_broadcaster1, m_table_broadcaster2;
	ListBoxExample_StdFunction m_tablestdfunction1, m_tablestdfunction2;
	ListBoxExample_Value m_tablevalue1, m_tablevalue2;
	ListBoxExample_PassOwner m_table_passowner1, m_table_passowner2;
	ListBoxExample_ChangeBroadcaster m_table_changebroadcaster1, m_table_changebroadcaster2;
	juce::Label m_infolabel;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
