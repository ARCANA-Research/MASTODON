#### Ain't nobody got time to write this in c++
#### This program enables people to write code in EasyRACER (.erc) file
import re
import sys

forbidden_loop_vars = ["r","m"]
free_reg = []

def name_check(macros_vars,new_var):
    for exsiting_macros in macros_vars:
        #print(new_var, exsiting_macros)
        if isinstance(exsiting_macros,int):
            # iteration will go to the int values in the pairs of dict()
            continue
        if new_var in exsiting_macros or exsiting_macros in new_var:
            print(new_var+" conflicts with "+exsiting_macros+", please rename!")
            exit()

        # TODO: switch to the following condition when we can replace variables appropriately
        # if new_var == exsiting_macros:
        #     print(new_var+" conflicts with "+exsiting_macros+", please rename!")
        #     exit()


def fetch_program(file_addr):
    program_string = []
    comment_block = False
    with open(file_addr, "r") as file:
        for line in file:
            if "/*" in line:
                comment_block = True
            else:
                if comment_block:
                    if "*/" in line:
                        comment_block = False
                else:
                    if not line.isspace() and "//" not in line:
                        program_string.append(line.strip())
    return program_string

def replace_vars(macro_vars, for_vars, for_vals, line):
    #inst_b = line.find(" ")
    #inst = line[:inst_b+1]
    #rest = line[inst_b+1:]
    # variable names should avoid keywords (e.g., insts, for, define)
    start = line.find('[')
    end = line.find(']')
    #print(line)
    #print("initial start end",start,end)
    cnt = 0
    rest_line = line
    complete_line = ""
    while(start != -1):
        complete_line += rest_line[:start]
        tmp = rest_line[start:end+1] # replace only the content inside [...]
        for i in range(len(for_vars)):
            #tmp = replace_skip_substring(tmp, for_vars[i], str(for_vals[i]))
            tmp = tmp.replace(for_vars[i], str(for_vals[i]))
        for i in macro_vars:
            #tmp = replace_skip_substring(tmp, i, str(macro_vars[i]))
            tmp = tmp.replace(i, str(macro_vars[i]))
        rest_line = rest_line[end+1:]
        complete_line += tmp
        start = rest_line.find('[')
        # it has to be based on start since end idx might already be changed!!!
        end = rest_line.find(']')
        #if(start != -1):
            #print("line[start+1:end]",line[start+1:end])
        cnt += 1
        if(cnt >10):
            break
    return complete_line + rest_line
# this functions simplifies all math expressions in square brackets

# TODO: someone please look into how to execute this replace condition correctly
# def replace_skip_substring(square_bracket_content, var_name, var_value):
#     pattern = re.compile(r'(?<![a-zA-Z_])' + re.escape(var_value) + r'(?![a-zA-Z_])')
#     replaced_string = pattern.sub(var_name, square_bracket_content)
#     print(replaced_string)
#     return replaced_string

def evaluate_line(s):
    res = s
    start = s.find('[')
    end = s.find(']')
    while (start != -1):
        tmp = res[start+1:end]
        #print("eval_line",tmp)
        res = res.replace("[" + tmp + "]", str(int(eval(tmp))))
        start = res.find('[')
        end = res.find(']')
    return res

