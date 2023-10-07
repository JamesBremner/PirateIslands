

#include "pirateIslands.h"



main()
{

    cInstance thePirateIslands1;

    thePirateIslands1.generateExample1();

    thePirateIslands1.boat.navigate();

    thePirateIslands1.boat.printSafePath();

    if (thePirateIslands1.boat.getLength() != 9)
        std::cout << "BAD RESULT\n";

    cInstance thePirateIslands2;

    thePirateIslands2.generateExample2();

    thePirateIslands2.boat.navigate();

    thePirateIslands2.boat.printSafePath();

    if (thePirateIslands2.boat.getLength() != 8)
        std::cout << "BAD RESULT\n";

    return 0;
}
