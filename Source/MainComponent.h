#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class ListBoxExample_Base : public Component, private ListBoxModel, public SettableTooltipClient
{
public:
	ListBoxExample_Base()
	{
		m_listbox.setModel(this);
		addAndMakeVisible(m_listbox);
	}
	
protected:
	ListBox m_listbox;
	int getNumRows() override
	{
		return 100;
	}
	void paintListBoxItem(int rowNumber, Graphics & g, int width, int height, bool rowIsSelected) override
	{
		if (rowIsSelected)
		{
			g.fillAll(Colours::blue);
		}
		else
		{
			g.fillAll(Colours::black);
		}
		g.setColour(Colours::white);
		g.drawText(String(rowNumber), 0, 0, width, height, Justification::centredLeft);
	}
	void resized() override
	{
		m_listbox.setBounds(0, 0, getWidth(), getHeight());
	}
};

class ListBoxExample_Broadcaster;

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
	ListenerList<ListBoxExampleListener> m_listeners;
	
	void listBoxItemDoubleClicked(int row, const MouseEvent&) override
	{
		sendMessagesToListeners(row, true);
	}
	void listBoxItemClicked(int row, const MouseEvent&) override
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

class ListBoxExample_StdFunction : public ListBoxExample_Base
{
public:
	ListBoxExample_StdFunction() : ListBoxExample_Base()
	{

	}
	std::function<void(ListBoxExample_StdFunction*, int, bool)> onRowSelected;
private:
	void listBoxItemDoubleClicked(int row, const MouseEvent&) override
	{
		if (onRowSelected) // check that the callback function has been set
			onRowSelected(this, row, true);
	}
	void listBoxItemClicked(int row, const MouseEvent&) override
	{
		if (onRowSelected) // check that the callback function has been set
			onRowSelected(this, row, false);
	}
};

template<typename... Ts>
inline var make_var_array(Ts... values)
{
	return Array<var>(values...);
}

#if JUCE_CXX17_IS_AVAILABLE
template<typename... Ts>
inline void values_from_var_array(var src, Ts&... values)
{
	jassert(src.isArray());
	jassert(src.size() >= sizeof...(values));
	int i = 0;
	auto f = [&i,&src](auto& x) 
	{ 
		x = src[i]; 
		++i; 
	};
	(f(values), ...);
}

template<typename... Ts>
inline std::tuple<Ts...> tuple_from_var_array(var src)
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

class ListBoxExample_Value : public ListBoxExample_Base
{
public:
	ListBoxExample_Value() : ListBoxExample_Base()
	{}
	void listBoxItemDoubleClicked(int row, const MouseEvent&) override
	{
		listValue = make_var_array(getName(), row, true);
	}
	void listBoxItemClicked(int row, const MouseEvent&) override
	{
		listValue = make_var_array(getName(), row, false);
	}
	Value listValue;
};

class MainComponent;

class ListBoxExample_PassOwner : public ListBoxExample_Base
{
public:
	ListBoxExample_PassOwner(MainComponent& mc) : ListBoxExample_Base(), m_maincomponent(mc) 
	{}
	// Because of forward declaration for MainComponent, the implementations need to go into the .cpp file
	void listBoxItemDoubleClicked(int row, const MouseEvent&) override;
	void listBoxItemClicked(int row, const MouseEvent&) override;
private:
	MainComponent& m_maincomponent;
};

class ListBoxExample_ChangeBroadcaster : public ListBoxExample_Base, public ChangeBroadcaster
{
public:
	ListBoxExample_ChangeBroadcaster() : ListBoxExample_Base()
	{
		
	}
	void listBoxItemDoubleClicked(int row, const MouseEvent&) override
	{
		wasDoubleClicked = true;
		selectedRow = row;
		sendChangeMessage();
	}
	void listBoxItemClicked(int row, const MouseEvent&) override
	{
		wasDoubleClicked = false;
		selectedRow = row;
		sendChangeMessage();
	}
	int selectedRow = -1;
	bool wasDoubleClicked = false;
};

class MainComponent   : public Component, 
	public ListBoxExampleListener,
	public Value::Listener,
	public ChangeListener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
	void tableRowChanged(ListBoxExample_PassOwner* sender, int row, bool wasDoubleClicked);
	void changeListenerCallback(ChangeBroadcaster* cb) override;
private:
    void listBoxRowSelected(ListBoxExample_Broadcaster* sender, int whichrow, bool wasDoubleClicked) override;
	void valueChanged(Value& value) override;
	ListBoxExample_Broadcaster m_table_broadcaster1, m_table_broadcaster2;
	ListBoxExample_StdFunction m_tablestdfunction1, m_tablestdfunction2;
	ListBoxExample_Value m_tablevalue1, m_tablevalue2;
	ListBoxExample_PassOwner m_table_passowner1, m_table_passowner2;
	ListBoxExample_ChangeBroadcaster m_table_changebroadcaster1, m_table_changebroadcaster2;
	Slider m_test1, m_test2, m_test3;
	Label m_infolabel;
	void handleListBoxEvent(String name, int row, bool wasDoubleClicked);
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
