#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include "../PathFinder/src/GraphTheory.h"

typedef raven::graph::path_cost_t path_t;

class cIslands
{
public:
    void clear()
    {
        gd.g.clear();
        gd.edgeWeight.clear();
    }

    /// @brief Add two adjacent islands
    /// @param name1
    /// @param name2
    /// @param cost     // time taken to sail between islands

    void addAdjacentIslands(
        const std::string &name1,
        const std::string &name2,
        int cost);

    raven::graph::sGraphData &
    getGraph()
    {
        return gd;
    }

    std::string name(int isle) const
    {
        return gd.g.userName(isle);
    }
    int find(const std::string &name) const
    {
        return gd.g.find(name);
    }

    /// @brief time required to sail between two islands
    /// @param prev
    /// @param isle
    /// @return

    int sailtime(int prev, int isle) const
    {
        return gd.edgeWeight[gd.g.find(prev, isle)];
    }

private:
    raven::graph::sGraphData gd; // island adjacencies
};

class cPirates
{
public:
    void clear()
    {
        vLocation.clear();
    }
    void add(
        int isle,
        int time)
    {
        vLocation.push_back(
            std::make_pair(isle, time));
    }
    /// @brief true if there are pirates on an island at a certain time
    /// @param isle island index
    /// @param time

    bool isCollision(
        int isle,
        int time) const
    {
        return std::find(
                   vLocation.begin(), vLocation.end(),
                   std::make_pair(isle, time)) != vLocation.end();
    }

private:
    std::vector<std::pair<int, int>> vLocation; // pirate locations and times
};

class cBoat
{
public:
    cBoat()
        : pIsles(0)
    {
    }

    void clear()
    {

    }

    /// @brief set route
    /// @param dep departure island
    /// @param dst destination island
    /// @param fuel

    void set(
        const std::string &dep,
        const std::string &dst,
        int fuel)
    {
        startName = dep;
        endName = dst;
        myFuel = fuel;
    }
    void set(cIslands &islands)
    {
        pIsles = &islands;
    }
    void set(cPirates &pirates)
    {
        pPirates = &pirates;
    }

    /// @brief Navigate from start to finish, avoiding pirates

    void navigate();

    /// @brief time from start to destination, including sailing and dodging pirates

    int getLength() const;

    /// @brief output details of the safe path

    std::string realPathText() const;

private:
    std::string startName;
    std::string endName;

    cIslands *pIsles;
    cPirates *pPirates;

    path_t myPath;

    bool fPirateDelay;

    /// @brief activity occupying previous time slot
    enum class eActivity
    {
        start,
        sail,
        dodge,
        refuel,
        end,
    };

    /// @brief timeline of activities
    std::vector<std::pair<int, eActivity>> myTimeline;

    int myFuel;


    /** @brief Follow a path through islands, refuelling and dodging pirates as needed
     *
     * Graph theory has given a path through the islands
     * assuming that there are no delays
     * 
     * This generates a realistic timeline, 
     * including delays as required for refuleling and dodging pirates 
     */

    void reality(); 

    /// @brief wait for pirates, if they would be encountered on next island
    /// @param isle // next island
    /// @return delay

    void waitForPirates(int isle);

    /// @brief refuel if neccessary to reach the next island
    /// @param isle // next island

    void refuel(int isle);

    /// @brief sail to the next island
    /// @param isle 

    void sail(int isle);

    /// @brief add busy timeslot
    /// @param isle
    /// @param A // activity

    void timestep(int isle, eActivity A);
};

/// @brief Some islands, some pirates infesting the islands, a boat trying to safely navigate between the islands

class cInstance
{
public:
    cIslands isles;
    cPirates pirates;
    cBoat boat;
    std::string myfname;

    cInstance()
    {
        boat.set(isles);
        boat.set(pirates);
    }
    void clear();
    void readfile(const std::string &fname);
    void navigate();
    void printResult();
    std::string textResults();
};