#include <agents/GraphicsAgent.hpp>
#include <agents/OBDAgent.hpp>
#include <agents/BluetoothAgent.hpp>

int main(void) {
    OBDAgent obda;
    BluetoothAgent bla;
    GraphicsAgent ga;

    obda.start();
    bla.start();
    ga.start();
    
    obda.stop();
    bla.stop();
    return 0;
}
