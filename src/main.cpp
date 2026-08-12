#include <agents/DisplayAgent.hpp>
#include <agents/OBDAgent.hpp>
#include <agents/BluetoothAgent.hpp>
#include <agents/SystemAgent.hpp>
#include <agents/GPSAgent.hpp>

int main(void) {
    OBDAgent obda;
    BluetoothAgent bta;
    SystemAgent sa;
    GPSAgent gpsa;
    DisplayAgent ga;

    obda.start();
    bta.start();
    sa.start();
    gpsa.start();
    ga.start(); // This agent runs on current main thread

    obda.stop();
    bta.stop();
    sa.stop();
    gpsa.stop();
    return 0;
}
