from tile import *
import numpy as np
import xmltodict
import ast
import pickle

class tile_Wrapper():
    def __init__(self, tile_idx, no_of_wordlines, no_of_bitlines, R, C, cycle_time, step_size, exec_path):
        self.no_of_wordlines = no_of_wordlines
        self.no_of_bitlines = no_of_bitlines
        self.cycle_time = cycle_time
        self.step_size = step_size
        self.R = R
        self.C = C
        self.tile_idx = tile_idx
        self.exec_path = exec_path

        #Store the tile_idx as the key to access different dictionaries
        self.key = 'tile_' + str(tile_idx)

        #Open tiles.pickle and try to load the file if it is not empty
        tiles = {}
        tiles_file = open(self.exec_path+'/tiles.pickle', 'rb+')
        try:
            tiles = pickle.load(tiles_file)
        except:
            pass

        #If tile_idx already exists in the tiles dictionary
        if bool(tiles) and self.key in tiles:
            #tiles.pickle is not empty and tile_idx exists in tile.obj
            self.tile = tiles[self.key]
        else:
            #either tiles.obj is empty or tile_idx does not exist in tile.obj
            #Open states.xml to store the states of the new tile_idx
            file = open(self.exec_path+"/states.xml", "r+", encoding='utf-8')
            xml_string = file.read()
            if xml_string != "":
                all_states = xmltodict.parse(xml_string)
            else:
                all_states = {'all_states': {}}
        
            initial_states = np.random.randint(2, size=(self.no_of_wordlines,self.no_of_bitlines))
            all_states['all_states'][self.key] = initial_states.tolist()

            file.seek(0)
            file.truncate()
            xmltodict.unparse(all_states,file)
            file.close()

            #Create a new tile object and store it in the tiles dictionary
            tiles[self.key] = tile(self.no_of_wordlines, self.no_of_bitlines, self.R, self.C, initial_states)
            self.tile  = tiles[self.key]

        #Dump the tiles dictionary into the tiles.pickle file
        tiles_file.seek(0)
        tiles_file.truncate
        pickle.dump(tiles, tiles_file)
        tiles_file.close()


    def store(self, tile_idx, state_threshold):
        #Store the tile_idx as the key to access different dictionaries
        self.key = "tile_" + str(tile_idx)

        #Create arrays to store the current states and also their binarized version
        self.current_states = np.zeros((self.no_of_wordlines,self.no_of_bitlines))
        self.current_states_binary = np.zeros((self.no_of_wordlines,self.no_of_bitlines))

        #Create the voltage arrays for bitline and wordline
        V_INPUT_BL = [0.1 for i in range(self.no_of_bitlines)]
        V_INPUT_WL = [0 for i in range(self.no_of_wordlines)]
        
        #Get the current states of all the cells in the tile
        tile_step_energy, self.current_states = self.tile.simulate(V_INPUT_BL, V_INPUT_WL, self.step_size, return_states = True)

        #Store the current states to the states.xml file
        state_file = open(self.exec_path+"/states.xml", "r+", encoding='utf-8')
        xml_string = state_file.read()
        all_states = xmltodict.parse(xml_string)
        all_states['all_states'][self.key] = self.current_states.tolist()
        state_file.seek(0)
        state_file.truncate()
        xmltodict.unparse(all_states,state_file)
        state_file.close

        #Store the updated tile to the tiles.pickle file
        tiles_file = open(self.exec_path+'/tiles.pickle', 'rb+')
        tiles = pickle.load(tiles_file)
        tiles[self.key] = self.tile
        tiles_file.seek(0)
        tiles_file.truncate
        pickle.dump(tiles, tiles_file)
        tiles_file.close()

        #Binarize the current state array depending upon the state_threshold
        for r in range(self.no_of_wordlines):
            for c in range(self.no_of_bitlines):
                if self.current_states[r][c] > state_threshold:
                    self.current_states_binary[r][c] = 1
                else:
                    self.current_states_binary[r][c] = 0
        
        #Store the binarized current states to the states_binary.xml file
        state_file = open(self.exec_path+"/states_binary.xml", "r+", encoding='utf-8')
        xml_string = state_file.read()
        if xml_string != "":
            all_states = xmltodict.parse(xml_string)
        else:
            all_states = {'all_states': {}}
        all_states['all_states'][self.key] = {}

        for c in range(self.no_of_bitlines):
            col_state_str = ""
            for r in range(self.no_of_wordlines):
                col_state_str += str(int(self.current_states_binary[r,c]))
            all_states['all_states'][self.key]['col_'+str(c)] = col_state_str
            
        state_file.seek(0)
        state_file.truncate()
        xmltodict.unparse(all_states,state_file)
        state_file.close
        
    def simulate_LOGIC(self, input_1_addr, input_2_addr, output_addr, V_MAGIC, V_ISOLATE_BL, V_ISOLATE_WL):
        #Create the voltage array for bitlines depending upon the input column addresses and output column address
        V_INPUT_BL = []
        for i in range(self.no_of_bitlines):
            if i == input_1_addr or i == input_2_addr:
                V_INPUT_BL.append(V_MAGIC)
            elif i == output_addr:
                V_INPUT_BL.append(0)
            else:
                V_INPUT_BL.append(V_ISOLATE_BL)

        #Create the voltage array for wordlines. All wordlines are kept as floating ('F')
        V_INPUT_WL = []
        for i in range(self.no_of_wordlines):
            V_INPUT_WL.append('F')

        #Simulate the tile and calculate the total energy consumption
        tile_cycle_energy = 0
        for i in range(int(self.cycle_time/self.step_size)):
            print("Simulating tile: " + str(self.tile_idx) + " @ time step: " + str(i))
            tile_step_energy = self.tile.simulate(V_INPUT_BL, V_INPUT_WL, self.step_size, return_states = False)
            tile_cycle_energy += tile_step_energy
        tile_cycle_energy = tile_cycle_energy * (self.step_size/1e-12)
        
        #Store the energy consumption
        file = open(self.exec_path+"/energy.xml", "r+", encoding='utf-8')
        xml_string = file.read()
        if xml_string != "":
            print("reading file")
            tiles_energy = xmltodict.parse(xml_string)
        else:
            tiles_energy = {'tiles_energy': {}}

        if self.key in tiles_energy['tiles_energy']:
            tiles_energy['tiles_energy'][self.key] = float(tiles_energy['tiles_energy'][self.key]) + tile_cycle_energy
        else:
            tiles_energy['tiles_energy'][self.key] = tile_cycle_energy

        file.seek(0)
        file.truncate()
        xmltodict.unparse(tiles_energy,file)
        file.close()

    def simulate_WRITE_ALL(self, addr, desired_state, V_APPLIED, V_ISOLATE_BL, V_ISOLATE_WL, row_parallel = False):
        #SET cycle of WRITE operation
        if row_parallel == True:
            V_INPUT_WL = []
            for r in range(self.no_of_wordlines):
                if r == addr:
                    V_INPUT_WL.append(0)
                else:
                    V_INPUT_WL.append(V_ISOLATE_WL)
            
            V_INPUT_BL = []
            for c in range(self.no_of_bitlines):
                if desired_state[c] == "1":
                    V_INPUT_BL.append(V_APPLIED[0])
                else:
                    V_INPUT_BL.append(V_ISOLATE_BL)
        else:
            V_INPUT_BL = []
            for c in range(self.no_of_bitlines):
                if c == addr:
                    V_INPUT_BL.append(0)
                else:
                    V_INPUT_BL.append(-V_ISOLATE_BL)
            
            V_INPUT_WL = []
            for r in range(self.no_of_wordlines):
                if desired_state[r] == "1":
                    V_INPUT_WL.append(-V_APPLIED[0])
                else:
                    V_INPUT_WL.append(-V_ISOLATE_WL)

        #Simulate the tile and calculate the total energy consumption
        tile_cycle_energy = 0
        for i in range(int(self.cycle_time/self.step_size)):
            tile_step_energy = self.tile.simulate(V_INPUT_BL, V_INPUT_WL, self.step_size, return_states = False)
            tile_cycle_energy += tile_step_energy
        tile_cycle_energy = tile_cycle_energy * (self.step_size/1e-12)

        #Store the energy consumption
        file = open(self.exec_path+"/energy.xml", "r+", encoding='utf-8')
        xml_string = file.read()
        if xml_string != "":
            tiles_energy = xmltodict.parse(xml_string)
        else:
            tiles_energy = {'tiles_energy': {}}

        if self.key in tiles_energy['tiles_energy']:
            tiles_energy['tiles_energy'][self.key] = float(tiles_energy['tiles_energy'][self.key]) + tile_cycle_energy
        else:
            tiles_energy['tiles_energy'][self.key] = tile_cycle_energy
        
        file.seek(0)
        file.truncate()
        xmltodict.unparse(tiles_energy,file)
        file.close()

        #RESET cycle of WRITE operation
        if row_parallel == True:
            V_INPUT_WL = []
            for r in range(self.no_of_wordlines):
                if r == addr:
                    V_INPUT_WL.append(0)
                else:
                    V_INPUT_WL.append(-V_ISOLATE_WL)
            
            V_INPUT_BL = []
            for c in range(self.no_of_bitlines):
                if desired_state[c] == "0":
                    V_INPUT_BL.append(V_APPLIED[1])
                else:
                    V_INPUT_BL.append(-V_ISOLATE_BL)
        else:
            V_INPUT_BL = []
            for c in range(self.no_of_bitlines):
                if c == addr:
                    V_INPUT_BL.append(0)
                else:
                    V_INPUT_BL.append(V_ISOLATE_BL)
            
            V_INPUT_WL = []
            for r in range(self.no_of_wordlines):
                if desired_state[r] == "0":
                    V_INPUT_WL.append(-V_APPLIED[1])
                else:
                    V_INPUT_WL.append(V_ISOLATE_WL)

        #Simulate the tile and calculate the total energy consumption
        tile_cycle_energy = 0
        for i in range(int(self.cycle_time/self.step_size)):
            tile_step_energy = self.tile.simulate(V_INPUT_BL, V_INPUT_WL, self.step_size, return_states = False)
            tile_cycle_energy += tile_step_energy
        tile_cycle_energy = tile_cycle_energy * (self.step_size/1e-12)

        #Store the energy consumption
        file = open(self.exec_path+"/energy.xml", "r+", encoding='utf-8')
        xml_string = file.read()
        if xml_string != "":
            tiles_energy = xmltodict.parse(xml_string)
        else:
            tiles_energy = {'tiles_energy': {}}

        if self.key in tiles_energy['tiles_energy']:
            tiles_energy['tiles_energy'][self.key] = float(tiles_energy['tiles_energy'][self.key]) + tile_cycle_energy
        else:
            tiles_energy['tiles_energy'][self.key] = tile_cycle_energy
        
        file.seek(0)
        file.truncate()
        xmltodict.unparse(tiles_energy,file)
        file.close()
        
    def simulate_WRITE(self, addr_col, addr_row, V_APPLIED, V_ISOLATE_BL, V_ISOLATE_WL, operation = "SET"):
        if operation == "SET":
            V_INPUT_BL = []
            for c in range(self.no_of_bitlines):
                if c == addr_col:
                    V_INPUT_BL.append(V_APPLIED)
                else:
                    V_INPUT_BL.append(V_ISOLATE_BL)

            V_INPUT_WL = []
            for r in range(self.no_of_wordlines):
                if r == addr_row:
                    V_INPUT_WL.append(0)
                else:
                    V_INPUT_WL.append(V_ISOLATE_WL)

        else:
            V_INPUT_BL = []
            for c in range(self.no_of_bitlines):
                if c == addr_col:
                    V_INPUT_BL.append(V_APPLIED)
                else:
                    V_INPUT_BL.append(-V_ISOLATE_BL)

            V_INPUT_WL = []
            for r in range(self.no_of_wordlines):
                if r == addr_row:
                    V_INPUT_WL.append(0)
                else:
                    V_INPUT_WL.append(-V_ISOLATE_WL)

        #Simulate the tile and calculate the total energy consumption
        tile_cycle_energy = 0
        for i in range(int(self.cycle_time/self.step_size)):
            tile_step_energy = self.tile.simulate(V_INPUT_BL, V_INPUT_WL, self.step_size, return_states = False)
            tile_cycle_energy += tile_step_energy
        tile_cycle_energy = tile_cycle_energy * (self.step_size/1e-12)

        #Store the energy consumption
        file = open(self.exec_path+"/energy.xml", "r+", encoding='utf-8')
        xml_string = file.read()
        if xml_string != "":
            tiles_energy = xmltodict.parse(xml_string)
        else:
            tiles_energy = {'tiles_energy': {}}

        if self.key in tiles_energy['tiles_energy']:
            tiles_energy['tiles_energy'][self.key] = float(tiles_energy['tiles_energy'][self.key]) + tile_cycle_energy
        else:
            tiles_energy['tiles_energy'][self.key] = tile_cycle_energy
        
        file.seek(0)
        file.truncate()
        xmltodict.unparse(tiles_energy,file)
        file.close()
