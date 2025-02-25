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

class Item {
private:
    std::string name;
    std::string description;
    int calories;
    float weight;

public:
    Item(const std::string& name, const std::string& description, int calories, float weight);
    std::string getName() const;
    std::string getDescription() const;
    int getCalories() const;
    float getWeight() const;
    friend std::ostream& operator<<(std::ostream& os,const Item& item);
};


class NPC {
private:
    std::string name;
    std::string description;
    std::vector<std::string> messages;
    int messageNumber;

public:
    NPC(const std::string& name, const std::string& description);
    std::string getName() const;
    std::string getDescription() const;
    void addMessage(std::string& message);
    std::string getMessage();
    friend std::ostream& operator<<(std::ostream& os,const NPC& npc);

};

class Location {
private:
    std::string name;
    std::string description;
    std::map<std::string, Location*> neighbors;
    std::vector<NPC> npcs;
    std::vector<Item> items;
    bool visited;

public:
    Location(const std::string& name, const std::string& description);
    std::map<std::string, Location*> get_locations() const;
    void add_location(const std::string& direction, Location* location);
    void add_npc(NPC npc);
    std::vector<NPC> get_npcs() const;
    void add_item(const Item& item);
    std::vector<Item> get_items() const;
    void set_visited();
    bool get_visited() const;
    friend std::ostream& operator<<(std::ostream& os, const Location& location);
};

class Game {
public:
    Game();
    void play();
    void executeCommand(std::string command, std::vector<std::string> args);
    void normalizeCommand(std::string& command);
    


private:
    std::map<std::string, std::function<void(std::vector<std::string>)>> commands;
    std::vector<Item> inventory;
    int currentWeight;
    std::vector<Location> locations;
    Location* currentLocation;
    int caloriesNeeded;
    bool inProgress;
    int drunkness;
    void createWorld();
    std::map<std::string, std::function<void(std::vector<std::string>)>> setup_commands();
    Location* randomLocation();
    void showHelp(std::vector<std::string> args);
    void talk(std::vector<std::string> target);
    void meet(std::vector<std::string> target);
    void take(std::vector<std::string> target);
    void give(std::vector<std::string> target);
    void go(std::vector<std::string> target);
    void showItems(std::vector<std::string> target);
    void look(std::vector<std::string> target);
    void quit(std::vector<std::string> target);
    void drink(std::vector<std::string> target);
    std::map<std::string, std::string> commandAliases;
};

#endif