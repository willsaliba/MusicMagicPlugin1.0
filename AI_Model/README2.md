# INFORMATION TO ENABLE PLUGIN REQUESTS 

----- INITIAL SET UP -----
(only do this the first time activating the model)

first ensure python & miniconda/anaconda installed

//RUN FOLLOWING COMMANDS:
conda create --name riffusion python=3.9
conda activate riffusion
conda install -c conda-forge ffmpeg
python -m pip install -r requirements_all.txt
python3 setup.py install

//UPDATE FILE PATHS IN
plugin_requests.py (line 13 & 16)
scripts/generate.py (line 15)
scripts/inpaint.py (line 20)


----- TO ACTIVATE MODEL TO RECIEVE REQUESTS -----
(do this everytime using the model)

//RUN FOLLOWING COMMANDS:
conda activate riffusion
python3 riffusion/server.py


----- DEACTIVATING MODEL -----
(do this everytime you finish using the model)

when finished using plugin, perform CRTL+C in terminal
