#pragma once
#include <stdexcept>
#include <array>
#include <stdio.h>
class Car
{
protected:
    //Static data needed for calculations, dependant on car model
    float transfer_ratio_of_main_transmitter = 3.938;
    std::array<float, 5> transfer_ratios_in_the_gearbox = { 2.925, 1.75, 1.508, 1.055, 0.848 };
    float dynamic_radius_of_wheel = 0.27;
    float mass = 1515;
    //dynamic data showed on the instrument panel
    int gear;
    float speedometer;
    float maxSpeed = 250;
    float odometer;
    float maxFuelAmount = 50;
    float fuelAmount;
    float engineTemperature;
    float tirePressure;
    float tachometer;
    bool isEngineOn;

    //Warning lights
    bool absLight;
    bool strongHeadLight;
    bool brakeWarningLight;
    bool coolantLight;
    bool lowFuelLight;
    bool lowTirePressureLight;
    bool doorOpenLight;
    bool beltLight;
    bool turnLeftLight;
    bool turnRightLight;
    bool batteryLight;
    bool airbagLight;
    bool lightNotWorking;
    bool oilPressureLow;
    bool checkEngine;
    bool brakeActivated;
public:
    // Constructor
    Car();


    // Getters
    float getMaxSpeed() const;
    int getGear() const;
    float getSpeed() const;
    float getOdometer() const;
    float getMaxFuelAmount() const;
    float getFuelAmount() const;
    float getFluidLevel() const;
    float getEngineTemperature() const;
    float getTirePressure() const;
    float getTachometer() const;
    bool getAbsLight() const;
    bool getStrongHeadLight() const;
    bool getBrakeWarningLight() const;
    bool getCoolantLight() const;
    bool getLowFuelLight() const;
    bool getLowTirePressureLight() const;
    bool getDoorOpenLight() const;
    bool getBeltLight() const;
    bool getTurnLeftSignal() const;
    bool getTurnRightSignal() const;
    bool getIsEngineOn() const;
    bool getBatteryProblemLight() const;
    bool getAirbagLight() const;
    bool getLightNotWorking() const;
    bool getOilPressureLow() const;
    bool getCheckEngineLight() const;
    bool getBrakeActivated() const;
    // Setters
    void setMaxSpeed(float newMaxSpeed);
    void setGear(int newGear);
    void setSpeedometer(float newSpeedometer);
    void setOdometer(float newOdometer);
    void setFuelAmount(float newfuelAmount);
    void setFluidLevel(float newFluidLevel);
    void setEngineTemperature(float newEngineTemp);
    void setTirePressure(float newTirePressure);
    void setTachometer(float newTachometer);
    void setAbsLight(bool newValue);
    void setStrongHeadLight(bool newValue);
    void setBrakeWarningLight(bool newValue);
    void setCoolantLight(bool newValue);
    void setLowFuelLight(bool newValue);
    void setLowTirePressureLight(bool newValue);
    void setDoorOpenLight(bool newValue);
    void setBeltLight(bool newValue);
    void setTurnLeftSignal(bool newValue);
    void setTurnRightSignal(bool newValue);
    void setIsEngineOn(bool newValue);
    void setBatteryLight(bool newValue);
    void setAirbagLight(bool newValue);
    void setLightNotWorking(bool newValue);
    void setOilPressureLow(bool newValue);
    void setCheckEngine(bool newValue);
    void setBrakeActivated(bool newValue);
    // Methods
    void speedUp();
    void slowDown();
    void turnStoppedLightsOn();
    void turnStoppedLightsOff();
    void calculateFuelSpent(float kmPassed);
};



