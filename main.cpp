#include <SFML\Graphics.hpp>
#include <iostream>
#include "simulation.h"
#include <fstream>

int main() {
    std::ofstream data;
    data.open("ArgonData.txt");
    double temperature = 1000;
    for (int j = 182; j < 183; ++j) {
        Settings::temperature = temperature;
        Settings::recalculateConstants();
        int i = 0;
        data << "Temperature: " << temperature << ' ';
        while (i < 3) {
            Simulation simulation{};

            double cr = simulation.mainLoop(temperature);

            if (cr < 0) continue;

            data << "Trial " << i << ": " << cr << ' ' << "Temp Uncertainty: " << simulation.getTempUncMax() * 100 << "% ";
            ++i;
        }
        data << '\n';
        temperature += 5;
        Settings::delta_time /= 1.0116;
    }

    data.close();
    return 0;
}