import sst
from sst.merlin import *
from node_endpoint import NODE_Endpoint_Generator

node_params = {
         "config_file" : "./test.config",
         "max_idle" : 1000,
         "num_peers" : 16
         }

# change the binary accordingly
binary_dict = {}
binary_dict[0]  = "./demo_networks/simple_send.rc"
for i in range (1,15):
    binary_dict[i] = "./demo_networks/simple_add.rc"
binary_dict[15]  = "./demo_networks/simple_recv.rc"


sst.merlin._params["flit_size"] = "16B"
sst.merlin._params["link_bw"] = "4.0GB/s"
sst.merlin._params["xbar_bw"] = "4.0GB/s"
sst.merlin._params["input_latency"] = "0.0ns"
sst.merlin._params["output_latency"] = "0.0ns"
sst.merlin._params["input_buf_size"] = "16.0KB"
sst.merlin._params["output_buf_size"] = "16.0KB"
sst.merlin._params["link_lat"] = "800ps"

merlintorusparams = {}
merlintorusparams["torus.shape"]="4x4"
merlintorusparams["torus.width"]="1x1"
merlintorusparams["torus.local_ports"]="1"
sst.merlin._params.update(merlintorusparams)
topo = topoTorus()
topo.prepParams()

endPoint = NODE_Endpoint_Generator(node_params, binary_dict)
endPoint.prepParams()

topo.setEndPoint(endPoint)
topo.build()
