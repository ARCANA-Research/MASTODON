import os
import sys
import time as Time


#Open configuration file
config_filename = "../../executable/Single_Node/test.config"
config_data = {}
with open(config_filename, 'r') as f:
    for line in f:
        if line[0:2] != "//" and line.strip() != "":
            key, value = line.strip().split('=')
            config_data[key.strip()] = value.strip()

#Store the parameters from the config_data dictionary
file_name = config_data['verilog_filename']
memorisation = config_data['memorisation']
no_of_bitlines  = int(config_data['num_col']) + int(config_data['num_imm'])
no_of_wordlines = int(config_data['num_row'])
V_MAGIC = float(config_data['volt_MAGIC'])
V_SET = float(config_data['volt_SET'])
V_RESET = float(config_data['volt_RESET'])
V_ISOLATE_BL = float(config_data['volt_ISO_BL'])
V_ISOLATE_WL = float(config_data['volt_ISO_WL'])
cycle_time = float(config_data['second_cycle_time'])
step_size = float(config_data['second_step_size'])
R = float(config_data['ohm_R'])
C = float(config_data['farad_C'])
state_threshold = float(config_data['state_threshold'])

if memorisation == "false":
    #Check if Device_Model_Class.py exists and if not, use the parser to create it
    if not(os.path.isfile(exec_path + "/Device_Model_Class.py")):
        import verilogA_parser
        verilogA_parser.parse(exec_path + "/device_models/" + file_name, exec_path)

    #Import the tile wrapper module
    from tile_Wrapper import *

    #Initialize a tile object with given parameters
    T1 = tile_Wrapper(int(sys.argv[2]), no_of_wordlines, no_of_bitlines, R, C, cycle_time, step_size, exec_path)

    #Performing NOR operation
    if sys.argv[1] == "NOR":
        desired_state = ""
        for r in range(no_of_wordlines):
            desired_state = desired_state + "1"
        T1.simulate_WRITE_ALL(int(sys.argv[5]), desired_state, [V_SET, V_RESET], V_ISOLATE_BL, V_ISOLATE_WL, False)
        T1.store(int(sys.argv[2]),state_threshold)
        start_time = Time.time()
        T1.simulate_LOGIC(int(sys.argv[3]), int(sys.argv[4]), int(sys.argv[5]), V_MAGIC, V_ISOLATE_BL, V_ISOLATE_WL)
        T1.store(int(sys.argv[2]),state_threshold)

    #Performing WRITE operation
    if sys.argv[1] == "WRITE" or sys.argv[1] == "INIT":
        start_time = Time.time()
        T1.simulate_WRITE_ALL(int(sys.argv[3]), str(sys.argv[4]),  [V_SET, V_RESET], V_ISOLATE_BL, V_ISOLATE_WL, False)
        end_time = Time.time()
        T1.store(int(sys.argv[2]), state_threshold)

    #Performing SET operation
    elif sys.argv[1] == "SET":
        start_time = Time.time()
        T1.simulate_WRITE(int(sys.argv[3]), int(sys.argv[4]), V_SET, V_ISOLATE_BL, V_ISOLATE_WL, "SET")
        end_time = Time.time()
        T1.store(int(sys.argv[2]),state_threshold)

    #Performing RESET operation
    elif sys.argv[1] == "RESET":
        start_time = Time.time()
        T1.simulate_WRITE(int(sys.argv[3]), int(sys.argv[4]), V_RESET, V_ISOLATE_BL, V_ISOLATE_WL, "RESET")
        end_time = Time.time()
        T1.store(int(sys.argv[2]),state_threshold)
        
    #Performing READ operation
    elif sys.argv[1] == "READ":
        T1.store(int(sys.argv[2]),state_threshold)


