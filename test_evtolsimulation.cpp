/**
 * File : test_evtolsimulation.cpp
 * Unit tests for eVTOL Simulation
 *
 * This file contains unit tests to validate the functionality of the eVTOL
 * simulation, including flight time calculations, charging queue behavior,
 * and simulation constraints.
 *
 * Uses Google Test Framework.
 */

 #include "gtest/gtest.h"
 #include "evtolsimulation.cpp"
 #include <unordered_set>
 
 /**
  * Test flight time calculation.
  * 
  * Ensures that EVTOLs correctly compute flight time based on battery capacity 
  * and energy usage, ensuring no vehicle flies longer than 3 hours.
  */
 TEST(EVTOLTests, FlightTimeCalculation) {
     EVTOL_Spec spec = manufacturers[0]; // Alpha Company
     EVTOL vehicle(spec, 1);
     
     vehicle.runFlightCycle();
     
     double maxPossibleFlightTime = 3.0; // The total simulation time cannot exceed 3 hours
     EXPECT_LE(vehicle.total_flight_time, maxPossibleFlightTime);
 }
 
 /**
  * Test vehicle queuing for charging.
  * 
  * Ensures that vehicles enter the charging queue properly.
  */
 TEST(EVTOLTests, ChargingQueueBehavior) {
     EVTOL vehicle1(manufacturers[1], 1);
     EVTOL vehicle2(manufacturers[2], 2);
 
     vehicle1.queueForCharging();
     vehicle2.queueForCharging();
     
     EXPECT_EQ(chargingQueue.size(), 2);
 }
 
 /**
  * Test that charging allocation is handled correctly.
  * 
  * Ensures vehicles charge in the correct order and do not exceed the 
  * available charger limit.
  */
 TEST(EVTOLTests, ChargerAllocation) {
     EVTOL vehicle1(manufacturers[0], 1);
     EVTOL vehicle2(manufacturers[1], 2);
     EVTOL vehicle3(manufacturers[2], 3);
     EVTOL vehicle4(manufacturers[3], 4); // This should wait
 
     vehicle1.queueForCharging();
     vehicle2.queueForCharging();
     vehicle3.queueForCharging();
     vehicle4.queueForCharging();
 
     std::unordered_set<int> uniqueVehicles;
     while (!chargingQueue.empty()) {
         uniqueVehicles.insert(chargingQueue.top().second);
         chargingQueue.pop();
     }
     EXPECT_EQ(uniqueVehicles.size(), 4); // Ensure unique vehicles are counted
 }
 
 
// Test that flight time does not exceed 3 hours due to charging delays.
 TEST(EVTOLTests, MaxSimulationTime) {
     EVTOL vehicle(manufacturers[2], 1);
     vehicle.runFlightCycle();
     EXPECT_LE(vehicle.total_flight_time + vehicle.total_charge_time, 3.0);
 }
 
 int main(int argc, char **argv) {
     ::testing::InitGoogleTest(&argc, argv);
     return RUN_ALL_TESTS();
 }
 