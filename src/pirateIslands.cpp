#include "pirateIslands.h"

void cIslands::addAdjacentIslands(
    const std::string &name1,
    const std::string &name2,
    int cost)
{
    int ei = gd.g.add(name1, name2);
    if (ei >= gd.edgeWeight.size())
        gd.edgeWeight.resize(ei + 1);
    gd.edgeWeight[ei] = cost;
}

void cBoat::navigate()
{
    if (!pIsles)
        throw std::runtime_error("cBoat isles unset");

    // apply dijsktra
    auto &gd = pIsles->getGraph();
    gd.startName = startName;
    gd.endName = endName;
    myPath = path(gd);

    // dodge pirates
    reality();

    if (! fPirateDelay )
        return;

    // there was a pirate delay,
    // check alternate paths from Yen's algorithm
    auto bestTimeline = myTimeline;
    bool first = true;
    for (auto &pathtest : allPaths(gd))
    {
        if (first)
        {
            first = false;

            // ignore first path from Yen, it is the dijsktra path
            continue;
        }
        std::cout << "\nCheck alternative path\n";

        // dodge pirates
        myPath = pathtest;
        reality();

        // check for better path
        if (myTimeline.size() < bestTimeline.size())
        {
            bestTimeline = myTimeline;
            std::cout << "\nbetter path: more sailing less pirates\n";
        }

        // check for no pirate delays
        // in which case there is no expectation of finding a better path
        // since the rest of the possible paths are longer, even with no pirate delays
        if (!fPirateDelay)
            break;
    }

    // keep the best found
    myTimeline = bestTimeline;
}

void cBoat::reality()
{
    fPirateDelay = false;

    /* There may be delays, due to refueling or waiting for pirates,
     *  myPath, the optimum path from Dijsktra or Yen,
     *  will be followed with the addition of these delays
     */
    myTimeline.clear();

    for( int nextIsland : myPath.first)
    {
        if (nextIsland == myPath.first[0])
        {
            // at start island
            timestep(
                nextIsland,
                eActivity::start);
            continue;
        }

        // check fuel
        refuel(nextIsland);

        // wait until island is free of pirates
        waitForPirates(nextIsland);

        // able and safe to move on
        sail(nextIsland);
    }
    timestep(
        pIsles->find(endName),
        eActivity::end );
}

void cBoat::refuel(int isle)
{
    int stime = pIsles->sailtime(myTimeline.back().first, isle);
    while (myFuel < stime)
    {
        timestep(myTimeline.back().first, eActivity::refuel);
        myFuel++;
    }
}

void cBoat::waitForPirates(int isle)
{
    int prev = myTimeline.back().first;
    int atime = myTimeline.size() + pIsles->sailtime(prev, isle);
    while (pPirates->isCollision(isle, atime))
    {
        // pirates waiting, stay where we are
        timestep(prev, eActivity::dodge);
        atime++;
        myFuel++;
        fPirateDelay = true;
    }
}

void cBoat::sail(int isle)
{
     int stime = pIsles->sailtime(myTimeline.back().first, isle);
    for (
        int s = 0;
        s < stime;
        s++)
    {
        timestep(isle, eActivity::sail);
        myFuel--;
    }
}

void cBoat::timestep(int isle, eActivity A)
{
    myTimeline.emplace_back(isle, A);

    std::cout << "| time " << myTimeline.size()-1
              << " ";
    switch (A)
    {
    case eActivity::start:
        std::cout << "starting at ";
        break;

    case eActivity::refuel:
        std::cout << "refueling at ";
        break;

    case eActivity::dodge:
        std::cout << "dodging pirates at ";
        break;

    case eActivity::sail:
        std::cout << "sailing to ";
        break;

    case eActivity::end:
        std::cout << "destination ";
        break;
    }
    std::cout << pIsles->name(isle) << " ";
}

int cBoat::getLength() const
{
    return myTimeline.size() - 2;
}

void cBoat::printSafePath() const
{

    int time = 0;
    for (auto &la : myTimeline)
    {
        std::cout << " time " << time++ << ": ";
        switch (la.second)
        {
        case eActivity::start:
            std::cout << "starting at " << pIsles->name(la.first) << "\n";
            break;

        case eActivity::refuel:
            std::cout << "refueling at " << pIsles->name(la.first) << "\n";
            break;

        case eActivity::dodge:
            std::cout << "dodging pirates at " << pIsles->name(la.first) << "\n";
            break;

        case eActivity::sail:
            std::cout << "sailing to " << pIsles->name(la.first) << "\n";
            break;

        case eActivity::end:
            std::cout << "at destination " << pIsles->name(la.first) << "\n";
            break;
        }
    }
}

void cInstance::navigate()
{
    boat.navigate();
}

void cInstance::printResult()
{
    std::cout << "\n========= " << myfname << " Safe Path =========================\n";
    boat.printSafePath();
    std::cout << "==================================\n\n";
}

void cInstance::readfile(const std::string &fname)
{
    std::ifstream ifs(fname);
    if (!ifs.is_open())
        throw std::runtime_error("Cannot open file");
    myfname = fname;

    std::string stype, sn1, sn2, scost;
    int ki;
    ifs >> stype;

    while (ifs.good())
    {
        switch (stype[0])
        {
        case 'l':
            ifs >> sn1 >> sn2 >> scost;
            isles.addAdjacentIslands(sn1, sn2, atoi(scost.c_str()));
            break;
        case 'p':
            ifs >> sn1 >> scost;
            ki = isles.find(sn1);
            if (ki < 0)
                throw std::runtime_error("Pirate cannot find " + sn1);
            pirates.add(ki, atoi(scost.c_str()));
            break;
        case 'r':
            ifs >> sn1 >> sn2 >> scost;
            boat.set(sn1, sn2, atoi(scost.c_str()));
            break;
        }
        ifs >> stype;
    }
}