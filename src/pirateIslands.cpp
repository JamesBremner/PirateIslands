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
    auto bestPath = myPath;
    bool first = true;
    for (auto &pathtest : allPaths(gd))
    {
        if (first)
        {
            first = false;

            // ignore first path from Yen, it is the dijsktra path
            continue;
        }
        std::cout << "Check alternative path\n";

        // dodge pirates
        myPath = pathtest;
        bool fPirateDelay = dodgePirates();

        // check for better path
        if (myPath.second < bestPath.second)
        {
            bestPath = myPath;
            std::cout << "better path: more sailing less pirates\n";
        }

        // check for no pirate delays
        // in which case there is no expectation of finding a better path
        // since the rest of the possible paths are longer, even with no pirate delays
        if (!fPirateDelay)
            break;
    }

    // keep the best path found
    myPath = bestPath;
}

bool cBoat::dodgePirates()
{
    bool fPirateDelay = false;
    int time = 0;
    auto prev = myPath.first.end();
    raven::graph::path_cost_t delayPath;

    // loop over path
    for (
        auto it = myPath.first.begin();
        it != myPath.first.end();
        it++)
    {
        if (prev == myPath.first.end())
        {
            // at start island
            prev = it;
            arrive(
                delayPath,
                *it,
                time);
            continue;
        }

        // calculate the time
        int isle = *it;
        int stime = pIsles->sailtime(*prev, isle);
        time += stime;
        delayPath.second += stime;

        // wait until island is free of pirates
        bool delayed = waitForPirates(
            delayPath,
            *prev,
            isle,
            time);
        if (!fPirateDelay)
            fPirateDelay = delayed;

        // safe to move on
        arrive(
            delayPath,
            isle,
            time);

        prev = it;
    }
    myPath = delayPath;
    return fPirateDelay;
}

bool cBoat::waitForPirates(
    path_t &path,
    int prev,
    int isle,
    int &time)
{
    bool ret = false;
    while (pPirates->isCollision(isle, time))
    {
        ret = true;
        std::cout << "Pirates on " << pIsles->name(isle) << " staying on ";
        arrive(
            path,
            prev,
            time);
        time++;
        path.second++;
    }
    return ret;
}

void cBoat::arrive(
    path_t &safePath,
    int isle,
    int time)
{
    std::cout << pIsles->name(isle) << " at time " << time << "\n";
    safePath.first.push_back(isle);
}

void cBoat::printSafePath() const
{
    int prev = -1;
    int time = 0;
    for (int isle : myPath.first)
    {
        if (prev == -1)
        {
            std::cout << "\nat time " << time << " " << pIsles->name(isle) << "\n";
            prev = isle;
            continue;
        }
        if (isle == prev)
        {
            std::cout << "staying ";
            time++;
            prev = isle;
            continue;
        }
        for (
            int stime = pIsles->sailtime(prev, isle);
            stime;
            stime--)
        {
            std::cout << "sailing ";
            time++;
        }
        std::cout << "\nat time " << time << " arrive " << pIsles->name(isle) << "\n";
        prev = isle;
    }
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
            ifs >> sn1 >> sn2;
            boat.set(sn1,sn2);
            break;
        }
        ifs >> stype;
    }
}