#include "TestBed.h"
#include <thread>
#include <random>
using seconds = std::chrono::duration<double>;
#define km_conversion pow(10, -6)   / 3600 

double randomNumber() {
    std::mt19937 generator(static_cast<unsigned int>(std::time(0)));
    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    return distribution(generator);
}
void checkGear(Car* car) {
    double speedRandomizer = randomNumber();
    double speed = car->getSpeed();
    if (car->getTachometer() <= 1500 && car->getGear() == 2)
    {
        car->setGear(1);
    }
    if ((speed > 20 + speedRandomizer * 5 && speed < 35 + speedRandomizer * 5) || (car->getGear() == 3 && car->getTachometer() <= 1500) || (car->getGear() == 1 && car->getTachometer() >= 3000)) {
        car->setGear(2);
        return;
    }

    if ((speed >= 35 + speedRandomizer * 5 && speed < 55 + speedRandomizer * 5) || (car->getGear() == 4 && car->getTachometer() <= 1500) || (car->getGear() == 2 && car->getTachometer() >= 3000)) {
        car->setGear(3);
        return;
    }

    if ((speed >= 55 + speedRandomizer * 5 && speed < 75 + speedRandomizer * 5) || (car->getGear() == 5 && car->getTachometer() <= 1500) || (car->getGear() == 3 && car->getTachometer() >= 3000)) {
        car->setGear(4);
        return;
    }

    if (speed >= 75 + speedRandomizer * 5 || (car->getGear() == 4 && car->getTachometer() >= 3000))
    {
        car->setGear(5);
        return;
    }
}

