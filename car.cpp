#include "Car.h"
Car::Car() {
    gear = 0;
    speedometer = 0;
    odometer = 0;
    fuelAmount = maxFuelAmount;
    engineTemperature = 0;
    tirePressure = 0;
    tachometer = 0;
    isEngineOn = false;
    //Warning lights
    absLight = false;
    strongHeadLight = false;
    brakeWarningLight = false;
    coolantLight = false;
    lowFuelLight = false;
    lowTirePressureLight = false;
    doorOpenLight = false;
    beltLight = false;
    turnLeftLight = false;
    turnRightLight = false;
    airbagLight = false;
    batteryLight = false;
    lightNotWorking = false;
    oilPressureLow = false;
    checkEngine = false;
    brakeActivated = false;
}

//getters return the value of a property
float Car::getMaxSpeed() const {
    return maxSpeed;
}

int Car::getGear() const {
    return gear;
}

float Car::getSpeed() const {
    return speedometer;
}

float Car::getOdometer() const {
    return odometer;
}
float Car::getMaxFuelAmount() const {
    return maxFuelAmount;

}
float Car::getFuelAmount() const {
    return fuelAmount;
}

float Car::getEngineTemperature() const {
    return engineTemperature;
}

float Car::getTirePressure() const {
    return tirePressure;
}

float Car::getTachometer() const {
    return tachometer;
}


bool Car::getAbsLight() const {
    return absLight;
}

bool Car::getStrongHeadLight() const {
    return strongHeadLight;
}
bool Car::getBrakeWarningLight() const {
    return brakeWarningLight;
}


bool  Car::getCoolantLight() const {
    return coolantLight;
}

bool Car::getLowFuelLight() const {
    return lowFuelLight;
}

bool  Car::getLowTirePressureLight() const {
    return lowTirePressureLight;
}

bool  Car::getDoorOpenLight() const {
    return doorOpenLight;
}

bool  Car::getBeltLight() const {
    return beltLight;
}

bool  Car::getTurnLeftSignal() const {
    return turnLeftLight;
}

bool  Car::getTurnRightSignal() const {
    return turnRightLight;
}
bool  Car::getIsEngineOn() const {
    return isEngineOn;
}

bool Car::getBatteryProblemLight() const {
    return batteryLight;
}
bool Car::getAirbagLight() const {
    return airbagLight;
}
bool Car::getLightNotWorking() const {
    return lightNotWorking;
}
bool Car::getOilPressureLow() const {
    return oilPressureLow;
}
bool Car::getCheckEngineLight() const {
    return checkEngine;
}
bool Car::getBrakeActivated() const {
    return brakeActivated;
}
//setters
void  Car::setMaxSpeed(float newMaxSpeed) {
    maxSpeed = newMaxSpeed;
}

void  Car::setGear(int newGear) {
    if (newGear < -1 || newGear >5) {
        throw std::invalid_argument("Car can be in gears  -1 to 5");
    }
    gear = newGear;
}

void  Car::setSpeedometer(float newSpeedometer) {
    if (newSpeedometer < 0)
    {
        throw std::invalid_argument("speed has to be greater than or equal to 0");
    }
    speedometer = newSpeedometer;

}

void  Car::setOdometer(float newOdometer) {
    odometer = newOdometer;

}

void  Car::setFuelAmount(float newfuelAmount) {
    if (newfuelAmount > maxFuelAmount) {
        throw std::invalid_argument("fuels level can't be larger than max fuels amount");
    }
    fuelAmount = newfuelAmount;
}

void Car::setEngineTemperature(float newEngineTemp) {
    engineTemperature = newEngineTemp;
}

void Car::setTirePressure(float newTirePressure) {
    tirePressure = newTirePressure;
}

void Car::setTachometer(float newTachometer) {
    if (newTachometer < 1500)
    {
        if (gear != 1)
            gear = gear - 1;
        odometer = 1500;
    }
    else
    {
        tachometer = newTachometer;
    }
}


void Car::setAbsLight(bool newValue) {
    absLight = newValue;
}

void Car::setStrongHeadLight(bool newValue) {
    strongHeadLight = newValue;
}

void Car::setBrakeWarningLight(bool newValue) {
    brakeWarningLight = newValue;
}

void Car::setCoolantLight(bool newValue) {
    coolantLight = newValue;
}

