#include <agents/DisplayAgent.hpp>
#include <agents/OBDAgent.hpp>
#include <agents/BluetoothAgent.hpp>

int main(void) {
    OBDAgent obda;
    BluetoothAgent bta;
    DisplayAgent ga;

    obda.start();
    bta.start();
    ga.start(); // This agent runs on current main thread
    
    obda.stop();
    bta.stop();
    return 0;
}