void someoneExitsCar(Car* car) {
    int slowDownTime = 166 + 300 * randomNumber();
    while (car->getSpeed() > 0)
    {
        auto iteration_start = std::chrono::system_clock::now();
        car->slowDown();
        std::this_thread::sleep_for(std::chrono::milliseconds(slowDownTime));
        checkGear(car);
        car->setOdometer(car->getOdometer() + (std::chrono::system_clock::now() - iteration_start).count() * car->getSpeed() * km_conversion);
        car->calculateFuelSpent((std::chrono::system_clock::now() - iteration_start).count() * car->getSpeed() * km_conversion);
    }
    car->turnStoppedLightsOn();
    car->setBeltLight(true);
    int exitTime = 10000 + randomNumber() * 30000;
    std::this_thread::sleep_for(std::chrono::milliseconds(exitTime));
    car->setDoorOpenLight(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    car->setDoorOpenLight(false);
    car->turnStoppedLightsOff();
}

void turn(Car* car)
{
    double direction = randomNumber();
    direction > 0.5 ? car->setTurnLeftSignal(true) : car->setTurnRightSignal(true);
    double turnSpeed = 10 + 15 * randomNumber();
    int slowDownTime = 166 + 300 * randomNumber();
    //Car is moving too fast to make a turn
    while (car->getSpeed() > turnSpeed)
    {
        auto iteration_start = std::chrono::system_clock::now();
        car->slowDown();
        std::this_thread::sleep_for(std::chrono::milliseconds(slowDownTime));
        checkGear(car);
        car->setOdometer(car->getOdometer() + (std::chrono::system_clock::now() - iteration_start).count() * car->getSpeed() * km_conversion);
        car->calculateFuelSpent((std::chrono::system_clock::now() - iteration_start).count() * car->getSpeed() * km_conversion);
    }
    //car was slowing down or stopped and is now making a turn
    while (car->getSpeed() < turnSpeed)
    {
        auto iteration_start = std::chrono::system_clock::now();
        car->speedUp();
        std::this_thread::sleep_for(std::chrono::milliseconds(slowDownTime));
        checkGear(car);
        car->setOdometer(car->getOdometer() + (std::chrono::system_clock::now() - iteration_start).count() * car->getSpeed() * km_conversion);
        car->calculateFuelSpent((std::chrono::system_clock::now() - iteration_start).count() * car->getSpeed() * km_conversion);
    }
    //time it takes to make a turn
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    direction > 0.5 ? car->setTurnLeftSignal(false) : car->setTurnRightSignal(false);

}

void overtake(Car* car)
{
    double overtakingSpeed = car->getSpeed() + car->getSpeed() * (0.5 - 0.1 * randomNumber() - 0.07 * car->getGear());
    double overtakingDuration = 166 + 166 * randomNumber();
    car->setTurnLeftSignal(true);
    while (car->getSpeed() < overtakingSpeed)
    {
        auto iteration_start = std::chrono::system_clock::now();
        car->speedUp();
        std::this_thread::sleep_for(std::chrono::milliseconds((int)overtakingDuration));
        checkGear(car);
        car->setOdometer(car->getOdometer() + (std::chrono::system_clock::now() - iteration_start).count() * car->getSpeed() * km_conversion);
        car->calculateFuelSpent((std::chrono::system_clock::now() - iteration_start).count() * car->getSpeed() * km_conversion);
    }
    car->setTurnLeftSignal(false);
    car->setTurnRightSignal(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    car->setTurnRightSignal(false);
    car->setOdometer(car->getOdometer() + (3 * pow(10, 7) * car->getSpeed() * km_conversion));
    car->calculateFuelSpent(3000000 * car->getSpeed() * km_conversion);
}
void speedUp(Car* car)
{
    car->speedUp();
    checkGear(car);
}
void slowDown(Car* car) {
    car->slowDown();
    checkGear(car);
}
void testWarningLightsAtCarStart(Car* car)
{
    car->setAbsLight(true);
    car->setCheckEngine(true);
    car->setAirbagLight(true);
    car->setBatteryLight(true);
    car->setBeltLight(true);
    car->setBrakeWarningLight(true);
    car->setCheckEngine(true);
    car->setCoolantLight(true);
    car->setLightNotWorking(true);
    car->setLowTirePressureLight(true);
    car->setOilPressureLow(true);
    car->setBrakeActivated(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(4000));
    double abs_working_probability = randomNumber();
    if (abs_working_probability > 0.008)
    {
        car->setAbsLight(false);
    }
    double engine_working_probability = randomNumber();
    if (engine_working_probability > 0.02)
    {
        car->setCheckEngine(false);
    }
    double airbag_working_probability = randomNumber();
    if (abs_working_probability > 0.002)
    {
        car->setAirbagLight(false);
    }
    double battery_working_probability = randomNumber();
    if (battery_working_probability > 0.005)
    {
        car->setBatteryLight(false);
    }
    double fastened_belt_probability = randomNumber();
    if (fastened_belt_probability > 0.01)
    {
        car->setBeltLight(false);
    }
    double brake_working_probability = randomNumber();
    if (brake_working_probability > 0.002)
    {
        car->setBrakeWarningLight(false);

    }
    double coolant_normal_probability = randomNumber();
    if (coolant_normal_probability > 0.002)
    {
        car->setCoolantLight(false);
    }
    double lights_working_probability = randomNumber();
    if (lights_working_probability > 0.01)
    {
        car->setLightNotWorking(false);
    }
    double tire_pressure_good_probability = randomNumber();
    if (tire_pressure_good_probability > 0.008)
    {
        car->setLowTirePressureLight(false);
    }
    double oil_pressure_good_probability = randomNumber();
    if (oil_pressure_good_probability > 0.006)
    {
        car->setOilPressureLow(false);
    }
    car->setBrakeActivated(false);
}
void runSimulation(Car* car) {
    car->setIsEngineOn(true);
    if (car->getGear() == 0) {
        double door_not_closed_properly = randomNumber();
        if (door_not_closed_properly < 0.05)
        {
            car->setDoorOpenLight(true);
            std::this_thread::sleep_for(std::chrono::milliseconds(5000));
            car->setDoorOpenLight(false);
        }
        if (car->getSpeed() == 0)
        {
            testWarningLightsAtCarStart(car);
            car->setGear(1);
        }

        while (car->getSpeed() < 30)
        {
            auto iteration_beginnign_time = std::chrono::system_clock::now();
            car->speedUp();
            std::this_thread::sleep_for(std::chrono::milliseconds(166));
            car->setOdometer(car->getOdometer() + car->getSpeed() * (std::chrono::system_clock::now() - iteration_beginnign_time).count() * km_conversion);
            car->calculateFuelSpent(car->getSpeed() * (std::chrono::system_clock::now() - iteration_beginnign_time).count() * km_conversion);
        }

    }
    while (car->getIsEngineOn()) {
        auto last_odometer_calculation_time = std::chrono::system_clock::now();
        auto last_fuel_calculation_time = std::chrono::system_clock::now();
        checkGear(car);
        double speedUpChance = randomNumber();
        if (speedUpChance > (0.70)) {
            if (speedUpChance <= 1 - (car->getGear()) * 0.05) {
                std::this_thread::sleep_for(std::chrono::milliseconds(166));
                car->speedUp();
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(166));
                car->slowDown();
            }
            checkGear(car);
        }
        //brake is activated
        double brakeChance = randomNumber();
        if (brakeChance < 0.035) {
            car->setBrakeActivated(true);
            double slowDownAmount = randomNumber();
            int slowDownTime = 166 + 300 * randomNumber();
            for (int i = 0; i <= 5 + (int)car->getSpeed() * 0.7 * slowDownAmount; i++) {
                std::this_thread::sleep_for(std::chrono::milliseconds(slowDownTime));
                car->slowDown();
            }
            checkGear(car);
            car->setBrakeActivated(false);
        }
        //belt taken off while car is working
        double change_belt_status_chance = randomNumber();
        if (change_belt_status_chance < 0.008)
        {
            if (change_belt_status_chance < 0.005) {
                someoneExitsCar(car);
            }
            car->setBeltLight(!car->getBeltLight());
        }
        //faulty brakes
        double brakes_faulty_probability = randomNumber();
        if (!car->getBrakeWarningLight() && brakes_faulty_probability < 0.0045)
        {
            car->setOdometer(car->getOdometer() + car->getSpeed() * (std::chrono::system_clock::now() - last_odometer_calculation_time).count() * km_conversion);
            car->setBrakeWarningLight(true);
            last_odometer_calculation_time = std::chrono::system_clock::now();
        }
        //headlights
        double strong_headlight_change_probability = randomNumber();
        if (strong_headlight_change_probability < 0.015)
        {
            car->setStrongHeadLight(!car->getStrongHeadLight());
        }
        double abs_working_probability = randomNumber();
        if (abs_working_probability < 0.004)
        {
            car->setAbsLight(true);
        }
        double engine_problems = randomNumber();
        if (engine_problems < 0.008)
        {
            car->setCheckEngine(true);
        }
        double oilPressureLow = randomNumber();
        if (oilPressureLow < 0.008)
        {
            car->setOilPressureLow(true);
        }
        double lightNotWorking = randomNumber();
        if (lightNotWorking < 0.01)
        {
            car->setLightNotWorking(true);
        }
        //turn
        double turnChance = randomNumber();
        if (turnChance < 0.08 - (car->getGear() - 1) * 0.015)
        {
            car->setOdometer(car->getOdometer() + car->getSpeed() * (std::chrono::system_clock::now() - last_odometer_calculation_time).count() * km_conversion);
            car->calculateFuelSpent(car->getSpeed() * (std::chrono::system_clock::now() - last_fuel_calculation_time).count() * km_conversion);
            turn(car);
            last_odometer_calculation_time = std::chrono::system_clock::now();
            last_fuel_calculation_time = std::chrono::system_clock::now();
        }
        double overtakeChance = randomNumber();
        if (overtakeChance < 0.03)
        {
            car->setOdometer(car->getOdometer() + car->getSpeed() * (std::chrono::system_clock::now() - last_odometer_calculation_time).count() * km_conversion);
            car->calculateFuelSpent(car->getSpeed() * (std::chrono::system_clock::now() - last_fuel_calculation_time).count() * km_conversion);
            overtake(car);
            last_odometer_calculation_time = std::chrono::system_clock::now();
            last_fuel_calculation_time = std::chrono::system_clock::now();
        }
        car->setOdometer(car->getOdometer() + car->getSpeed() * (std::chrono::system_clock::now() - last_odometer_calculation_time).count() * km_conversion);
        car->calculateFuelSpent(car->getSpeed() * (std::chrono::system_clock::now() - last_fuel_calculation_time).count() * km_conversion);
        last_fuel_calculation_time = std::chrono::system_clock::now();
        double batteryProblemsProbability = randomNumber();
        if (batteryProblemsProbability < 0.02) {
            car->setBatteryLight(true);
        }

        double airbagProblemsProbability = randomNumber();
        if (airbagProblemsProbability < 0.001) {
            car->setAirbagLight(true);
        }

        double lowTireProbability = randomNumber();
        if (lowTireProbability < 0.018) {
            car->setLowTirePressureLight(true);
        }
    }

}

Car getCar()
{

    return  Car();
}
void startSimulation(Car* car)
{
    std::thread simuation(runSimulation, car);
    simuation.detach();

}
void endSimulation(Car* car)
{
    car->setIsEngineOn(false);
}
