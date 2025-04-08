from mininet.topo import Topo
from mininet.net import Mininet
from mininet.link import TCLink
from mininet.node import Node
from mininet.cli import CLI
from mininet.log import setLogLevel

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
        h9 = self.addHost('h9', ip='172.16.10.10/24')  # NAT box

        # External hosts
        h3 = self.addHost('h3', ip='10.0.0.4/24')
        h4 = self.addHost('h4', ip='10.0.0.5/24')
        h5 = self.addHost('h5', ip='10.0.0.6/24')
        h6 = self.addHost('h6', ip='10.0.0.7/24')
        h7 = self.addHost('h7', ip='10.0.0.8/24')
        h8 = self.addHost('h8', ip='10.0.0.9/24')

        # Connect hosts to switches
        self.addLink(h3, s2, cls=TCLink, delay='5ms')
        self.addLink(h4, s2, cls=TCLink, delay='5ms')
        self.addLink(h5, s3, cls=TCLink, delay='5ms')
        self.addLink(h6, s3, cls=TCLink, delay='5ms')
        self.addLink(h7, s4, cls=TCLink, delay='5ms')
        self.addLink(h8, s4, cls=TCLink, delay='5ms')

        # Host-to-NAT connections
        self.addLink(h1, h9, cls=TCLink, delay='5ms')
        self.addLink(h2, h9, cls=TCLink, delay='5ms')

        # NAT to switch
        self.addLink(h9, s1, cls=TCLink, delay='5ms')

        # Switch interconnections (7ms latency)
        self.addLink(s1, s2, cls=TCLink, delay='7ms')
        self.addLink(s2, s3, cls=TCLink, delay='7ms')
        self.addLink(s3, s4, cls=TCLink, delay='7ms')
        self.addLink(s4, s1, cls=TCLink, delay='7ms')
        self.addLink(s1, s3, cls=TCLink, delay='7ms')

def run():
    net = Mininet(topo=NATTopo(), link=TCLink, controller=None)
    net.start()

    # Assign routes
    h1, h2, h3, h5, h6, h8, h9 = net.get('h1', 'h2', 'h3', 'h5', 'h6', 'h8', 'h9')

    # Enable IP forwarding in h9
    h9.cmd('sysctl -w net.ipv4.ip_forward=1')

    # Configure NAT for private IPs (SNAT)
    h9.cmd('iptables -t nat -A POSTROUTING -s 10.1.1.0/24 -o h9-eth2 -j SNAT --to-source 172.16.10.10')

    # Add routes on h1 and h2
    h1.cmd('ip route add default via 10.1.1.1')  # Peer IP on h9-eth0
    h2.cmd('ip route add default via 10.1.1.1')

    # Assign internal IPs on h9 interfaces
    h9.setIP('10.1.1.1/24', intf='h9-eth0')
    h9.setIP('10.1.1.1/24', intf='h9-eth1')  # h2
    # Public IP already on h9-eth2 via topology

    CLI(net)
    net.stop()

if __name__ == '__main__':
    setLogLevel('info')
    run()
