# DCAS Sim
Simulation of a Drone Collision Avoidance System (DCAS) Using a 2-D ASCII Plane

---

## How To Run

*To run this program you must be using a \*nix operating system.*
1. Open command terminal
2. Navigate to folder containing the repo
3. Type the following:
```
make dcas
```
4. A file named "dcas" should now be in the directory
5. Type the following (replacing -arg with the number of drones you would like to spawn)
```
./dcas -arg
```
6. Follow the onscreen instructions to continue with the program
7. Enjoy!

---

## Included Files

- src/dcas.cpp: main program file containing class definition and main function
- include/dcas.h: header file for dcas.cpp
- design.doc: flowchart showing object interaction within the program
- capture: terminal script capture to display steps listed in "How To Run" section
- Makefile: used to compile code into executable file named "dcas"
