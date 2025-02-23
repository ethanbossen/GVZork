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
private:
    std::string name;
    std::string description;
    int calories;
    float weight;

public:

    Item(const std::string& name, const std::string& description, int calories, float weight) {
        if (name.empty()) {
            throw std::invalid_argument("Name cannot be blank.");
        }
        if (description.empty()) {
            throw std::invalid_argument("Description cannot be blank.");
        }
        if (calories < 0 || calories > 1000) {
            throw std::invalid_argument("Calories must be between 0 and 1000.");
        }
        if (weight < 0 || weight > 500) {
            throw std::invalid_argument("Weight must be between 0 and 500.");
        }

        this->name = name;
        this->description = description;
        this->calories = calories;
        this->weight = weight;
    }
    
    std::string getName() const { return name; }
    
    std::string getDescription() const { return description; }
    
    int getCalories() const { return calories; }
    
    float getWeight() const { return weight; }

    friend std::ostream& operator<<(std::ostream& os, const Item& item) {
        os << item.name << " (" << item.calories << " calories)- " << item.weight << " lb- " << item.description;
        return os;
    }
};


class NPC {
private:
    std::string name;
    std::string description;
    std::vector<std::string> messages;
    int messageNumber;

public:
    NPC(const std::string& name, const std::string& description) {
        if (name.empty() || description.empty()) {
            throw std::invalid_argument("Name and description cannot be blank.");
        }

        this->name = name;
        this->description = description;
    }
    
    std::string getName() const { return name; }
    
    std::string getDescription() const { return description; }

    void addMessage(const std::string& message) {
        messages.push_back(message);
    }

    std::string getMessage() {
        if (messages.empty()) {
            return ("This NPC has no messages.");
        }
        std::string message = messages[messageNumber];
        messageNumber = (messageNumber + 1) % messages.size();
        return message;
    }

    friend std::ostream& operator<<(std::ostream& os, const NPC& npc) {
        os << npc.name;
        return os;
    }

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
    Location(const std::string& name, const std::string& description) {
        if (name.empty() || description.empty()) {
            throw std::invalid_argument("Name and description cannot be blank.");
        }

        this->name = name;
        this->description = description;

    }

    std::map<std::string, Location*> get_locations() const { return neighbors;}
    
    void add_location(std::string& direction, Location* location){
        if (direction.empty()){
            throw std::invalid_argument("Direction cannot be empty.");
        }
        if (neighbors.count(direction) > 0){
            throw std::invalid_argument("That direction is already mapped for this location.");
        }
        neighbors[direction] = location;
    }

    void add_npc(NPC npc){
        npcs.push_back(npc);
    }

    std::vector<NPC> get_npcs() const { return npcs;}
    
    void add_item(Item item){
        items.push_back(item);
    }
    
    std::vector<Item> get_items() const { return items;}
    
    void set_visited(){
        visited = true;
    }

    bool get_visited(){
        if (this->visited) {
            return true;
        }
        return false;
    }

    friend std::ostream& operator<<(std::ostream& os, const Location& location) {
        // Location name and description
        os << location.name << "- " << location.description << "\n\n";

        // List NPCs
        os << "You see the following NPCs:\n";
        if (location.npcs.empty()) {
            os << "- None\n";
        } else {
            for (const NPC& npc : location.npcs) {
                os << "- " << npc << "\n";
            }
        }

        // List items
        os << "\nYou see the following Items:\n";
        if (location.items.empty()) {
            os << "- None\n";
        } else {
            for (const Item& item : location.items) {
                os << "- " << item.getName() << " (" << item.getCalories() << " calories) - "
                   << item.getWeight() << " lb- " << item.getDescription() << "\n";
            }
        }

        // List directions
        os << "\nYou can go in the following Directions:\n";
        if (location.neighbors.empty()) {
            os << "- None\n";
        } else {
            for (const auto& neighbor : location.neighbors) {
                os << "- " << neighbor.first << "- "
                   << (neighbor.second->get_visited() ? neighbor.second->name : "Unknown")
                   << (neighbor.second->get_visited() ? " (Visited)" : "") << "\n";
            }
        }

        return os;
    }
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