
import sst
from sst.merlin import *
from node_endpoint import NODE_Endpoint_Generator

node_params = {
         "config_file" : "./test.config",
         "max_idle" : 1000,
         "num_peers" : 16
         }

# change the program each node executes
binary_dict = {}
binary_dict[0]  = "./demo_ring16/node0.rc"
binary_dict[1]  = "./demo_ring16/node1.rc"
binary_dict[2]  = "./demo_ring16/node2.rc"
binary_dict[3]  = "./demo_ring16/node3.rc"
binary_dict[4]  = "./demo_ring16/node4.rc"
binary_dict[5]  = "./demo_ring16/node5.rc"
binary_dict[6]  = "./demo_ring16/node6.rc"
binary_dict[7]  = "./demo_ring16/node7.rc"
binary_dict[8]  = "./demo_ring16/node8.rc"
binary_dict[9]  = "./demo_ring16/node9.rc"
binary_dict[10]  = "./demo_ring16/node10.rc"
binary_dict[11]  = "./demo_ring16/node11.rc"
binary_dict[12]  = "./demo_ring16/node12.rc"
binary_dict[13]  = "./demo_ring16/node13.rc"
binary_dict[14]  = "./demo_ring16/node14.rc"
binary_dict[15]  = "./demo_ring16/node15.rc"

#Change any of the parameters
sst.merlin._params["flit_size"] = "16B"
sst.merlin._params["link_bw"] = "1.0GB/s" #CHANGE ME (4 -> 1)
sst.merlin._params["xbar_bw"] = "1.0GB/s" #CHANGE ME (4 -> 1)
sst.merlin._params["input_latency"] = "0.0ns"
sst.merlin._params["output_latency"] = "0.0ns"
sst.merlin._params["input_buf_size"] = "16.0KB"
sst.merlin._params["output_buf_size"] = "16.0KB"
sst.merlin._params["link_lat"] = "3200ps" #CHANGE ME (800 -> 3200)

#Change the topology
merlinmeshparams = {}
merlinmeshparams["mesh.shape"]="1x16" # 1-D mesh
merlinmeshparams["mesh.width"]="1x1"
merlinmeshparams["mesh.local_ports"]="1"
sst.merlin._params.update(merlinmeshparams)
topo = topoMesh()
topo.prepParams()

endPoint = NODE_Endpoint_Generator(node_params, binary_dict)
endPoint.prepParams()

topo.setEndPoint(endPoint)
topo.build()
