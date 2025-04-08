from mininet.topo import Topo
from mininet.net import Mininet
from mininet.link import TCLink
from mininet.node import OVSSwitch, RemoteController
from mininet.cli import CLI
from mininet.log import setLogLevel
import time
import os

class CustomRoutingTopo(Topo):
    def build(self):
        # Create switches
        s1 = self.addSwitch('s1')
        s2 = self.addSwitch('s2')
        s3 = self.addSwitch('s3')
        s4 = self.addSwitch('s4')

        # Create hosts and assign IPs
        h1 = self.addHost('h1', ip='10.0.0.2/24')
        h2 = self.addHost('h2', ip='10.0.0.3/24')
        h3 = self.addHost('h3', ip='10.0.0.4/24')
        h4 = self.addHost('h4', ip='10.0.0.5/24')
        h5 = self.addHost('h5', ip='10.0.0.6/24')
        h6 = self.addHost('h6', ip='10.0.0.7/24')
        h7 = self.addHost('h7', ip='10.0.0.8/24')
        h8 = self.addHost('h8', ip='10.0.0.9/24')

        # Host-Switch Links with 5ms latency
        self.addLink(h1, s1, cls=TCLink, delay='5ms')
        self.addLink(h2, s1, cls=TCLink, delay='5ms')
        self.addLink(h3, s2, cls=TCLink, delay='5ms')
        self.addLink(h4, s2, cls=TCLink, delay='5ms')
        self.addLink(h5, s3, cls=TCLink, delay='5ms')
        self.addLink(h6, s3, cls=TCLink, delay='5ms')
        self.addLink(h7, s4, cls=TCLink, delay='5ms')
        self.addLink(h8, s4, cls=TCLink, delay='5ms')

        # Switch-Switch Links with 7ms latency
        self.addLink(s1, s2, cls=TCLink, delay='7ms')
        self.addLink(s2, s3, cls=TCLink, delay='7ms')
        self.addLink(s3, s4, cls=TCLink, delay='7ms')
        self.addLink(s4, s1, cls=TCLink, delay='7ms')
        self.addLink(s1, s3, cls=TCLink, delay='7ms')  # Diagonal link

def run():
    topo = CustomRoutingTopo()
    net = Mininet(topo=topo, link=TCLink, switch=OVSSwitch)
    net.start()

    print("\nRunning Ping Tests:\n")
    h1, h3 = net.get('h1', 'h3')
    h5, h7 = net.get('h5', 'h7')
    h2, h8 = net.get('h2', 'h8')

    s1, s2, s3, s4 = net.get('s1', 's2', 's3', 's4')
    print("\nEnabling STP on all switches:\n")
    os.system('sudo ovs-vsctl set Bridge s1 stp_enable=true')
    os.system('sudo ovs-vsctl set Bridge s2 stp_enable=true')
    os.system('sudo ovs-vsctl set Bridge s3 stp_enable=true')
    os.system('sudo ovs-vsctl set Bridge s4 stp_enable=true')

    time.sleep(20)
    print("\nRunning pingAll to test connectivity between all hosts:\n")
    net.pingAll()


    for i in range(3):
        with open(f"logs/test_{i+1}_h3_to_h1.log", "w") as log1:
            print(f"\nTest {i+1}: h3 -> h1")
            output = h3.cmd('ping -w 30 ' + h1.IP())
            log1.write(output)
            avg_time = output.splitlines()[-1].split('/')[-3]
            print(f"Average time for h3 -> h1: {avg_time} ms (Output stored in test_{i+1}_h3_to_h1.log)")

        with open(f"logs/test_{i+1}_h5_to_h7.log", "w") as log2:
            print(f"\nTest {i+1}: h5 -> h7")
            output = h5.cmd('ping -w 30 ' + h7.IP())
            log2.write(output)
            avg_time = output.splitlines()[-1].split('/')[-3]
            print(f"Average time for h5 -> h7: {avg_time} ms (Output stored in test_{i+1}_h5_to_h7.log)")

        with open(f"logs/test_{i+1}_h8_to_h2.log", "w") as log3:
            print(f"\nTest {i+1}: h8 -> h2")
            output = h8.cmd('ping -w 30 ' + h2.IP())
            log3.write(output)
            avg_time = output.splitlines()[-1].split('/')[-3]
            print(f"Average time for h8 -> h2: {avg_time} ms (Output stored in test_{i+1}_h8_to_h2.log)")

    CLI(net)
    # net.stop()
# sudo ovs-vsctl set Bridge s4 stp_enable=true
# h2 ping h8 -w 30
# dpctl show

if __name__ == '__main__':
    setLogLevel('info')
    run()
