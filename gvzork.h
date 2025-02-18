//
// Created by Ethan Bossenbroek on 2/18/25.
//

#ifndef GVZORK_H
#define GVZORK_H
#endif

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <stdexcept>
#include <functional>

class Item {
public:
    Item(std::string name, std::string description, int calories, float weight);
    std::string getName() const;
    friend std::ostream& operator<<(std::ostream& os, const Item& item);

private:
    std::string name;
    std::string description;
    int calories;
    float weight;
};

class NPC {
public:
    NPC(std::string name, std::string description, std::vector<std::string> messages);
    std::string getName() const;
    std::string getMessage();
    friend std::ostream& operator<<(std::ostream& os, const NPC& npc);

private:
    std::string name;
    std::string description;
    std::vector<std::string> messages;
    int messageIndex;
};

class Location {
public:
    Location(std::string name, std::string description);
    void addNeighbor(std::string direction, Location* location);
    void addNPC(NPC npc);
    void addItem(Item item);
    std::vector<NPC> getNPCs() const;
    std::vector<Item> getItems() const;
    std::map<std::string, Location*> getNeighbors() const;
    friend std::ostream& operator<<(std::ostream& os, const Location& location);

private:
    std::string name;
    std::string description;
    std::map<std::string, Location*> neighbors;
    std::vector<NPC> npcs;
    std::vector<Item> items;
    bool visited;
};

class Game {
public:
    Game();
    void play();
    void createWorld();
    void executeCommand(std::string command, std::vector<std::string> args);
    void showHelp(std::vector<std::string> args);
    void talk(std::vector<std::string> args);
    void move(std::vector<std::string> args);
    void take(std::vector<std::string> args);
    void drink(std::vector<std::string> args);
    void look(std::vector<std::string> args);
    void quit(std::vector<std::string> args);

private:
    Location* currentLocation;
    std::vector<Location> locations;
    std::vector<Item> inventory;
    int drunkness;
    std::map<std::string, std::function<void(std::vector<std::string>)>> commands;
};



