# Bottom Stack of MASTODON

### Setup
The `Configuration.config` file contains all the required parameters. The parameters required by the bottom stack are:
- num_col
- num_row
- num_imm
- verilog_filename
- volt_MAGIC
- volt_SET
- volt_ISO_BL
- volt_ISO_WL
- second_cycle_time
- step_size
- ohm_R
- farad_C
- state_threshold

The C++ file to run is `Python_to_C.cpp`. It defines `simulate_tile` function which takes in a single array arguement called argv of wchar_t type. argv array should contain 6 elements in the following order:
- Name of the Python script to execute ("Py_Tile.py")
- Type of operation ("NOR" or "READ")
- Tile index on which you want to perform the operation
- Column address for input 1 (ignore if operation is "READ")
- Column address for input 2 (ignore if operation is "READ")
- Column address for output (ignore if operation is "READ")

To build the `Python_to_C.cpp` file, run the follow command (assuming Python is installed in default location): 

`g++ Python_to_C.cpp -o Py_Tile  -L "C:\Program Files\Python312\libs" -lpython312 -I "C:\Program Files\Python312\include"` 

Note that while running the build command, you need to provide the location for Python linker files in order to compile the cpp file correctly. Once built, run the following command: 

`./Py_Tile.exe` 

to simulate the tile and perform the desired operation.

**IMPORTANT** Before running `./Py_Tile.exe`, ensure that you have created the following files and they are empty:
- `states.xml`
- `states_binary.xml`
- `tiles.pickle`