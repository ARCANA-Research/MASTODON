
import sst
from sst.merlin import *
from node_endpoint import NODE_Endpoint_Generator

node_params = {
         "config_file" : "./test.config",
         "max_idle" : 1000,
         "num_peers" : 2
         }

# change the binary accordingly
binary_dict = {}
binary_dict[0]  = "./demo_basic/simple_send.rc"
binary_dict[1]  = "./demo_basic/simple_recv.rc"

#Change any merlin._params value
sst.merlin._params["flit_size"] = "16B"
sst.merlin._params["link_bw"] = "1.0GB/s" #We pre-changed this
sst.merlin._params["xbar_bw"] = "1.0GB/s" #We pre-changed this
sst.merlin._params["input_latency"] = "0.0ns"
sst.merlin._params["output_latency"] = "0.0ns"
sst.merlin._params["input_buf_size"] = "16.0KB"
sst.merlin._params["output_buf_size"] = "16.0KB"
sst.merlin._params["link_lat"] = "3200ps" #We pre-changed this

merlinmeshparams = {}
merlinmeshparams["mesh.shape"]="1x2"
merlinmeshparams["mesh.width"]="1x1"
merlinmeshparams["mesh.local_ports"]="1"
sst.merlin._params.update(merlinmeshparams)
topo = topoMesh()
topo.prepParams()

endPoint = NODE_Endpoint_Generator(node_params, binary_dict)
endPoint.prepParams()

topo.setEndPoint(endPoint)
topo.build()
