# INFORMATION TO ENABLE PLUGIN REQUESTS 

----- INITIAL SET UP -----  
(only do this the first time activating the plugin)  

- Ensure you have JUCE installed
- Once installed open Projucer and select the MM_Plugin.jucer file in the MM_Plugin folder
- Build the project in your respective OS (Mac, Windows, Linux)
- Compile project as standalone plugin (direct use) or as VST3 (for in DAW)


----- USING PLUGIN -----  
(do this everytime using the plugin)  

In your respective DAW navigate the the plugins folder to find MM_Plugin (some DAWs may require you to enable access to VST3).
Ensure AI_Model is active and start making requests!


----- Debugging -----  
(have not tested this plugin on other OS so may need to create new audio plugin)  

Ensure in the settings of the projucer project:  

    These plugin characteristics is set to true:  
    - Plugin is a Synth
    - Plugin MIDI input
    - Plugin MIDI Output
    - Plugin Editor Requires Keyboard Focus

    In preprocessor definitions you have:
    * JUCE_MODAL_LOOPS_PERMITTED=1 
