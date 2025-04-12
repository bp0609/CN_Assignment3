from mininet.topo import Topo
from mininet.net import Mininet
from mininet.link import TCLink
from mininet.node import OVSKernelSwitch, Controller
from mininet.cli import CLI
from mininet.log import setLogLevel
import time
import os

class NATTopo(Topo):
    def build(self):
        # Create switches
        s1 = self.addSwitch('s1')
        s2 = self.addSwitch('s2')
        s3 = self.addSwitch('s3')
        s4 = self.addSwitch('s4')

        # Hosts behind NAT
        h1 = self.addHost('h1', ip='10.1.1.2/24')
        h2 = self.addHost('h2', ip='10.1.1.3/24')
        
        h9 = self.addHost('h9', ip='172.16.10.10/24') # NAT
        
        # External Hosts
        h3 = self.addHost('h3', ip='10.0.0.4/24')
        h4 = self.addHost('h4', ip='10.0.0.5/24')
        h5 = self.addHost('h5', ip='10.0.0.6/24')
        h6 = self.addHost('h6', ip='10.0.0.7/24')
        h7 = self.addHost('h7', ip='10.0.0.8/24')
        h8 = self.addHost('h8', ip='10.0.0.9/24')
        
        # Connecting internal hosts with NAT
        self.addLink(h1, h9, delay='5ms')
        self.addLink(h2, h9, delay='5ms')
        
        # Connecting external hosts with switches
        self.addLink(h3, s2, delay='5ms')
        self.addLink(h4, s2, delay='5ms')
        self.addLink(h5, s3, delay='5ms')
        self.addLink(h6, s3, delay='5ms')
        self.addLink(h7, s4, delay='5ms')
        self.addLink(h8, s4, delay='5ms')
        
        # Connecting NAT to switch
        self.addLink(h9, s1, delay='5ms')
        
        # Switch interconnections (7ms latency)
        self.addLink(s1, s2, delay='7ms')
        self.addLink(s2, s3, delay='7ms')
        self.addLink(s3, s4, delay='7ms')
        self.addLink(s4, s1, delay='7ms')
        self.addLink(s1, s3, delay='7ms')

def STPsetup():
    print("\nEnabling STP on all switches:\n")
    print("Enabling STP on switches s1, s2, s3, and s4...")
    os.system('sudo ovs-vsctl set bridge s1 stp_enable=true')
    os.system('sudo ovs-vsctl set bridge s2 stp_enable=true')
    os.system('sudo ovs-vsctl set bridge s3 stp_enable=true')
    os.system('sudo ovs-vsctl set bridge s4 stp_enable=true')
    print("STP enabled. Waiting for 45 seconds for STP to converge...")
    time.sleep(20)

def run():
    net = Mininet(topo=NATTopo(), link=TCLink, controller=Controller, switch=OVSKernelSwitch)
    net.start()
    STPsetup()
    h1,h2,h3,h4,h5,h6,h7,h8 = net.get('h1','h2','h3','h4','h5','h6','h7','h8')
    CLI(net)
    net.stop()
    

if __name__ == '__main__':
    setLogLevel('info')
    run()