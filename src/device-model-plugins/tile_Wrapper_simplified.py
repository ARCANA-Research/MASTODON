import numpy as np
import xmltodict
import ast
import pickle

class tile_Wrapper_simplified():
    def __init__(self, tile_idx, no_of_wordlines, no_of_bitlines, R, C, cycle_time, step_size, exec_path):
        self.no_of_wordlines = no_of_wordlines
        self.no_of_bitlines = no_of_bitlines
        self.cycle_time = cycle_time
        self.step_size = step_size
        self.R = R
        self.C = C
        self.exec_path = exec_path

        #Store the tile_idx as the key to access different dictionaries
        self.key = 'tile_' + str(tile_idx)
        
        #Open states.xml to store the states of the new tile_idx
        state_file = open(self.exec_path+"/states.xml", "r+", encoding='utf-8')
        xml_string = state_file.read()
        state_binary_file = open(self.exec_path+"/states_binary.xml", "r+", encoding='utf-8')
        xml_binary_string = state_binary_file.read()

        if xml_string != "":
            all_states = xmltodict.parse(xml_string)
            all_states_binary = xmltodict.parse(xml_binary_string)
        else:
            all_states = {'all_states': {}} 
            all_states_binary = {'all_states': {}}

        if self.key not in all_states['all_states']:
            initial_states = np.random.randint(2, size=(self.no_of_wordlines,self.no_of_bitlines))
            all_states['all_states'][self.key] = {}
            for r in range(self.no_of_wordlines):
                row_states = ""
                for c in range(self.no_of_bitlines):
                    row_states = row_states + str(initial_states[r,c]) + ','
                all_states['all_states'][self.key]['row_'+str(r)] = row_states

            all_states_binary['all_states'][self.key] = {}
            for c in range(self.no_of_bitlines):
                col_states = ""
                for r in range(self.no_of_wordlines):
                    col_states = col_states + str(initial_states[r,c])
                all_states_binary['all_states'][self.key]['col_'+str(c)] = col_states

            state_file.seek(0)
            state_file.truncate()
            xmltodict.unparse(all_states,state_file)
            state_file.close()

            state_binary_file.seek(0)
            state_binary_file.truncate()
            xmltodict.unparse(all_states_binary,state_binary_file)
            state_binary_file.close()
        
    def simulate_LOGIC(self, input_1_addr, input_2_addr, output_addr, state_threshold):
        state_file = open(self.exec_path+"/states.xml", "r+", encoding='utf-8')
        xml_string = state_file.read()
        all_states = xmltodict.parse(xml_string)
        tile_states = all_states['all_states'][self.key]

        state_binary_file = open(self.exec_path+"/states_binary.xml", "r+", encoding='utf-8')
        xml_string = state_binary_file.read()
        all_states_binary = xmltodict.parse(xml_string)
        tile_states_binary = all_states_binary['all_states'][self.key]

        output_state_file = open(self.exec_path+"/output_states.xml", "r+", encoding='utf-8')
        xml_string = output_state_file.read()
        final_output_states = xmltodict.parse(xml_string)

        final_output_state_col_wise = ""

        for i in range(self.no_of_wordlines):
            row_states = tile_states['row_'+str(i)].split(',')
            if (row_states[-1] == ','):
                row_states.pop()
            initial_output_state = int(row_states[output_addr])
            input_1_state = round(float(row_states[input_1_addr]),1)
            input_2_state = round(float(row_states[input_2_addr]),1)
            # print("Input 1 state: " + str(input_1_state))
            # print("Input 2 state: " + str(input_2_state))
            
            no_of_LRS_iso_cell = 0
            for j in range(self.no_of_bitlines):
                state = tile_states_binary['col_' + str(j)][i]
                if j != input_1_addr and j != input_2_addr and j != output_addr:
                    if state == '1':
                        no_of_LRS_iso_cell += 1

            key2 = 'input_states_'+str(input_1_state)+'_'+str(input_2_state)
            key3 = 'output_state_'+str(initial_output_state)
            key4 = 'no_of_LRS_cells'+str(no_of_LRS_iso_cell)
            key5 = 'cycle_time_'+str(self.cycle_time)
            # print(key2)
            # print(key3)
            # print(key4)
            # print(key5)
            final_output_state = final_output_states['output_states'][key2][key3][key4][key5]
            # print(final_output_state)
            row_states[output_addr] = final_output_state
            all_states['all_states'][self.key]['row_'+str(i)] = ",".join(row_states)

            if float(final_output_state) > state_threshold:
                final_output_state_col_wise += '1'
            else:
                final_output_state_col_wise += '0'

        all_states_binary['all_states'][self.key]['col_'+str(output_addr)] = final_output_state_col_wise

        state_file.seek(0)
        state_file.truncate()
        xmltodict.unparse(all_states,state_file)
        state_file.close()

        state_binary_file.seek(0)
        state_binary_file.truncate()
        xmltodict.unparse(all_states_binary,state_binary_file)
        state_binary_file.close()
        
    def simulate_INIT(self, tile_idx, states_binary, states):
        state_file = open(self.exec_path+"/states.xml", "r+", encoding='utf-8')
        xml_string = state_file.read()
        all_states = xmltodict.parse(xml_string)

        all_states['all_states'][tile_idx] = states

        state_binary_file = open(self.exec_path+"/states_binary.xml", "r+", encoding='utf-8')
        xml_string = state_binary_file.read()
        all_states_binary = xmltodict.parse(xml_string)

        all_states_binary['all_states'][tile_idx] = states_binary

        state_file.seek(0)
        state_file.truncate()
        xmltodict.unparse(all_states,state_file)
        state_file.close()

        state_binary_file.seek(0)
        state_binary_file.truncate()
        xmltodict.unparse(all_states_binary,state_binary_file)
        state_binary_file.close()

    def simulate_WRITE(self, addr_col, addr_row, operation = "SET"):
        state_file = open(self.exec_path+"/states.xml", "r+", encoding='utf-8')
        xml_string = state_file.read()
        all_states = xmltodict.parse(xml_string)
        tile_states = all_states['all_states'][self.key]

        state_binary_file = open(self.exec_path+"/states_binary.xml", "r+", encoding='utf-8')
        xml_string = state_binary_file.read()
        all_states_binary = xmltodict.parse(xml_string)
        tile_states_binary = all_states_binary['all_states'][self.key]

        if operation == "SET":
            row_states = all_states['all_states'][self.key]['row_'+str(addr_row)].split(',')
            row_states[addr_col] = 1
            all_states['all_states'][self.key]['row_'+str(addr_row)] = ",".join(row_states)

            col_states = tile_states_binary['col_' + str(addr_col)]
            col_states[addr_row] = 1
            all_states_binary['all_states'][self.key]['col_' + str(addr_col)] = col_states
        elif operation == "RESET":
            row_states = all_states['all_states'][self.key]['row_'+str(addr_row)].split(',')
            row_states[addr_col] = 0
            all_states['all_states'][self.key]['row_'+str(addr_row)] = ",".join(row_states)

            col_states = tile_states_binary['col_' + str(addr_col)]
            col_states[addr_row] = 0
            all_states_binary['all_states'][self.key]['col_' + str(addr_col)] = col_states

        state_file.seek(0)
        state_file.truncate()
        xmltodict.unparse(all_states,state_file)
        state_file.close()

        state_binary_file.seek(0)
        state_binary_file.truncate()
        xmltodict.unparse(all_states_binary,state_binary_file)
        state_binary_file.close()


    def simulate_WRITE_ALL(self, addr, desired_state, row_parallel = False):
        state_file = open(self.exec_path+"/states.xml", "r+", encoding='utf-8')
        xml_string = state_file.read()
        all_states = xmltodict.parse(xml_string)

        state_binary_file = open(self.exec_path+"/states_binary.xml", "r+", encoding='utf-8')
        xml_string = state_binary_file.read()
        all_states_binary = xmltodict.parse(xml_string)

        if row_parallel == True:
            all_states['all_states'][self.key][addr] = desired_state
            for c in range(self.no_of_bitlines):
                all_states_binary['all_states'][self.key]['col_'+str(c)][addr] = desired_state[c]

        elif row_parallel == False:
            all_states_binary['all_states'][self.key]['col_'+str(addr)] = desired_state
            for r in range(self.no_of_wordlines):
                row_states = all_states['all_states'][self.key]['row_'+str(r)].split(',')
                row_states[addr] = desired_state[r]
                all_states['all_states'][self.key]['row_'+str(r)] = ",".join(row_states)
                # print(all_states['all_states'][self.key]['row_'+str(r)])

        state_file.seek(0)
        state_file.truncate()
        xmltodict.unparse(all_states,state_file)
        state_file.close()

        state_binary_file.seek(0)
        state_binary_file.truncate()
        xmltodict.unparse(all_states_binary,state_binary_file)
        state_binary_file.close()