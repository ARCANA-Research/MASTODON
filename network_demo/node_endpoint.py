import sst
from sst.merlin.base import *
from sst.merlin.endpoint import *
from sst.merlin.interface import *
from sst.merlin.topology import *
from sst.merlin import *

class NODE_Endpoint_Generator(EndPoint):
    def __init__(self, node_prams, binary_dict):
        EndPoint.__init__(self)
        self.epKeys.extend("link_bw")
        self.node_prams = node_prams
        self.binary_dict = binary_dict

    def getName(self):
        return "Node endpoint generator for SST"
    def prepParams(self):
        pass

    def build(self, nID, extraKeys):
        node = sst.Component("node_racer_%d"%nID, "mastodon.node_racer")
        linkif = node.setSubComponent("networkIF", "merlin.linkcontrol")
        linkif.addParam("link_bw", sst.merlin._params["link_bw"])
        node.addParams(self.node_prams)
        node.addParam("network_id" ,nID)
        node.addParam("binary_file", self.binary_dict[nID])
        return (linkif, "rtr_port", sst.merlin._params["link_lat"])
