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

    /// @brief set route
    /// @param dep departure island
    /// @param dst destination island
    /// @param fuel 

    void set(
        const std::string &dep,
        const std::string &dst,
        int fuel )
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

    void printSafePath() const;

private:
    std::string startName;
    std::string endName;

    cIslands *pIsles;
    cPirates *pPirates;

    path_t myPath;

    /// @brief activity occupying previous time slot
    enum class eActivity
    {
        start,
        sail,
        dodge,
        refuel
    };

    /// @brief timeline of activities 
    std::vector<std::pair<int,eActivity>> myTimeline;

    int myFuel;

    /// @brief Adjust path to avoid pirates
    /// @return true if dodging pirates was needed
    ///
    /// mySafePath attribute is input and modified
    /// to remain on an island if the next island would have pirates when we arrived there

    bool dodgePirates();

    /// @brief wait for pirates, if encountered on island
    /// @param[in/out] safePath
    /// @param prev
    /// @param isle // to check
    /// @param time // we would arrive with no pirates
    /// @return delay

    int waitForPirates(
        int prev,
        int isle,
        int deptime,
        int arrtime);

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
    void readfile( const std::string& fname );
    void printResult();
};