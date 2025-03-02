#ifndef GVZORK_H
#define GVZORK_H

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <stdexcept>
#include <functional>

/**
 * @class Item
 * @brief Represents an item in the game with a name, description, calories, and weight.
 */
class Item {
private:
    std::string name;        ///< The name of the item.
    std::string description; ///< A description of the item.
    int calories;            ///< The number of calories (or "awesome points") the item provides.
    float weight;            ///< The weight of the item in pounds.

public:
    /**
     * @brief Constructs an Item object.
     * @param name The name of the item.
     * @param description A description of the item.
     * @param calories The number of calories the item provides.
     * @param weight The weight of the item in pounds.
     * @throws std::invalid_argument If the name or description is empty, or if calories/weight are out of bounds.
     */
    Item(const std::string& name, const std::string& description, int calories, float weight);

    std::string getName() const;        ///< Returns the name of the item.
    std::string getDescription() const; ///< Returns the description of the item.
    int getCalories() const;            ///< Returns the number of calories the item provides.
    float getWeight() const;            ///< Returns the weight of the item in pounds.

    /**
     * @brief Overloads the << operator to print Item details.
     * @param os The output stream.
     * @param item The Item object to print.
     * @return The output stream.
     */
    friend std::ostream& operator<<(std::ostream& os, const Item& item);
};

/**
 * @class NPC
 * @brief Represents a non-player character (NPC) in the game with a name, description, and messages.
 */
class NPC {
private:
    std::string name;                ///< The name of the NPC.
    std::string description;         ///< A description of the NPC.
    std::vector<std::string> messages; ///< A list of messages the NPC can say.
    int messageNumber;               ///< The index of the current message to display.

public:
    /**
     * @brief Constructs an NPC object.
     * @param name The name of the NPC.
     * @param description A description of the NPC.
     * @throws std::invalid_argument If the name or description is empty.
     */
    NPC(const std::string& name, const std::string& description);

    std::string getName() const;        ///< Returns the name of the NPC.
    std::string getDescription() const; ///< Returns the description of the NPC.

    void addMessage(const std::string& message); ///< Adds a message to the NPC's list of messages.
    std::string getMessage();                    ///< Returns the next message in the NPC's list.

    /**
     * @brief Overloads the << operator to print the NPC's name.
     * @param os The output stream.
     * @param npc The NPC object to print.
     * @return The output stream.
     */
    friend std::ostream& operator<<(std::ostream& os, const NPC& npc);
};

/**
 * @class Location
 * @brief Represents a location in the game with a name, description, NPCs, items, and neighboring locations.
 */
class Location {
private:
    std::string name;                ///< The name of the location.
    std::string description;         ///< A description of the location.
    std::vector<NPC> npcs;           ///< A list of NPCs in the location.
    std::vector<Item> items;         ///< A list of items in the location.
    bool visited;                    ///< Whether the location has been visited by the player.

public:
    std::map<std::string, Location*> neighbors; ///< A map of neighboring locations and their directions.

    /**
     * @brief Constructs a Location object.
     * @param name The name of the location.
     * @param description A description of the location.
     * @throws std::invalid_argument If the name or description is empty.
     */
    Location(const std::string& name, const std::string& description);

    std::map<std::string, Location*> get_locations() const; ///< Returns the map of neighboring locations.
    void add_location(const std::string& direction, Location* location); ///< Adds a neighboring location.
    void add_npc(NPC& npc); ///< Adds an NPC to the location.
    std::vector<NPC>& get_npcs(); ///< Returns the list of NPCs in the location.
    void add_item(const Item& item); ///< Adds an item to the location.
    void remove_item(const Item& item); ///< Removes an item from the location.
    std::vector<Item> get_items() const; ///< Returns the list of items in the location.
    void set_visited(); ///< Marks the location as visited.
    bool get_visited() const; ///< Returns whether the location has been visited.
    std::string getName() const; ///< Returns the name of the location.

    /**
     * @brief Overloads the << operator to print Location details.
     * @param os The output stream.
     * @param location The Location object to print.
     * @return The output stream.
     */
    friend std::ostream& operator<<(std::ostream& os, const Location& location);
};

/**
 * @class Game
 * @brief Represents the game, managing the player's interactions, inventory, and world state.
 */
class Game {
public:
    Game(); ///< Constructs a Game object and initializes the game world.
    void play(); ///< Starts the game loop.
    void executeCommand(std::string command, std::vector<std::string> args); ///< Executes a game command.
    void showHelp(std::vector<std::string> args); ///< Displays a list of available commands.
    void talk(std::vector<std::string> target); ///< Allows the player to talk to an NPC.
    void kiss(std::vector<std::string> target); ///< Allows the player to kiss an NPC.
    void take(std::vector<std::string> target); ///< Allows the player to take an item.
    void give(std::vector<std::string> target); ///< Allows the player to give an item.
    void go(std::vector<std::string> target); ///< Allows the player to move to a new location.
    void look(std::vector<std::string> target); ///< Allows the player to look around the current location.
    void quit(std::vector<std::string> target); ///< Quits the game.
    void showInventory(std::vector<std::string> target); ///< Displays the player's inventory.
    void teleport(std::vector<std::string> target); ///< Teleports the player to a discovered location.

private:
    std::map<std::string, std::function<void(Game*, std::vector<std::string>)>> commands; ///< A map of available commands.
    float currentWeight; ///< The current weight of the player's inventory.
    bool isInPotty = false; ///< Whether the player is in the Porta-Potty.
    const int maxWeight = 50; ///< The maximum weight the player can carry.
    std::vector<Item> inventory; ///< The player's inventory.
    std::vector<Location> locations; ///< A list of all locations in the game.
    Location* currentLocation; ///< The player's current location.
    int caloriesNeeded; ///< The number of calories (or "awesome points") needed to complete the game.
    bool inProgress; ///< Whether the game is still in progress.
    void createWorld(); ///< Initializes the game world with locations, NPCs, and items.
    std::map<std::string, std::function<void(Game*, std::vector<std::string>)>> setup_commands(); ///< Sets up the available commands.
    Location* randomLocation(); ///< Returns a random location from the list of locations.
    std::map<std::string, std::string> commandAliases; ///< A map of command aliases.
    std::string toLowercase(const std::string& str); ///< Converts a string to lowercase.
};

#endif