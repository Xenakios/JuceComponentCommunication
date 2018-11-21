# JuceComponentCommunication
Demonstrates various ways, excluding use of global variables, to pass information between Components in a JUCE based project. 
The information sending Component is a simple ListBox implementation and the other Component, the MainComponent is interested in knowing 
which row of the listbox was selected and if a mouse double click was involved.

Techniques included :

-Broadcaster/Listener

-std::function

-Value and Value::Listener

-Making the listbox a ChangeBroadcaster and using a dynamic_cast to get access into the listbox variables

-Passing a reference of the MainComponent into the listbox

-Polling with a Timer
