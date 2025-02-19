//
// Created by Ethan Bossenbroek on 2/18/25.
//

#ifndef GVZORK_H
#define GVZORK_H


#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <stdexcept>
#include <functional>


// each item has an appropriate slot in the overall guitar, once every slot if filled then the
// guitar is complete, i.e. the player can collect multiple bridge pickups but only one can be in
// the guitar at a time. The player also has limited pocket space and will only be able to hold
// about 4 parts at once or like one body of a guitar

class Item {
public:
    Item(std::string name, std::string description, int slot, float pocket_space);

    std::string getName() const;

    friend std::ostream& operator<<(std::ostream& os, const Item& item);

private:
    std::string name;
    std::string description;
    int slot; // 0 = body, 1 = neck, 2 = pickup, etc.
    float pocket_space;
};

// Guitar class manages the ultimate guitar build
class Guitar {
public:
    Guitar(std::string name, std::string description);

    bool addPart(Item* item);
    void showGuitar() const;

private:
    std::string name;
    std::string description;
    std::vector<Item*> slots; // Holds guitar parts in designated slots
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
    void executeCommand(std::string command, std::vector<std::string> args);
    void normalizeCommand(std::string& command);


private:
    void createWorld();
    void showHelp(std::vector<std::string> args);
    void talk(std::vector<std::string> args);
    void move(std::vector<std::string> args);
    void take(std::vector<std::string> args);
    void drink(std::vector<std::string> args);
    void look(std::vector<std::string> args);
    void quit(std::vector<std::string> args);
    Location* currentLocation;
    std::vector<Location> locations;
    std::vector<Item> inventory;
    int drunkness;
    std::map<std::string, std::function<void(std::vector<std::string>)>> commands;
    std::map<std::string, std::string> commandAliases;
};

#endif