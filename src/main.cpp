#include <agents/DisplayAgent.hpp>
#include <agents/OBDAgent.hpp>
#include <agents/BluetoothAgent.hpp>

int main(void) {
    OBDAgent obda;
    BluetoothAgent bla;
    DisplayAgent ga;

    obda.start();
    bla.start();
    ga.start(); // This agent runs on current main thread
    
    obda.stop();
    bla.stop();
    return 0;
}
