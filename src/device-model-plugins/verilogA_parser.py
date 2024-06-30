import numpy as np
import os
import re
import math
import warnings

warnings.filterwarnings('ignore')

tab_space = 0
beginEndCount = 0
isCaseBlock = False
isModule = False

variable_declaration = ''
function_name = ''
variable_name = []
input_variables = ''
output_variables = []
parameter_variables = ''

def extract_variable(expression,variable_name):
  expression = expression.replace(" ","")
  expression = expression + " "

  table = expression.maketrans("=+*/-<>]&|!([)",",,,,,,,,,,,,,,")
  modified_exp = expression.translate(table)

  vars = modified_exp.split(",")

  for i in range(len(vars)):
    vars[i] = vars[i].strip()

  vars = list(set(vars))
  vars = [i for i in vars if i]
  vars = list(set(vars)&set(variable_name))

  expression_characters = [char for char in expression]

  variable_location = {new_list: [] for new_list in vars}

  for var in vars:
    L = len(var)
    for i in range(L,len(expression_characters)+1):
      if var == ''.join(expression_characters[i-L:i]) and expression_characters[i-L-1].isalpha() == False and expression_characters[i-L-1] != '_' and expression_characters[i].isalpha() == False and expression_characters[i] != '_' and var in variable_name:
        variable_location[var].append(i-L)

  locations = []

  for var in variable_location:
    for loc in variable_location[var]:
      locations.append(loc)

  locations.sort()
  count = 0

  new_expression = expression

  for loc in locations:
    new_expression = expression[0:loc+count*5] + "self." + expression[loc+count*5:]
    expression = new_expression
    count = count + 1

  return new_expression

def classify_line(line):
  if (line[0:2] == "//"):
    line_type = "comment"
  elif (line[0:6] == "module"):
    line_type = "module declaration"
  elif (line[0:6] == "analog"):
    line_type = "analog block"
  elif (line[0:8] == "`define "):
    line_type = "variable declaration type 1"
  elif (line[0:9] == "parameter"):
    line_type = "variable declaration type 2"
  elif (line[0:4] == "real" or line[0:7] == "integer"):
    line_type = "variable declaration type 3"
  elif (line[0:5] == "input" or line[0:5] == "inout"):
    line_type = "input variables"
  elif (line[0:6] == "output"):
    line_type = "output variables"
  elif (line[0:15] == "@(initial_step)"):
    line_type = "initial step"
  elif (line[0:5] == "begin" or line[0:3] == "end" or line[0:7] == "endcase"):
    line_type = "begin or end or endcase"
  elif (len(line) == 0):
    line_type = "blank line"
  elif (line[0:2] == "if" or line[0:4] == "else" or line[0:6] == "elseif"):
    line_type = "if else block"
  elif (line[0:3] == "for"):
    line_type = "for block"
  elif (line[0:4] == "case"):
    line_type = "case block"
  elif (line.find('=') != -1):# and line.find("V(") == -1 and line.find("I(") == -1):
    line_type = "arithmetic type 1"
  elif (line[0] == "$"):
    line_type = "$ statement"
  elif (isCaseBlock == True):
    line_type = "different cases"
  elif (line.find("<+") != -1):
    line_type = "voltage or current"
  else:
    line_type = "nothing"
  return line_type

