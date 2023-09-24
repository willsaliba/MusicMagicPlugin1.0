# PROJECT DESCRIPTION

This audio plugin is capable of four modes of generation:
- Extending audio clips
- Replacing audio clips
- In-filling audio clips
- Generating audio clips from scratch

Each of these modes have their own set of controls allowing you to fine tune the requests.
Since the plugin is built as a VST3 it can be opened in any digital audio workstation (DAW),
and inputs may be any waveform format (mp3, wav, aif).

Once the plugin makes a request, our fine-tuned model handles the request & produces the desired spectrogram 
which is then converted to an mp3 file and returned to the plugin which the user can then use in their DAW

Generation times will depend on audio clips lengths & your computers architecture.

# INSTRUCTIONS TO USE MUSIC MAGIC PLUGIN

-- ENSURE AI_MODEL IS SETUP & ACTIVE --  
For instructions see README2.md in the AI_Model folder

-- ENSURE PLUGIN IS COMPLIED AS VST3 --  
For instructions see README.md in the Plugin folder

-- THEN --  
Open the plugin in your desired DAW & start generating!

# TECHNICAL DETAILS

-- PLUGIN --  
The plugin is built in C++ using the JUCE framework suited for creating audio applications.
It makes requests to the model by calling the AI_Model/plugin_requests.py file and passing parameters in the command line

-- FINE-TUNED MODEL --  
Our model is based on the Riffusion model, which itself is an altered version of Stable Diffusion model (image generator), and 
creates music by generating spectrograms, which are image representations of audio.

Our model uses the parameters from the plugin to provide the user with a high level of control in the way music is generated
by the Riffusion model