def parse_program(program_string,outside_var):
    num_lines = len(program_string)
    result = ""
    elseif_list = []
    global_mask = ""

    #cnt keeps track of line in the program
    cnt = 0
    #loop depth
    depth = 0

    mask_cnt = 0

    # string representation for macro variables
    macros_vars = outside_var

    # The following is for loop meta data
    # string representation of for loop variables
    for_vars = []
    # values of the variables
    for_vals = []
    # start lines of the for loop (so you can go back at the end of loop)
    for_starts = []
    # loop stop condiitions to evaluate at endfor
    end_conditions = []
    # how you increment the variables
    for_incs = []
     
    # everything needed by a func definition and func
    # here i use mutiple lists to implement a hash table
    # with func_names as key
    # the other lists are val structs 
    # func_names = []
    # para_names = []
    func_attributes = {}
    # func_ranges = []
    func_ranges = {}
    # functions are similar to for loops in many ways
    # all the input parameters are local variables like for_vars
    # we also need to go back to where the function is called
    # after exit the function

    # stuff to handle while loop
    while_counter = 0;

    # while loop instead of for makes it easier to deal with fors
    # assume for loops are formatted as: for (i = 0; i < 5; i+=1)
    while (cnt < num_lines):
        line = program_string[cnt]
        # line = replace_vars(macros_vars, for_vars, for_vals, line)
        # line = evaluate_line(line)
        if "for" == line[:3]:
            line = replace_vars(macros_vars, for_vars, for_vals, line)
            line = evaluate_line(line)
            line = line.replace(' ', '')
            components = re.split(r";|\(|\)", line)
            #print("compoents: ")
            #print(components)
            if(len(components) < 4):
                print("ERROR!")
            init_ind = components[1].find('=')
            
            if (components[1][:init_ind] in forbidden_loop_vars):
                print("Sorry, the loop variable ", components[1][:init_ind], "is not allowed... Please choose something else.")
                exit(0)
            pending_for_var = components[1][:init_ind]
            pending_for_val = components[1][init_ind+1:]
            name_check(macros_vars,pending_for_var)
            name_check(for_vals,pending_for_var)
            for_vars.append(pending_for_var)
            for_vals.append(int(eval(pending_for_val)))
            # TODO: change the "+1" by default to the actual increment value
            pending_end_cond = components[2].replace(pending_for_var, ("for_vals[" + str(depth) + "] + 1"))
            cur_end_cond = components[2].replace(pending_for_var, ("for_vals[" + str(depth) + "]"))
#            print("cur_end_cond: " + cur_end_cond)
#            print(eval(cur_end_cond))
            if(not eval(cur_end_cond)):
                for_vars.pop()
                for_vals.pop()
                nested_forloops = 0
                cnt += 1
                while(cnt < num_lines):
                    # skip to the end of the correponding
                    # endfor for this for loop
                    line = program_string[cnt]
                    cnt +=1 # the cnt for the next line
                    if "for" == line[:3]:
                        nested_forloops += 1
                    if "endfor" in line:
                        if nested_forloops == 0:
                            break
                        else:
                            nested_forloops -= 1
                continue
            #    print(line)
            #    exit

            #print("for_vars: "+ pending_for_var)
            #print("for_vals: "+ pending_for_val)
            for_starts.append(cnt)
#            print(cnt)
            end_conditions.append(pending_end_cond)
#            print("end_conditions.append")
            for_incs.append(components[3].replace("=", "").replace(pending_for_var, ("for_vals[" + str(depth) + "]")))
            #print("for_incs: "+for_incs[-1])
            depth += 1
        elif "endfor" in line:
#            print('eval('+end_conditions[-1]+') = ' + str(eval(end_conditions[-1])))
            if(int(eval(end_conditions[-1]))):
                # still not finish the loop yet
