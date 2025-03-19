/**
 * File: evtolsimulation.cpp
 * Simulates eVTOL aircraft operations for a 3-hour period.
 *
 * This program models the behavior of 20 eVTOLs from different companies.
 * Each vehicle flies until its battery is depleted, then joins a charging queue.
 * The simulation ensures fair charger allocation, tracks faults, and computes stats.
 *
 * The code follows OOP principles, with classes for EVTOL vehicles and a 
 * SimulationManager to control execution.
 *
 * @author Ashish Chittimilla
 * @date March 17th 2025
 */

#include <iostream>
#include <vector>
#include <random>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <queue>
#include <algorithm>


// Struct to define eVTOL vehicle properties
struct EVTOL_Spec {
    std::string company;
    double cruise_speed;  // mph
    double battery_capacity;  // kWh
    double charge_time;  // hours
    double energy_use;  // kWh/mile
    int passenger_count;
    double fault_probability;
};

// Manufacturer data
std::vector<EVTOL_Spec> manufacturers = {
    {"Alpha Company", 120, 320, 0.6, 1.6, 4, 0.25},
    {"Bravo Company", 100, 100, 0.2, 1.5, 5, 0.10},
    {"Charlie Company", 160, 220, 0.8, 2.2, 3, 0.05},
    {"Delta Company", 90, 120, 0.62, 0.8, 2, 0.22},
    {"Echo Company", 30, 150, 0.3, 5.8, 2, 0.61}
};

// Synchronization for charging stations
std::mutex charger_mutex;
std::condition_variable charger_cv;
int available_chargers = 3;

// Charging queue (vehicles that need charging)
std::priority_queue<std::pair<double, int>, std::vector<std::pair<double, int>>, std::greater<>> chargingQueue;
std::mutex queue_mutex;

/**
 * Class EVTOL : Represents an electric vertical takeoff and landing (eVTOL) vehicle.
 *
 * This class models an individual eVTOL's flight, battery usage, and charging behavior.
 * Each EVTOL operates within a 3-hour window, flying until its battery depletes, 
 * then queuing for charging.
 */
class EVTOL {
public:
    EVTOL_Spec spec;
    int vehicle_id;
    double total_flight_time = 0;
    double total_distance_traveled = 0;
    double total_charge_time = 0;
    int total_faults = 0;
    double total_passenger_miles = 0;
    double simulation_time = 3.0; // Simulation window of 3 hours

    EVTOL(EVTOL_Spec spec, int id) : spec(spec), vehicle_id(id) {}

    /**
     * Simulates EVTOL behavior within 3 hours, including flight and charging.
     */
    void runFlightCycle() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> random_prob(0.0, 1.0);

        while (simulation_time > 0) {
            // Compute how long this vehicle can fly before battery depletion
            double max_flight_time = spec.battery_capacity / (spec.energy_use * spec.cruise_speed);
            max_flight_time = std::min(max_flight_time, simulation_time);

            // Update flight statistics
            total_flight_time += max_flight_time;
            double distance = max_flight_time * spec.cruise_speed;
            total_distance_traveled += distance;
            total_passenger_miles += spec.passenger_count * distance;
            simulation_time -= max_flight_time;

            // Fault calculation
            for (double hour = 0; hour < max_flight_time; hour += 1.0) {
                if (random_prob(gen) < spec.fault_probability) {
                    total_faults++;
                }
            }

            // If there's still simulation time left, queue for charging
            if (simulation_time > 0) {
                queueForCharging();
                if (!charge()) break; // If we couldn't charge in time, stop
            }
        }
    }

    
    //Adds the EVTOL to the charging queue.
    void queueForCharging() {
        std::lock_guard<std::mutex> lock(queue_mutex);
        chargingQueue.push({simulation_time, vehicle_id});
    }

    /**
     * Simulates the charging process.
     * returns True if charging was successful, false otherwise.
     */
    bool charge() {
        std::unique_lock<std::mutex> lock(charger_mutex);

        // Wait for available charger
        bool got_charger = charger_cv.wait_for(lock, std::chrono::milliseconds(100), [this] {
            return available_chargers > 0 && !chargingQueue.empty() && chargingQueue.top().second == vehicle_id;
        });

        if (!got_charger) return false; // If we couldn't charge in time, we stop here

        // Take a charger
        available_chargers--;
        chargingQueue.pop(); // Remove from queue
        lock.unlock();

        // Simulate charging
        double charging_duration = std::min(spec.charge_time, simulation_time);
        if (charging_duration + total_flight_time > 3.0) return false; // If charge exceeds 3-hour window, stop

        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(charging_duration * 1000)));
        total_charge_time += charging_duration;
        simulation_time -= charging_duration;

        // Release charger
        lock.lock();
        available_chargers++;
        lock.unlock();
        charger_cv.notify_all();

        return true;
    }

    void run() {
        runFlightCycle();
    }

    /**
     * Prints the statistics of the EVTOL's flight and charging performance.
     */
    void printStats() const {
        std::cout << "Vehicle ID: " << vehicle_id << " | Company: " << spec.company << "\n";
        std::cout << "  Total Flight Time: " << total_flight_time << " hours\n";
        std::cout << "  Total Distance: " << total_distance_traveled << " miles\n";
        std::cout << "  Total Charge Time: " << total_charge_time << " hours\n";
        std::cout << "  Total Faults: " << total_faults << "\n";
        std::cout << "  Total Passenger Miles: " << total_passenger_miles << " miles\n";
        std::cout << "-----------------------------------\n";
    }
};

/**
 * Class SimulationManager : Manages simulation execution for multiple EVTOLs.
 */
class SimulationManager {
public:
    std::vector<EVTOL> vehicles;
    
    
    //Deploys 20 randomly chosen EVTOLs from different manufacturers.
    void deployVehicles() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(0, 4);

        // Deploy 20 random vehicles
        for (int i = 0; i < 20; i++) {
            EVTOL_Spec spec = manufacturers[dist(gen)];
            vehicles.emplace_back(spec, i + 1);
        }
    }

    //Starts the simulation by launching threads for each EVTOL.
    void startSimulation() {
        std::vector<std::thread> threads;

        for (auto &vehicle : vehicles) {
            threads.push_back(std::thread(&EVTOL::run, &vehicle));
        }

        for (auto &t : threads) {
            if (t.joinable()) t.join();
        }
    }

    
    //Prints simulation results.
    void printResults() {
        std::cout << "Simulation Results:\n";
        for (const auto &vehicle : vehicles) {
            vehicle.printStats();
        }
    }
};


int main() {
    SimulationManager sim;
    sim.deployVehicles();
    sim.startSimulation();
    sim.printResults();
    return 0;
}

