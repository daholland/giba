#include <spock.h>
#include <gibalib.h>
using namespace spock;
using namespace gibalib;
int main(int argc, char* argv[]) {
    Gibalib gb{0};
    gb.debugtest();

    Spock spock;

    spock.init();

    spock.run();

    spock.cleanup();

    return 0;
}