#                print('for_incs[-1] = '+ str(eval(for_incs[-1])))
                for_vals[-1] = int(eval(for_incs[-1]))
                cnt = for_starts[-1]
            else:
                # already outof the current loop
                for_vars.pop()
                for_vals.pop()
                for_starts.pop()
                end_conditions.pop()
                #print("end_conditions.pop")
                for_incs.pop()
                depth -= 1
        elif "if" == line[:2]:
            components = re.split(r";|\(|\)", line)
            ops = components[1].split(" ")
            if (len(ops) < 3):
                print("Parsing error: please add spaces in your if conditionals (it makes parsing easier)")

            tmp = components[0].split(" ")[1].split("<-")[0]
            global_mask = tmp.split(",")[0]
            mask = tmp.split(",")[1]
            elseif_list.append(mask)

            op1 = str(int(eval(replace_vars(macros_vars, for_vars, for_vals, " " + ops[0])[2:-1])))
            op2 = str(int(eval(replace_vars(macros_vars, for_vars, for_vals, " " + ops[2]).strip(" [] "))))

            if ops[1] == "<":
                result += "CMPST "
            elif ops[1] == ">":
                result += "CMPLT "
            elif ops[1] == "==":
                result += "CMPEQ "
            result += op1 + " " + op2 + "\n"
            result += "LFLUSH\n"
            result += "GETMASK " +  mask + "\n"
            result += "SETMASK " +  mask + "\n"
        elif "elif" in line:
            result += "UNMASK\n"
            components = re.split(r";|\(|\)", line)
            ops = components[1].split(" ")
            if (len(ops) < 3):
                print("Parsing error: please add spaces in your if conditionals (it makes parsing easier)")

            mask = components[0].split(" ")[1].split("<-")[0]

            op1 = str(int(eval(replace_vars(macros_vars, for_vars, for_vals, " " + ops[0])[2:-1])))
            op2 = str(int(eval(replace_vars(macros_vars, for_vars, for_vals, " " + ops[2]))))
            if ops[1] == "<":
                result += "CMPST "
            elif ops[1] == ">":
                result += "CMPLT "
            elif ops[1] == "==":
                result += "CMPEQ "
            result += op1 + " " + op2 + "\n"
            result += "LFLUSH\n"
            result += "GETMASK " +  mask + "\n"
            result += "MOV " + mask + " " + global_mask + "\n"
            for branch in elseif_list:
                result += "DINV " + branch + "\n"
                result += "AND " + global_mask + " " + branch + " " + global_mask + "\n"
                result += "DINV " + branch + "\n"
            result += "SETMASK " +  global_mask + "\n"
            elseif_list.append(mask)
        elif "else" in line:
            result += "UNMASK\n"
            result += "DINV " + elseif_list[0] + "\n"
            result += "MOV " + elseif_list[0] + " " + global_mask + "\n"
            result += "DINV " + elseif_list[0] + "\n"
            for branch in elseif_list[1:]:
                result += "DINV " + branch + "\n"
                result += "AND " + global_mask + " " + branch + " " + global_mask + "\n"
                result += "DINV " + branch + "\n"
            result += "SETMASK " +  global_mask + "\n"
        elif "endif" in line:
            result += "UNMASK\n"
            elseif_list = []
            global_mask = ""
        elif "while" == line[:5]:
            components = re.split(r";|\(|\)", line)

            ops = components[1].split(" ")
            op1 = str(eval(replace_vars(macros_vars, for_vars, for_vals, " " + ops[0])[2:-1]))
            op2 = str(eval(replace_vars(macros_vars, for_vars, for_vals, " " + ops[2])[2:-1]))

            masks = components[0].split(" ")[1].split("<-")[0]
            masks = masks.split(",")
            mask_reg = str(eval(replace_vars(macros_vars, for_vars, for_vals, " " + masks[0])[2:-1]))
            mask_throwaway = str(eval(replace_vars(macros_vars, for_vars, for_vals, " " + masks[1])[2:-1]))

            #result += "===== WHILE START ======\n"

            comparison = ""
            if (ops[1] == "<"):
                comparison = "CMPST "
            elif (ops[1] == ">"):
                comparison = "CMPLT "
            else:
                print("Error: while loop >= or <= not supported. Please use > or <")
                exit(0)
            
            result += comparison + op1 + " " + op2 + "\n"
            result += "LFLUSH\n"
            result += "GETMASK " + mask_reg + "\n"
            result += ".__while" + str(while_counter) + ":\n"

            result += "GETMASK " + mask_throwaway + "\n"

            result += "SETMASK " + mask_reg + "\n"

            #result += "===== WHILE BODY START ======\n"

        elif "endwhile" == line[:8]:
            #result += "===== WHILE BODY END ======\n"

            result += "UNMASK\n"
            result += comparison + op1 + " " + op2 + "\n"
            result += "LFLUSH\n"
            result += "GETMASK " + mask_reg + "\n"
            result += comparison + op1 + " " + op2 + "\n"
            result += "LFLUSH\n"
            result += "INV_TORUS\n"
            result += "JUMP_COND .__while" + str(while_counter) + "\n"
            result += "GETMASK " + mask_throwaway + "\n"

            while_counter += 1
            #result += "===== WHILE END ======\n"

        elif "#define" in line:
            line = replace_vars(macros_vars, for_vars, for_vals, line)
            line = evaluate_line(line)
            components = re.split(r" ", line)
            new_macro = components[1]
            if not new_macro in macros_vars:
                name_check(macros_vars,new_macro)
                macros_vars[new_macro] = int(components[2])
