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

void cInstance::generateExample1()
{
    std::cout << "\nExample1\n";

    isles.addAdjacentIslands("isle1", "isle2", 6);
    isles.addAdjacentIslands("isle2", "isle3", 1);
    isles.addAdjacentIslands("isle2", "isle4", 2);
    isles.addAdjacentIslands("isle3", "isle4", 3);

    pirates.add(isles.find("isle3"), 6);
    pirates.add(isles.find("isle3"), 7);
    pirates.add(isles.find("isle3"), 8);

    boat.set("isle1", "isle3");
}
void cInstance::generateExample2()
{
    std::cout << "\nExample2\n";

    isles.addAdjacentIslands("isle1", "isle2", 6);
    isles.addAdjacentIslands("isle1", "isle3", 1);
    isles.addAdjacentIslands("isle2", "isle4", 2);
    isles.addAdjacentIslands("isle3", "isle4", 3);

    pirates.add(isles.find("isle3"), 1);
    pirates.add(isles.find("isle3"), 2);
    pirates.add(isles.find("isle3"), 3);
    pirates.add(isles.find("isle3"), 4);
    pirates.add(isles.find("isle3"), 5);

    boat.set("isle1", "isle4");
}