# eVTOL Simulation

## Overview

This project simulates a fleet of 20 **electric vertical takeoff and landing (eVTOL) aircraft** operating for a **3-hour window**. Each eVTOL:

- Flies until the battery is depleted.
- Queues for a **limited number of 3 chargers**.
- Resumes flying after charging if time allows.
- Tracks **flight time, charging time, distance traveled, faults, and passenger miles**.

The program demonstrates **OOP principles, multithreading, and resource management**, with an emphasis on testability and modularity.

---

##  Features

- **Object-Oriented Design (OOP)**
  - `EVTOL` class represents individual aircraft behavior.
  - `SimulationManager` class handles execution and statistics.
- **Threading & Synchronization**
  - **Multithreading** for concurrent vehicle operations.
  - **Condition Variables & Mutexes** ensure correct charging behavior.
- **Priority Queue for Charging**
  - Vehicles **running out of battery first get priority** for charging.
- **Realistic Flight & Charging Cycle**
  - **Not all vehicles can charge due to the 3-hour limit.**
  - Vehicles may **stay grounded if they miss charging opportunities**.
- **Unit Testing with Google Test (gtest)**
  - Test cases validate **flight time calculations, queuing logic, and simulation constraints**.

---

## Installation & Compilation

### **Prerequisites**

Ensure you have the following installed:

- **C++ Compiler** (`g++` or `clang++`)
- **Google Test Framework** (for unit testing)

### **Build & Run Simulation**

```sh
# Compile the simulation
clang++ -std=c++14 evtolsimulation.cpp -o evtolsim -pthread

# Run the simulation
./evtolsim
```
## Output is saved in output.txt file with multiple simulations


### **Run Unit Tests**

```sh
# Compile with Google Test
g++ -std=c++14 test_evtolsimulation.cpp -o test_evtol -lgtest -lgtest_main -pthread -DUNIT_TEST

# Run tests
./test_evtol
```
## Output for Unit test
##  Unit Tests Includes

- **Flight Time Calculation**: Ensures EVTOLs calculate flight duration correctly.
- **Charging Queue Handling**: Checks correct queuing behavior.
- **Charger Allocation**: Ensures charging works properly under contention.

##  Test Results
The simulation has been validated using **Google Test (gtest)** to ensure correctness in flight time, charging behavior, and simulation constraints.

###  Running Tests
After compilation, running the test executable produces the following results:
```sh
[==========] Running 4 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 4 tests from EVTOLTests
[ RUN      ] EVTOLTests.FlightTimeCalculation
[       OK ] EVTOLTests.FlightTimeCalculation (605 ms)
[ RUN      ] EVTOLTests.ChargingQueueBehavior
[       OK ] EVTOLTests.ChargingQueueBehavior (0 ms)
[ RUN      ] EVTOLTests.ChargerAllocation
[       OK ] EVTOLTests.ChargerAllocation (0 ms)
[ RUN      ] EVTOLTests.MaxSimulationTime
[       OK ] EVTOLTests.MaxSimulationTime (1606 ms)
[----------] 4 tests from EVTOLTests (2211 ms total)

[==========] 4 tests from 1 test suite ran. (2211 ms total)
[  PASSED  ] 4 tests.

---

## Code Structure

```
 evtol-simulation
 ├── evtolsimulation.cpp      # Main simulation code
 ├── test_evtolsimulation.cpp # Unit test file
 ├── README.md                # Documentation
```

---


