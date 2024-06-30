from Device_Model_Class import *
import math
from decimal import Decimal

class tile():
    def __init__(self, no_of_wordlines, no_of_bitlines, R, C, initial_input_state = [0.1]):
        #Initialize all the required variables
        self.no_of_bitlines = no_of_bitlines
        self.no_of_wordlines = no_of_wordlines
        self.R = R
        self.C = C
        self.energy = np.zeros((self.no_of_wordlines,self.no_of_bitlines))
        self.states = np.zeros((self.no_of_wordlines,self.no_of_bitlines))

        self.BL_Node_Voltage = np.zeros((self.no_of_wordlines+1,self.no_of_bitlines))
        self.WL_Node_Voltage = np.zeros((self.no_of_wordlines,self.no_of_bitlines+1))

        self.time = np.zeros((self.no_of_wordlines,self.no_of_bitlines))
        self.cells = {}

        #Initialize each cell in the crossbar array with the given initial state
        for r in range(no_of_wordlines):
            for c in range(no_of_bitlines):
                cell_name = "CELL_" + str(r) + "_" + str(c)
                self.cells[cell_name] = Device(initial_state=initial_input_state[r][c])
                _,_ = self.cells[cell_name].simulate(0, 0, self.time[r][c], True)
                self.time[r][c] = 1e-12

    def simulate(self, V_INPUT_BL, V_INPUT_WL, step_size, return_states = False):
        #Copy input wordline voltages to WL_Node_Voltage matrix
        for r in range(self.no_of_wordlines):
            if V_INPUT_WL[r] == 'F':
                self.WL_Node_Voltage[r][0] = 0
            else:
                self.WL_Node_Voltage[r][0] = V_INPUT_WL[r]

        #Copy input bitline voltages to BL_Node_Voltage matrix
        for c in range(self.no_of_bitlines):
            self.BL_Node_Voltage[0][c] = V_INPUT_BL[c]

        #Traverse through each wordline of the array
        for r in range(self.no_of_wordlines):
            #Check if the wordline is floating
            if V_INPUT_WL[r] == 'F':
                resistances = []
                expression = ""
                
                #Calculate the resistance of each cell in the floating wordline
                for c in range(self.no_of_bitlines):
                    cell_name = "CELL_" + str(r) + "_" + str(c)
                    current, state = self.cells[cell_name].simulate(0.1, 0, self.time[r][c], False)
                    self.time[r][c] += step_size

                    resistances.append(0.1/current)
                
		#Calculating V_FLOAT
                num = Decimal(0)
                den = Decimal(0)
                for i in range(self.no_of_bitlines):
                    R_mul = Decimal(1)
                    for j in range(self.no_of_bitlines):
                        if j != i:
                            R_mul = Decimal(R_mul*Decimal(resistances[j]))
                    den = den + R_mul
                    num = num + Decimal(self.BL_Node_Voltage[r][i])*R_mul 

                V_FLOAT = Decimal(num/den)
                self.WL_Node_Voltage[r][0] = V_FLOAT
                    
            #Traverse through each bitline of the selected wordline of the array
            for c in range(self.no_of_bitlines):
                cell_name = "CELL_" + str(r) + "_" + str(c)
                #Calculate voltage drop due to parasitic RC network on bitline and wordline
                self.BL_Node_Voltage[r+1][c] = self.BL_Node_Voltage[r][c] - (self.BL_Node_Voltage[r][c] - self.BL_Node_Voltage[r+1][c])*(np.exp(-step_size/(self.R*self.C)))
                self.WL_Node_Voltage[r][c+1] = self.WL_Node_Voltage[r][c] - (self.WL_Node_Voltage[r][c] - self.WL_Node_Voltage[r][c+1])*(np.exp(-step_size/(self.R*self.C)))

                #Simulate the device with the calculated voltages
                current, state = self.cells[cell_name].simulate(self.BL_Node_Voltage[r+1][c], self.WL_Node_Voltage[r][c+1], self.time[r][c], False)
                self.states[r][c] = state

                if V_INPUT_WL[r] == 'F':
                    self.time[r][c] += step_size/1000
                else:
                    self.time[r][c] += step_size
                
                device_energy    = (abs(current)*abs(self.BL_Node_Voltage[r+1][c] - self.WL_Node_Voltage[r][c+1]))*step_size
                resistor_energy  = (((self.BL_Node_Voltage[r][c] - self.BL_Node_Voltage[r+1][c])**2 + (self.WL_Node_Voltage[r][c] - self.WL_Node_Voltage[r][c+1])**2)/self.R)*step_size
                capacitor_energy = 0.5*self.C*(self.BL_Node_Voltage[r+1][c]**2 + self.WL_Node_Voltage[r][c+1]**2)
                
                self.energy[r][c] = device_energy + resistor_energy + capacitor_energy

        tile_step_energy = 0
        for r in range(self.no_of_wordlines):
            for c in range(self.no_of_bitlines):
                tile_step_energy += self.energy[r][c]
                self.energy[r][c] = 0
            
        if return_states:
            return tile_step_energy, self.states
        else:
            return tile_step_energy
 
