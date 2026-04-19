#include <agents/DisplayAgent.hpp>
#include <agents/OBDAgent.hpp>
#include <agents/BluetoothAgent.hpp>
#include <agents/SystemAgent.hpp>

int main(void) {
    OBDAgent obda;
    BluetoothAgent bta;
    SystemAgent sa;
    DisplayAgent ga;

    obda.start();
    bta.start();
    sa.start();
    ga.start(); // This agent runs on current main thread
    
    obda.stop();
    bta.stop();
    sa.stop();
    return 0;
}
