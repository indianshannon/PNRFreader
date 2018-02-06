READING PNRF FILES USING C++ & R, for further processing in R
=============================================================

The folder is self-contained and complete, doesn't even need the PNRF reader toolkit 
(necessary .dll .lib .olb files are copied) to be installed on the system.
(I have taken the essential .dll .lib .olb files and copied locally)

The overall project has two stages as explained below. The proposed workflow is that 
we compile the C++ code once-for-lifetime, obtain the binary, and use it from within 
R all the time to read various PNRF files.

Step-1. Visual Studio: (once for life, to generate a binary)
======================

We generate a C++ compiled binary using Visual Studio (simply double-click PNRFreader.vcxproj and press F5). 
The actual C++ source file is PNRFreader.cpp, where every other file in the project is only to support it.

The binary resulting from the compilation will be capable of extracting the Direct Current data from the 
Channel-3 of any arbitrary PNRF file supplied as the command-line argument. However, one need not leave 
his R environment for this purpose - we may use the R-script below to call this binary internally.

On a default run the project uses the locally available VT690.pnrf file and store the results (extracted 
data of PNRF file) in a two-column table (time and direct-current) in a plain-text (.txt) file of the 
same file-name except for a suffix and .txt extension.

Step-2. R studio:  (all the while, internally calling the above binary)
=================

We simply have an R-script fundamentally containing a system() call to internally run the above binary 
and produce a text file for subsequent reading into a local R variable using the infamous read.table(). 
The script is named: plot_RMS_current_from_PNRF.R and is made available in the Debug folder.

The given 90MB pnrf file (VT609.pnrf) generated an easily readable 200MB plain-text file 
(VT609-Channel-3.txt) which can then be read into any local variable of R amenable for further processing.


Aside:
I experienced a serious issue with the provided files, which wasted a vast majority of the last two days. The installation of the
PNRF reader toolkit seems to have issues, at least with Visual Studio. The toolkit lacks .lib files of the corresponding percPNRFLoader.dll and percRecordingInterface.olb 
files and I failed to compile until I tweaked and generated my own .lib files and included in the project. The rest of the project has been a cakewalk.

Aside2:
One may think that using C++ code directly from R, instead of a compiled binary. However, official FAQ confirms that R and Visual Studio are incompatible, and PNRF Toolkit seems to require Visual Studio.
Using the binaries, as shown above, is my current workaround.