def write_to_python_file(python_file, line_type, line_from_file):
  global variable_declaration
  global function_name
  global tab_space
  global input_variables
  global output_variables
  global beginEndCount
  global isCaseBlock
  global parameter_variables
  #------------------------------------------------------------------------------------------------------------------------------
  #WHITE SPACE
  if (line_type == "blank line"):
    python_file.write("\n")
  #------------------------------------------------------------------------------------------------------------------------------
  #COMMENT
  elif (line_type == "comment"):
    pass
  #------------------------------------------------------------------------------------------------------------------------------
  #VARIABLE DECLARATION TYPE 1
  elif (line_type == "variable declaration type 1"):
    line_from_file = line_from_file[8:]
    variable_name.append(line_from_file[0:line_from_file.find(" ")])
    line_from_file = line_from_file.replace(" "," = ",1);
    line_from_file = "self." + line_from_file + '\n'
    variable_declaration = variable_declaration + line_from_file

    while (variable_declaration.find("`") != -1):
      variable_declaration = variable_declaration.replace('`','');
  #------------------------------------------------------------------------------------------------------------------------------
  #VARIABLE DECLARATION TYPE 2
  elif (line_type == "variable declaration type 2"):
    line_from_file = line_from_file[9:].lstrip().replace("\n","")
    # print(line_from_file)
    if (line_from_file[0:4] == "real"):
      line_from_file = line_from_file[5:].lstrip()
    elif (line_from_file[0:7] == "integer"):
      line_from_file = line_from_file[8:].lstrip()

    line_from_file = line_from_file.replace(";","")

    if "from" in line_from_file:
      line_from_file = line_from_file[0:line_from_file.find("from")-1]

    variable_name.append(line_from_file[0:line_from_file.find(" ")])

    parameter_variables = parameter_variables + ", " + line_from_file
    line_from_file = "self." + line_from_file + '\n'

    while (line_from_file.find("`") != -1):
      line_from_file = line_from_file.replace("`","")

    var_name = line_from_file[5:line_from_file.find(" ")]

    line_from_file = line_from_file[0:line_from_file.find("=")+1] + var_name + "\n"

    variable_declaration = variable_declaration + line_from_file
  #------------------------------------------------------------------------------------------------------------------------------
  #VARIABLE DECLARATION TYPE 3
  elif (line_type == "variable declaration type 3"):
    #REAL
    if (line_from_file[0:4] == "real"):
      line_from_file = line_from_file[5:]
    elif (line_from_file[0:7] == "integer"):
      line_from_file = line_from_file[8:]

    line_from_file = line_from_file.replace(";","")
    line_from_file = line_from_file.replace("\n","")

    # variable_name.append(line_from_file[0:line_from_file.find(" ")])
    line_from_file = "self." + line_from_file

    while (line_from_file.find("`") != -1):
      line_from_file = line_from_file.replace("`","")

    if (line_from_file.find("[") != -1 and line_from_file.find("]") != -1 and line_from_file.find(":") != -1):
      variable_name.append(line_from_file[5:line_from_file.find("[")].strip())
      line_from_file = line_from_file.replace(":",",")
      line_from_file = line_from_file.replace("["," = np.zeros((")
      line_from_file = line_from_file.replace(",0",",1")
      line_from_file = line_from_file.replace("]","))\n")
    elif (line_from_file.find("=") == -1):
      variable_name.append(line_from_file[5:].strip())
      line_from_file = line_from_file + " = 0\n"
    else:
      variable_name.append(line_from_file[5:line_from_file.find("=")-1].strip())
      line_from_file = line_from_file + "\n"

    variable_declaration = variable_declaration + line_from_file
  #------------------------------------------------------------------------------------------------------------------------------
  #INPUT VARIABLES
  elif (line_type == "input variables"):
    line_from_file = line_from_file[6:]
    line_from_file = line_from_file.replace(";","")
    line_from_file = line_from_file.replace("\n","")
    input_variables = line_from_file + ", current_time, initial_step"
  #------------------------------------------------------------------------------------------------------------------------------
  #OUTPUT VARIABLES
  elif (line_type == "output variables"):
    line_from_file = line_from_file[7:]
    line_from_file = line_from_file.replace(";","")
    line_from_file = line_from_file.replace("\n","")

    if "," in line_from_file:
      output_variables = line_from_file.split(",")
    else:
      output_variables.append(line_from_file)

    for i in range(len(output_variables)):
      output_variables[i] = output_variables[i].strip()
  #------------------------------------------------------------------------------------------------------------------------------
  #INITIAL STEP
  elif (line_type == "initial step"):
    i = 0
    while (i < tab_space):
      python_file.write("\t")
      i = i + 1

    python_file.write("if (initial_step):\n")
  #------------------------------------------------------------------------------------------------------------------------------
  #ANALOG BLOCK
  elif (line_type == "analog block"):
    python_file.write("class " + function_name + ":\n")

    i = 0
    tab_space = tab_space + 1
    while (i < tab_space):
      python_file.write("\t")
      i = i + 1
    python_file.write("def __init__(self" + parameter_variables + "):\n")

    tab_space = tab_space + 1

    for variable in variable_declaration.splitlines():
      i = 0
      while (i < tab_space):
        python_file.write("\t")
        i = i + 1
      python_file.write(variable + "\n")

    python_file.write("\n")
    tab_space = tab_space - 1
    i = 0
    while (i < tab_space):
      python_file.write("\t")
      i = i + 1

    python_file.write("def V(self,T1,T2):\n")

    tab_space = tab_space + 1
    i = 0
    while (i < tab_space):
      python_file.write("\t")
      i = i + 1

    python_file.write("return T1 - T2\n\n")

    tab_space = tab_space - 1
    i = 0
    while (i < tab_space):
      python_file.write("\t")
      i = i + 1

    python_file.write("def simulate(self, " + input_variables + "):\n")
  #------------------------------------------------------------------------------------------------------------------------------
  #BEGIN END
  elif (line_type == "begin or end or endcase"):
    line_from_file = line_from_file.lstrip().rstrip().replace("\n","")
    if (line_from_file[0:5] == "begin"):
      tab_space = tab_space + 1
      beginEndCount = beginEndCount + 1
    elif (line_from_file == "endcase"):
      tab_space = tab_space - 1
    elif (line_from_file == "end"):
      beginEndCount = beginEndCount - 1
      if beginEndCount == 0:
        i = 0
        while (i < tab_space):
          python_file.write("\t")
          i = i + 1

        if not output_variables:
          python_file.write("return current")
        else:
          temp = str()
          for var in output_variables:
            temp = temp + ", " + var
          python_file.write("return current"+temp)
      tab_space = tab_space - 1
  #------------------------------------------------------------------------------------------------------------------------------
  #ARITHMETIC TYPE
  elif (line_type == "arithmetic type 1"):
    while (line_from_file.find("`") != -1):
      line_from_file = line_from_file.replace("`","")

    line_from_file = line_from_file.replace("\n","")
    line_from_file = line_from_file.replace(";","")

    # print(line_from_file)
    line_from_file = extract_variable(line_from_file,variable_name)

    while (line_from_file.find("ln") != -1):
      line_from_file = line_from_file.replace("ln","math.log")

    while (line_from_file.find("limexp") != -1):
      line_from_file = line_from_file.replace("limexp","math.e**")

    while (line_from_file.find("exp") != -1):
      line_from_file = line_from_file.replace("exp","math.e**")

    while (line_from_file.find("sqrt") != -1):
      line_from_file = line_from_file.replace("sqrt","ssqqrrtt")

    while (line_from_file.find("ssqqrrtt") != -1):
      line_from_file = line_from_file.replace("ssqqrrtt","math.sqrt")

    while (line_from_file.find("sinh") != -1):
      line_from_file = line_from_file.replace("sinh","ssiinnhh")

    while (line_from_file.find("ssiinnhh") != -1):
      line_from_file = line_from_file.replace("ssiinnhh","math.sinh")

    if ("$strobe" in line_from_file):
      line_from_file = line_from_file.replace("$strobe","print")
      line_from_file = line_from_file.replace(","," % (")
      line_from_file = line_from_file.replace(")","))")
      # print(line_from_file)

    if ("$abstime" in line_from_file):
      line_from_file = line_from_file.replace("$abstime","current_time")

    if ("V(" in line_from_file):
      line_from_file = line_from_file.replace("V(","self.V(")

    if ("$fopen" in line_from_file):
      line_from_file = " "

    if ("$fwrite" in line_from_file):
      line_from_file = " "

    line_from_file = line_from_file.replace(";","")

    i = 0
    while (i < tab_space):
      python_file.write("\t")
      i = i + 1

    python_file.write(line_from_file + "\n")
  #------------------------------------------------------------------------------------------------------------------------------
  #IF ELSE BLOCK
  elif (line_type == "if else block"):
    i = 0
    while (i < tab_space):
      python_file.write("\t")
      i = i + 1

    line_from_file = line_from_file.lstrip().rstrip().replace("\n","")

    if (line_from_file.find("else if") != -1):
      line_from_file = line_from_file.replace("else if","elif")

    while (line_from_file.find("`") != -1):
      line_from_file = line_from_file.replace("`","")

    line_from_file = extract_variable(line_from_file,variable_name)

    while (line_from_file.find("$abstime") != -1):
      line_from_file = line_from_file.replace("$abstime","current_time")

    while (line_from_file.find("&&") != -1):
      line_from_file = line_from_file.replace("&&"," and ")

    while (line_from_file.find("||") != -1):
      line_from_file = line_from_file.replace("||"," or ")

    if ("V(" in line_from_file):
      line_from_file = line_from_file.replace("V(","self.V(")

    line_from_file = line_from_file + ":\n"

    python_file.write(line_from_file)
  #------------------------------------------------------------------------------------------------------------------------------
  #FOR LOOP BLOCK
  elif (line_type == "for block"):
    line_from_file = line_from_file.rstrip().replace("\n","")
    while (line_from_file.find("`") != -1):
      line_from_file = line_from_file.replace("`","")

    var_temp = line_from_file[line_from_file.find("(")+1:line_from_file.find("=")].strip()
    start_range_of_loop = line_from_file[line_from_file.find("=")+1:line_from_file.find(";")].strip()

    if (line_from_file.find("<") != -1):
      end_range_of_loop = line_from_file[line_from_file.find("<")+1:line_from_file.find(";",line_from_file.find("<"))].strip()
    elif (line_from_file.find("<=") != -1):
      end_range_of_loop = line_from_file[line_from_file.find("<=")+1:line_from_file.find(";",line_from_file.find("<="))].strip()
    elif (line_from_file.find(">") != -1):
      end_range_of_loop = line_from_file[line_from_file.find(">")+1:line_from_file.find(";",line_from_file.find(">"))].strip()
    elif (line_from_file.find(">=") != -1):
      end_range_of_loop = line_from_file[line_from_file.find(">=")+1:line_from_file.find(";",line_from_file.find(">="))].strip()

    line_from_file = extract_variable(line_from_file,variable_name)

    i = 0
    while (i < tab_space):
      python_file.write("\t")
      i = i + 1

    if (start_range_of_loop.isnumeric()):
      python_file.write("for self." + var_temp + " in range(" + start_range_of_loop + ",self." + end_range_of_loop + "):\n")
    else:
      python_file.write("for self." + var_temp + " in range(self." + start_range_of_loop + ",self." + end_range_of_loop + "):\n")
  #------------------------------------------------------------------------------------------------------------------------------
  #CASE BLOCK
  elif (line_type == "case block"):
    isCaseBlock = True

    line_from_file = extract_variable(line_from_file,variable_name)

    line_from_file = line_from_file.replace("case","match")
    line_from_file = line_from_file.replace("("," ")
    line_from_file = line_from_file.replace(")",":")
    line_from_file = line_from_file + "\n"

    i = 0
    while (i < tab_space):
      python_file.write("\t")
      i = i + 1

    python_file.write(line_from_file)
    tab_space = tab_space + 1
  #------------------------------------------------------------------------------------------------------------------------------
  #DIFFERENT  CASES
  elif (line_type == "different cases"):
    line_from_file = line_from_file.replace("\n","")
    if (line_from_file[0:7] == "default"):
      isCaseBlock = False
      line_from_file = "case _:\n"
    else:
      line_from_file = "case " + line_from_file + "\n"

    i = 0
    while (i < tab_space):
      python_file.write("\t")
      i = i + 1

    python_file.write(line_from_file)
  #------------------------------------------------------------------------------------------------------------------------------
  #VOLTAGE OR CURRENT BLOCK
  elif (line_type == "voltage or current"):
    if (line_from_file.find("I(") != -1):
      line_from_file = line_from_file.rstrip().replace("\n","")
      line_from_file = line_from_file.replace(";","")
      while (line_from_file.find("`") != -1):
        line_from_file = line_from_file.replace("`","")
      
      while (line_from_file.find("exp") != -1):
        line_from_file = line_from_file.replace("exp","math.e**")

      line_from_file = extract_variable(line_from_file,variable_name)

      line_from_file = "current " + line_from_file[line_from_file.find("<+"):] + "\n"

      line_from_file = line_from_file.replace("<+"," = ")

      if ("V(" in line_from_file):
        line_from_file = line_from_file.replace("V(","self.V(")

      i = 0
      while (i < tab_space):
        python_file.write("\t")
        i = i + 1

      python_file.write(line_from_file + "\n")

    elif (line_from_file.find("V(") != -1):
      line_from_file = line_from_file.rstrip().replace("\n","")
      line_from_file = line_from_file.replace(";","")
      while (line_from_file.find("`") != -1):
        line_from_file = line_from_file.replace("`","")

      line_from_file = extract_variable(line_from_file,variable_name)
      

      line_from_file = line_from_file.replace("V(","")
      line_from_file = line_from_file.replace(")","",1)
 
      line_from_file = line_from_file.replace("<+"," = ")

      i = 0
      while (i < tab_space):
        python_file.write("\t")
        i = i + 1

      python_file.write(line_from_file + "\n")
  #------------------------------------------------------------------------------------------------------------------------------
  #NOTHING
  elif (line_type == "nothing"):
    pass
  #------------------------------------------------------------------------------------------------------------------------------
  #MODULE DECLARATION
  elif (line_type == "module declaration"):
    function_name = "Device"

def parse(file_name, exec_path):
  print("Parsing")
  verilog_A_file = open(file_name + ".va")
  python_file = open(exec_path + '/Python_File.txt','x')
  python_file.write("import math\nimport numpy as np\n")

  for line in verilog_A_file:
      line = line.lstrip()
      line_type = classify_line(line)
      write_to_python_file(python_file, line_type, line)

  python_file.close()
  verilog_A_file.close()
  os.rename(exec_path + '/Python_File.txt',exec_path + '/Device_Model_Class.py')
  print("Parsing Done")