#                print(components[1], macros_vars[components[1]])
        elif "func" == line[:4]:
            line = replace_vars(macros_vars, for_vars, for_vals, line)
            line = evaluate_line(line)
            components = re.split(r"\(|\)", line[5:])
            #print(components)
            func_name = components[0]
            arguments = components[1].split(",")
            arguments_stripped = []
            for arg in arguments:
                arguments_stripped.append(arg.strip())
            # func_names.append(func_name)
            # para_names.append(arguments)
            func_attributes[func_name] = arguments_stripped
            func_start = cnt+1
            while(cnt+1 < num_lines):
                cnt +=1
                line = program_string[cnt]
                if "endfunc" in line:
                    func_ranges[func_name] = (func_start,cnt)
                    break
                else:
                    continue
        else:
            #print(line)
            line = replace_vars(macros_vars, for_vars, for_vals, line)
            line = evaluate_line(line)
            components = re.split(r"\(|\)", line)
            for func_name in func_attributes:
                #print(idx, func_vars[idx])
                if func_name == components[0]:
                    start_line_cnt = func_ranges[func_name][0]
                    end_line_cnt = func_ranges[func_name][1]

                    # #assume one input value
                    # input_vars = {para_names[idx]:components[1]}
                    # print(input_vars)

                    param_idx = 0
                    param_values = components[1].split(",")
                    input_vars = {}
                    for param in func_attributes[func_name]:
                        input_vars[param] = "".join(param_values[param_idx].split(" "))
                        param_idx += 1

                    # In python 3.9.0 or greater:
                    #line = parse_program(program_string[start_line_cnt:end_line_cnt],input_vars | macros_vars)
                    # In Python 3.5 or greater:
                    line = parse_program(program_string[start_line_cnt:end_line_cnt],{**input_vars,** macros_vars})
                    break
            
            result += line + "\n"
        
        cnt += 1
    return result

def find_largest_smaller_than(S, A):
    # Initialize a variable to store the result
    result = None
    # Iterate through the list of ascending numbers
    for number in S:
        # Check if the current number is smaller than A
        if number < A:
            # Update the result if the current number is the largest so far
            result = number

    return str(result)


def replace_jump_addr(program_string):
    # this will only replace those jump operations
    jump_locations = {}
    line_num = 0
    lines = program_string.split("\n")
    replace_def = []
    for line in lines:
        # if there is a jump routine definition
        if (":" in line):
            jump_name = line.split(":")[0]
            line = line.replace(jump_name, str(line_num + 1))
            if (jump_name in jump_locations):
                jump_locations[jump_name].append(line_num + 1)
            else:
                jump_locations[jump_name] = [line_num + 1]
        line_num += 1 
        replace_def.append(line)

    line_num = 0
    replace_instance = []
    for line in replace_def:
        for jump_name in jump_locations:
            if jump_name in line:
                if ("JUMP_COND" in line):
                    line = line.replace(jump_name, find_largest_smaller_than(jump_locations[jump_name], line_num))
                else:
                    if (len(jump_locations[jump_name]) > 1):
                        print("ERROR: Multiple definition of routine not supported")
                        exit(-1)
                    line = line.replace(jump_name, str(jump_locations[jump_name][0]))
        line_num += 1
        replace_instance.append(line)

    ret = ""
    for line in replace_instance:
        ret = ret + line + "\n"

    return ret

def delete_trailing_whitespace(program_string):
    lines = program_string.split("\n");
    ret = ""
    for line in lines:
        if line == "\n" or line == "":
            continue
        ret += line.rstrip() + "\n"
    return ret.rstrip()

if __name__ == '__main__':
    file_addr = sys.argv[1]
    input_size = len(sys.argv)
    outside_var = dict()
    if input_size > 2:
        if(input_size % 2):
            print("command should have even number of inputs!")
            exit()
        for i in range(2,input_size,2):
            outside_var[sys.argv[i]] = int(sys.argv[i+1])
    program_string = fetch_program(file_addr)
    program_string = parse_program(program_string,outside_var)
    program_string = delete_trailing_whitespace(program_string)
    program_string = replace_jump_addr(program_string)
    with open(file_addr[:-4] + ".rc", "w") as file:
        # Write a string to the file
        file.write(program_string)


