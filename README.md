# CN_Assignment3

## Instructions to Run the Code

1. **Question 1**  
   Run the script for Question 1 using:
   ```bash
   sudo python3 intial_routing_topology.py
   ```
   To fix the initial routing topology, run the below python script.
   ```bash
   sudo python3 generate_results_script.py
   ```
   The logs are saved in the `logs` directory.
2. **Question 2**
   Run the script for Question 2 using:
   ```bash
   sudo python3 initial_topo.py
   ```
   To fix the initial topology and configure NAT, run the below python file.
   ````bash
     sudo python3 NAT_Configured_topo.py
     ```
   ````
3. **Question 3**
   To perform asynchronous distance vector routing, first compile the below code.
   ```bash
   cc distance_vector.c node0.c node1.c node2.c node3.c -o dv_routing
   ```
   Then run the generated executable file using:
   ```bash
   ./dv_routing
   ```
   The program will ask for an input TRACE, we can enter the trace to be **2**.
