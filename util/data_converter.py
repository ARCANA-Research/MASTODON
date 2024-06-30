######################## CAUTION #############################
# STOP USING THIS UTILITY FUNCTION
# THE ASSOCIATED FUNCTION WITH THIS FILE init_node_user_input HAS BEEN RETIRED
# USE THE XML FORMAT AS INPUT INSTEAD
##############################################################


# ain't nobody got time to write this in c++
# Convert a human human_readable data file into something that can be fed to the Node
import sys
def fetch_program(file_addr):
    program_string = []
    return_list = []
    with open(file_addr, "r") as file:
        for line in file:
            if not line.isspace():
                program_string.append(line.strip())
            if "#" in line:
                return_list.append(program_string)
                program_string = []
    return return_list

def transpose_data(cluster_data, R, C):
    cluster_data_t = []
    for pipeline_data in cluster_data:
        pipeline_data_t = [None] * (R*C)
        for i in range(R):
            for j in range(C):
                pipeline_data_t[i + R * j] = pipeline_data[j + C * i]
        cluster_data_t.append(pipeline_data_t)
    return cluster_data_t

def convert_to_binary(cluster_data_t, G):
    cluster_data_t_bin = []
    for pipeline_data_t in cluster_data_t:
        pipeline_data_t_bin = []
        for value in pipeline_data_t:
            pipeline_data_t_bin.append(format(value, '0'+G+'b'))
        cluster_data_t_bin.append(pipeline_data_t_bin)
    return cluster_data_t_bin

def save_string(cluster_data_t_bin, file_addr):
    final_string = ''
    for pipeline_data_t_bin in cluster_data_t_bin:
        for bin in pipeline_data_t_bin:
            final_string += bin
    with open(file_addr, 'a') as file:
        file.write(final_string)

if __name__ == '__main__':
    in_file_addr = sys.argv[1]
    R = int(sys.argv[2]) # num row in tile
    C = int(sys.argv[3]) # num column in tile
    G = sys.argv[4] #granulairty
    raw = fetch_program(in_file_addr)
    for lanes in raw:
        cluster_data = []
        pipeline_data = []
        for line in lanes:
            if (line == '*'):
                data_string = (" ".join(pipeline_data)).split(" ")
                cluster_data.append([int(x) for x in data_string])
                pipeline_data = []
            else:
                pipeline_data.append(line)
        cluster_data_t = transpose_data(cluster_data, R, C)
        cluster_data_t_bin = convert_to_binary(cluster_data_t, G)
        save_string(cluster_data_t_bin,in_file_addr[:-10] + "node.data")
