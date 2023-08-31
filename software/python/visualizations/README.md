# How to run the Basler and Hamamatsu visualizations from source

## Installing Visualization Source Distribution

0. (First time only) Create a new python virtual environment for running the visualizations.

``` python -m venv C:\vizenv```
1. Activate the visualization virtual python environment you created.

```<drive>:\<virtual-environment-path>\Scripts\activate.bat```

where <drive> and <virtual-environment-path> are replaced by the drive and the path to the virtual environment you created in step 0.

2. Extract all files in visualizations.zip

3. Open a command prompt and change directory to the directory where the contents of the zip file were extracted.

4. ```pip install labgraph\<labgraph-wheel-name>```

where <labgraph-wheel-name> is replaced with the correct name of the "*.whl" file delivered in the "labgraph" directory.

5. ```pip install -r bci\example_requirements.txt```
 
If you get an error building "_yappi" extension, follow instructions under *"Installing Microsoft Visual C++ Build Tools"* now and then rerun this step.


# Installing Microsoft Visual C++ Build Tools

1. go to  https://visualstudio.microsoft.com/downloads/

2. Click top row button *"Free Visual Studio"*

3. Click *"Free Download"* under Visual Studio Community

4. Open downloaded file

5. Allow program to make changes to your computer

6. Under *"Desktop and Mobile"* select *"Desktop Development with C++"*

7. Click *"Install"*


# Running Visualizations

## Running Visualizations from the command line

First, make sure you have activated the python virtual environment you created for visualizations.

```<drive>:\<virtual-environment-name>\Scripts\activate.bat```

where <drive> and <virtual-environment-path> are replaced by the drive and the path to the virtual environment you created

**Run the visualizations from the directory where the visualizations.zip contents were extracted.**

Depending on the camera mode and the choice of tcp port, the full command for running the visualization software will likely look something like

```python -m bci.libraries.labgraph_viz.bassler_hamamatsu_viz --camera-mode basler```

The `-m` flag specifies that the script is to be run as a module.  The visualizations will not work without this flag.

`bci.libraries.labgraph_viz.bassler_hamamatsu_viz` is the relative path of the visualization module from the root directory of the visualizations folder.

## Choosing Camera Mode

The `--camera-mode basler` flag informs the visualization software that the message field types and the sizes of the heatmaps should be fitted for basler sized images.  To run the visualizations in hamamatsu mode, change this command to `--camera-mode hamamatsu`.

## Changing the TCP port

There is an optional flag that can be appended to change the tcp port where the visualizations receive incoming messages.  By default, the visualizations expect the messages to appear at `tcp://127.0.0.1:5060`.  However, in order to run both the visualizations for both basler and hamamatsu at the same time, one must ensure that each instance receives the messages at different ports.  This can be accomplished by including the flag `--read-addr tcp://127.0.0.1:5061`.  For instance, one could run the hamamatsu visualizations from messages coming through `tcp://127.0.0.1:5061` using the full command

```python -m bci.libraries.labgraph_viz.bassler_hamamatsu_viz --camera-mode hamamatsu --read-addr tcp://127.0.0.1:5061```

The ability to specify which port the messages are arriving at allows us to run the basler and hamamatsu visualizations in parallel.  Running the two commands

```python -m bci.libraries.labgraph_viz.bassler_hamamatsu_viz --camera-mode basler```

```python -m bci.libraries.labgraph_viz.bassler_hamamatsu_viz --camera-mode hamamatsu --read-addr tcp://127.0.0.1:5061```

concurrently allows for both visualization modes to run concurrently with no conflicts.

# Using the Visualizations

When the visualizations are run, two separate windows should pop up.  One consists of four panels displaying live visualizations for the running camera.  The other is a Region of Interest Selection Utility.

To activate a region of interest in the RoI popup, drag on one of the handles in the upper right or bottom left of the camera live feed.  This will activate (and resize) a region of interest.  The RoI can further be repositioned by clicking and dragging.  Associated updates in the visualizations can be observed upon RoI activation.  

In general, try not to activate any more RoI's than are needed for the experiment.  Calculating the contrast of each region is taxing, and activating too many might lead to performance or stability issues.

Two spinboxes can also be found in the visualization suite.  The one within the RoI popup dictates how long of a rolling average window to maintain.  The one within the main four-panel graph dictates how large of a rolling average window to maintain, as used in normalizing the values when computing the contrast.  Neither of these parameters should have a drastic impact on performance.

Finally, for the sake of stability, try not to run any intensive applications in parallel with the visualizations.  They are very computationally intensive programs that run with stability when run standalone, but using them in conjunction with other high or medium intensity programs, such as heavy duty IDE's or some browsers, might lead to performance issues.  So, avoid this when possible.