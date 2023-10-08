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
    if (!dodgePirates())
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
        bool fPirateDelay = dodgePirates();

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

bool cBoat::dodgePirates()
{
    bool fPirateDelay = false;
    int time = 0;

    /* There may be delays, due to refueling or waiting for pirates,
     *  myPath, the optimum path from Dijsktra or Yen,
     *  will be followed with the addition of these delays
     */
    myTimeline.clear();

    // loop over path
    auto prev = myPath.first.end();
    for (
        auto it = myPath.first.begin();
        it != myPath.first.end();
        it++)
    {
        if (prev == myPath.first.end())
        {
            // at start island
            timestep(0, eActivity::start);
            prev = it;
            continue;
        }

        // calculate the time to next island
        int isle = *it;
        int stime = pIsles->sailtime(*prev, isle);

        // check fuel
        while (myFuel < stime)
        {
            std::cout << "Refueling on " << pIsles->name(*prev) << " ";
            timestep(*prev, eActivity::refuel);
            time++;
            myFuel++;
        }

        // wait until island is free of pirates
        int delay = waitForPirates(
            *prev,
            isle,
            time,
            time + stime);
        if (!fPirateDelay)
            fPirateDelay = (delay > 0);

        // safe to move on
        for (int s = 0; s < stime; s++)
            timestep(isle, eActivity::sail);
        time += delay + stime;

        prev = it;
    }
    // myPath = delayPath;
    return fPirateDelay;
}

int cBoat::waitForPirates(
    int prev,
    int isle,
    int deptime,
    int arrtime)
{
    int delay = 0;
    while (pPirates->isCollision(isle, arrtime))
    {
        // pirates waiting, stay where we are
        std::cout << "Pirates on " << pIsles->name(isle) << " staying on ";
        myTimeline.push_back(std::make_pair(prev, eActivity::dodge));
        deptime++;
        arrtime++;
        delay++;
        myFuel++;
    }
    return delay;
}

void cBoat::timestep(int isle, eActivity A)
{
    myTimeline.emplace_back(isle, A);
    std::cout << "time " << myTimeline.size()
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
    }
    std::cout << pIsles->name(isle) << " ";
}

// void cBoat::arrive(
//     path_t &safePath,
//     int isle,
//     int time)
// {
//     std::cout <<"\n"<< pIsles->name(isle) << " at time " << time << "\n";
//     safePath.first.push_back(isle);
//     myFuel--;
// }

int cBoat::getLength() const
{
    return myTimeline.size() - 1;
}

void cBoat::printSafePath() const
{
    std::cout << "\n=========Best Safe Path =========================\n";
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
        }
    }

    std::cout << "==================================\n\n";
}

void cInstance::readfile(const std::string &fname)
{
    std::ifstream ifs(fname);
    if (!ifs.is_open())
        throw std::runtime_error("Cannot open file");

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