void Car::setLowFuelLight(bool newValue) {
    lowFuelLight = newValue;
}

void Car::setLowTirePressureLight(bool newValue) {
    lowTirePressureLight = newValue;
}

void Car::setDoorOpenLight(bool newValue) {
    doorOpenLight = newValue;
}

void Car::setBeltLight(bool newValue) {
    beltLight = newValue;
}

void Car::setTurnLeftSignal(bool newValue) {
    turnLeftLight = newValue;
}

void Car::setTurnRightSignal(bool newValue) {
    turnRightLight = newValue;
}
void Car::setIsEngineOn(bool newValue) {
    isEngineOn = newValue;
}
void Car::setBatteryLight(bool newValue) {
    batteryLight = newValue;
}
void Car::setAirbagLight(bool newValue) {
    airbagLight = newValue;
}
void Car::setLightNotWorking(bool newValue) {
    lightNotWorking = newValue;
}
void Car::setOilPressureLow(bool newValue) {
    oilPressureLow = newValue;
}
void Car::setCheckEngine(bool newValue) {
    checkEngine = newValue;
}
void Car::setBrakeActivated(bool newValue) {
    brakeActivated = newValue;
}
//methods
void Car::speedUp()
{
    speedometer == maxSpeed ? speedometer : speedometer += 1;
    tachometer = 2.65 * transfer_ratio_of_main_transmitter * transfer_ratios_in_the_gearbox[gear - 1] * speedometer / dynamic_radius_of_wheel;
    speedometer == 1 ? gear = 1 : speedometer;
}
void  Car::slowDown()
{
    if (speedometer != 0) {
        speedometer -= 1;
        tachometer = 2.65 * transfer_ratio_of_main_transmitter * transfer_ratios_in_the_gearbox[gear - 1] * speedometer / dynamic_radius_of_wheel;
    }
    else
    {
        tachometer = 1500;
    }

}
void  Car::turnStoppedLightsOn()
{
    turnLeftLight = true;
    turnRightLight = true;
}
void  Car::turnStoppedLightsOff()
{
    turnLeftLight = false;
    turnRightLight = false;
}
void Car::calculateFuelSpent(float kmPassed)
{
    float newfuelAmount = fuelAmount;
    switch (gear)
    {
    case 1:
        //polynomial value is an interpolation of fuel consumption curve in relation to velocity, and current gear, 
        newfuelAmount = fuelAmount - kmPassed * (0.00000545455 * pow(speedometer, 4) - 0.00080575 * pow(speedometer, 3) + 0.0433974 * pow(speedometer, 2) - 0.894308 * pow(speedometer, 1) + 20.2636) / 100;
        break;
    case 2:
        newfuelAmount = fuelAmount - kmPassed * (3.2579 * pow(10, -7) * pow(speedometer, 4) - 0.0000770258 * pow(speedometer, 3) + 0.00723163 * pow(speedometer, 2) - 0.273164 * pow(speedometer, 1) + 12.4449) / 100;
        break;
    case 3:
        newfuelAmount = fuelAmount - kmPassed * (-1.8748 * pow(10, -7) * pow(speedometer, 4) + 0.0000478741 * pow(speedometer, 3) - 0.00272499 * pow(speedometer, 2) + 0.017912 * pow(speedometer, 1) + 7.66769) / 100;
        break;
    case 4:
        newfuelAmount = fuelAmount - kmPassed * (3.8153 * pow(10, -8) * pow(speedometer, 4) - 0.0000158767 * pow(speedometer, 3) + 0.00258409 * pow(speedometer, 2) - 0.155483 * pow(speedometer, 1) + 8.81499) / 100;
        break;
    case 5:
        newfuelAmount = fuelAmount - kmPassed * (1.0016 * pow(10, -7) * pow(speedometer, 4) - 0.0000377478 * pow(speedometer, 3) + 0.00513284 * pow(speedometer, 2) - 0.250828 * pow(speedometer, 1) + 8.90571) / 100;
        break;
    default:
        break;
    }
    if (newfuelAmount <= 0.15 * maxFuelAmount)
    {
        lowFuelLight = true;
        if (newfuelAmount <= 0) {
            fuelAmount = 0;
            while (speedometer)
            {
                slowDown();
            }
            isEngineOn = false;
        }
    }
    else
    {
        lowFuelLight = false;
        fuelAmount = newfuelAmount;
    }


}
