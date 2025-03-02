#include "gvzork.h"
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <random>


Item::Item(const std::string& name, const std::string& description, int calories, float weight) {
    if (name.empty()) throw std::invalid_argument("Name cannot be blank.");
    if (description.empty()) throw std::invalid_argument("Description cannot be blank.");
    if (calories < 0 || calories > 1000) throw std::invalid_argument("Calories must be between 0 and 1000.");
    if (weight < 0 || weight > 500) throw std::invalid_argument("Weight must be between 0 and 500.");

    this->name = name;
    this->description = description;
    this->calories = calories;
    this->weight = weight;
}

std::string Item::getName() const { return name; }
std::string Item::getDescription() const { return description; }
int Item::getCalories() const { return calories; }
float Item::getWeight() const { return weight; }

std::ostream& operator<<(std::ostream& os, const Item& item) {
    os << item.name << " (" << item.calories << " awesome points)- " << item.weight << " lb- " << item.description;
    return os;
}


NPC::NPC(const std::string& name, const std::string& description) {
    if (name.empty() || description.empty()) {
        throw std::invalid_argument("Name and description cannot be blank.");
    }

    this->name = name;
    this->description = description;
    this->messageNumber = 0;
    }
    
    std::string NPC::getName() const { return name; }
    
    std::string NPC::getDescription() const { return description; }


	void NPC::addMessage(const std::string& message) {
    messages.push_back(message);  // Add the message to the vector
}

    std::string NPC::getMessage() {
        if (messages.empty()) {
            return ("This NPC has no messages.");
        }
        std::string message = messages[messageNumber];
        messageNumber = (messageNumber + 1) % messages.size();
        return message;
    }

    std::ostream& operator<<(std::ostream& os, const NPC& npc) {
        os << npc.name;
        return os;
    }

Location::Location(const std::string& name, const std::string& description) {
    if (name.empty() || description.empty()) throw std::invalid_argument("Name and description cannot be blank.");
    this->name = name;
    this->description = description;
    this->visited = false;
    }
    
    std::string Location::getName() const { return name; }

    std::map<std::string, Location*> Location::get_locations() const { return neighbors;}

    void Location::add_location(const std::string& direction, Location* location){
        if (direction.empty()){
            throw std::invalid_argument("Direction cannot be empty.");
        }
        if (neighbors.count(direction) > 0){
            throw std::invalid_argument("That direction is already mapped for this location.");
        }
        neighbors[direction] = location;
    }

    void Location::add_npc(NPC& npc) { // Store NPC reference
        npcs.push_back(npc);
    }

    std::vector<NPC>& Location::get_npcs() {
        return npcs; // Return references instead of copies
    }

    void Location::add_item(const Item& item){
        items.push_back(item);
    }

    void Location::remove_item(const Item& item) {
    auto it = std::find_if(items.begin(), items.end(),
        [&item](const Item& i) { return i.getName() == item.getName(); });
    if (it != items.end()) {
        items.erase(it);
    }
    }
    std::vector<Item> Location::get_items() const { return items;}

    void Location::set_visited(){
        visited = true;
    }

    bool Location::get_visited() const {
        if (this->visited) {
            return true;
        }
        return false;
    }

    std::ostream& operator<<(std::ostream& os, const Location& location) {
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
                os << "- " << item.getName() << " (" << item.getCalories() << " awesome points) - "
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



Game::Game() {
    commands = setup_commands();
    createWorld();
    currentWeight = 0;
    caloriesNeeded = 500;
    inProgress = true;
    currentLocation = randomLocation();
    drunkness = 0;

    if (currentLocation) {
        currentLocation->set_visited();
    } else {
        throw std::runtime_error("Error: No valid starting location.");
    }
}

std::map<std::string, std::function<void(Game*, std::vector<std::string>)>> Game::setup_commands() {
    std::map<std::string, std::function<void(Game*, std::vector<std::string>)>> commands;
    commands.insert(std::make_pair("help", &Game::showHelp));
    commands.insert(std::make_pair("talk", &Game::talk));
    commands.insert(std::make_pair("take", &Game::take));
    commands.insert(std::make_pair("get", &Game::take));
    commands.insert(std::make_pair("give", &Game::give));
    commands.insert(std::make_pair("go", &Game::go));
    commands.insert(std::make_pair("look", &Game::look));
    commands.insert(std::make_pair("quit", &Game::quit));
    commands.insert(std::make_pair("exit", &Game::quit));
    commands.insert(std::make_pair("i", &Game::showInventory));
    commands.insert(std::make_pair("kiss", &Game::kiss));

    return commands;
}

void Game::createWorld() {

  	    std::cout << R"(
\m/*******************************************\m/
*  METALZORK: Metalapokolips 2: The First One  *
*          THE ULTIMATE RIFF QUEST             *
\m/*******************************************\m/

You're trapped in the most brutal metal festival of all time.
The air reeks of burnt amplifiers and monster energy drinks.

YOUR MISSION:
James Hetfield broke his wrist and cant play the show toingt!
Now it's up to you, a young opener to take his place. However, the only
way to take his place and play a legenedary show with Metallica is...
with the ultimate guitar!!!

Collect guitar parts from the festival grounds and deliver them
to Dean Zelinsky at the VIP Lounge. He needs 500 awesome points
to forge the guitar that will save metal forever.

COMMANDS:
- GO [direction] (north/south/east/west/etc)
- LOOK           (survey your surroundings)
- TALK [name]    (chat with metal legends)
- TAKE [item]    (acquire sweet gear)
- GIVE [item]    (contribute to the ultimate axe)
- INVENTORY      (check your loot)
- HELP           (show commands)
- QUIT           (abandon the pit)

The crowd is getting restless... Go melt some faces!
)" << std::endl;



    // Create locations and add them to the vector
    locations.push_back(Location("Main Stage", "The heart of the festival, a colossal stage towering over the crowd. Flames erupt from the stage as the band rips into a brutal breakdown."));
    locations.push_back(Location("Second Stage", "A slightly smaller stage, but still packed with energy. The air smells like sweat, beer, and distortion pedals cranked to 11."));
    locations.push_back(Location("Third Stage", "A more underground stage, featuring extreme metal bands. The pit here is absolute chaos."));
    locations.push_back(Location("VIP Lounge", "An exclusive area behind the main stage. You hear whispers of legendary rockstars hanging out here."));
    locations.push_back(Location("Porta-Potty Row", "A long line of overused porta-potties. The air is thick with regret."));
    locations.push_back(Location("Porta-Potty", "Ew it stinks, and a carving on the wall says: *try down* weird."));
    locations.push_back(Location("Founders Beer Tent", "A massive beer tent, offering legendary brews. It’s crowded, but the drinks are worth it."));
    locations.push_back(Location("Three Floyds Beer Tent", "Another beer tent, home to Zombie Dust and more. You overhear someone say, 'Best beer at the fest!'"));
    locations.push_back(Location("Merch Booths", "A row of tents selling band shirts, records, and obscure patches. You spot a rare vinyl you’ve been hunting for years."));
    locations.push_back(Location("Food Court", "A collection of food trucks selling everything from greasy festival burgers to vegan burritos."));
    locations.push_back(Location("Medical Tent", "A small white tent with a red cross. Someone inside is getting their wounds patched up from a wild mosh pit."));
    locations.push_back(Location("Camping Grounds", "A sea of tents and campfires, where festival-goers rest between sets. Smells like beer, weed, and cheap ramen."));
    locations.push_back(Location("Parking Lot", "A large open area filled with cars. It’s noisy and smells like gasoline. Why did you come here?"));
    locations.push_back(Location("Hell", "You’ve somehow found yourself in Hell. But wait, is that Dimebag Darrell shredding in the distance?")); // Secret cheat code location

    // Add NPCs to locations

     // 0: MS, 1: 2S, 2: 3S, 3: VIP, 4: PortaRow, 5: Potty, 6: Founders, 7: 3Floyds, 8: Merch
    // 9: FC, 10: Meds, 11: Camp, 12: Parking Lot, 13: hell

    NPC luthier("Dean", "Dean Zelinsky, a legendary luthier some even say he has powers.");
    luthier.addMessage("I need quality parts to build the ultimate axe!");
    luthier.addMessage("That's the stuff! Keep 'em coming!");
    luthier.addMessage("One more piece and this baby will scream!");
    locations[3].add_npc(luthier); // Add to VIP Lounge

    NPC soundEngineer("Sound Engineer", "A stressed-looking guy adjusting the mix.");
    soundEngineer.addMessage("If you mess with my soundboard, I swear to Dio…");
    soundEngineer.addMessage("This mix is the difference between a killer set and total disaster.");
    locations[0].add_npc(soundEngineer);

    NPC securityGuard("Security Guard", "A no-nonsense security guard scanning the crowd.");
    securityGuard.addMessage("Keep it safe, but go hard.");
    securityGuard.addMessage("No crowd surfing past the barricade!");
    locations[6].add_npc(securityGuard);

    NPC roadie("Roadie", "A rugged roadie moving amps.");
    roadie.addMessage("You think this job is easy? Load in at 6 AM, load out at 2 AM.");
    roadie.addMessage("We run this festival, not the bands.");
    locations[1].add_npc(roadie);

    NPC beerVendor("Beer Vendor", "A cheerful vendor pouring pints.");
    beerVendor.addMessage("One sip of this, and you'll be ready for the next set!");
    beerVendor.addMessage("We ran out of IPA? Damn, that was fast.");
    locations[5].add_npc(beerVendor);

    // Metal legends in the VIP Lounge
    NPC ozzy("Ozzy Osbourne", "The Prince of Darkness himself, sipping a drink in the VIP Lounge.");
    ozzy.addMessage("Sharon! Where’s my bloody bat?!");
    ozzy.addMessage("Metal ain't dead, mate. Just evolving.");
    locations[3].add_npc(ozzy);

    NPC lemmy("Lemmy Kilmister", "The legendary Motörhead frontman, playing a slot machine in the corner.");
    lemmy.addMessage("If you think you’re too old for rock and roll, then you are.");
    lemmy.addMessage("Ace of Spades, mate! That’s the only song you need.");
    locations[3].add_npc(lemmy);

    NPC dimebag("Dimebag Darrell", "A ghostly presence, now a true Cowboy of Hell.");
    dimebag.addMessage("Dude, you made it to Hell? That’s METAL!");
    dimebag.addMessage("I got riffs that’d melt your face off. Want a lesson?");
    locations[13].add_npc(dimebag);

    NPC evh("Eddie Van Halen", "A ghostly presence, shredding in the fires of Hell.");
    dimebag.addMessage("What's up dude.");
    dimebag.addMessage("Wanna come try my rig?");
    locations[13].add_npc(evh);

    NPC ronnie("Ronnie James Dio", "The master of metal, throwing up the horns.");
    ronnie.addMessage("We are the last in line! Don’t forget that.");
    ronnie.addMessage("Man, Heaven and Hell still holds up!");
    locations[13].add_npc(ronnie);

     // 0: MS, 1: 2S, 2: 3S, 3: VIP, 4: PortaRow, 5: Potty, 6: Founders, 7: 3Floyds, 8: Merch
    // 9: FC, 10: Meds, 11: Camp, 12: Parking Lot, 13: hell

	locations[8].add_item(Item("Neck", "Maple guitar neck with rosewood fretboard", 50, 4.2));
	locations[1].add_item(Item("Body", "Solid mahogany body with flame top", 60, 8.5));
	locations[0].add_item(Item("Pickups", "High-output humbuckers with coil tapping", 45, 1.8));
	locations[3].add_item(Item("Tuners", "Locking machine heads for perfect tuning", 50, 0.9));
	locations[7].add_item(Item("Strings", "Heavy gauge nickel-wound strings", 45, 0.3));
	locations[11].add_item(Item("Floyd Rose", "Professional tremolo system", 65, 2.1));
	locations[2].add_item(Item("Bridge", "Fixed bridge for enhanced sustain", 55, 2.0));
	locations[13].add_item(Item("Pickguard", "Classic black pickguard", 40, 0.5));
    locations[0].add_item(Item("Nut", "Lol, Bone nut for better tone and sustain", 30, 0.1));
	locations[1].add_item(Item("Truss Rod", "Adjustable truss rod for neck stability", 35, 0.3));
	locations[2].add_item(Item("Volume Knob", "Gold-plated volume knob, a little dusty", 35, 0.2));
	locations[3].add_item(Item("Tone Knob", "Gold-plated tone knob actually kinda cool", 40, 0.2));
	locations[7].add_item(Item("Output Jack", "High-quality 1/4-inch output jack", 30, 0.1));
	locations[8].add_item(Item("Strap Buttons", "Secure locking strap buttons", 25, 0.2));
	locations[11].add_item(Item("Capacitor", "Orange drop capacitor for tone control", 30, 0.05));
	locations[13].add_item(Item("Dime's Floyd", "The Floyd Rose used by the goat himself", 120, 0.15));
    locations[13].add_item(Item("Hell Pickup", "Hand wound by EVH himself, this thing roars", 150, 0.15));


    // Main Stage
    locations[0].add_location("north", &locations[3]);   // VIP Lounge
    locations[0].add_location("south", &locations[9]);   // Food Court
    locations[0].add_location("east", &locations[1]);    // Second Stage
    locations[0].add_location("west", &locations[2]);    // Third Stage

    // Second Stage
    locations[1].add_location("west", &locations[0]);    // Back to Main Stage
    locations[1].add_location("east", &locations[6]);    // Founders Beer Tent

    // Third Stage
    locations[2].add_location("east", &locations[0]);    // Back to Main Stage
    locations[2].add_location("west", &locations[7]);    // Three Floyds Beer Tent

    // VIP Lounge
    locations[3].add_location("south", &locations[0]);   // Back to Main Stage

    // Food Court (9)
    locations[9].add_location("north", &locations[0]);   // Main Stage
    locations[9].add_location("south", &locations[10]);  // Medical Tent
    locations[9].add_location("east", &locations[6]);    // Founders Beer Tent
    locations[9].add_location("west", &locations[7]);    // Three Floyds Beer Tent
    locations[9].add_location("northeast", &locations[8]);// Merch Booths
    locations[9].add_location("northwest", &locations[4]);// Porta-Potty Row

    // Porta-Potty Row (4)
    locations[4].add_location("southeast", &locations[9]);// Back to Food Court
    locations[4].add_location("enter", &locations[5]);    // Into Porta-Potty

    // Porta-Potty
    locations[5].add_location("down", &locations[13]);   // Secret path to Hell

    // Medical Tent (10)
    locations[10].add_location("north", &locations[9]);  // Back to Food Court
    locations[10].add_location("south", &locations[11]); // Camping Grounds

    // Camping Grounds (11)
    locations[11].add_location("north", &locations[10]); // Back to Medical
    locations[11].add_location("east", &locations[12]);  // Parking Lot

    // Parking Lot (12)
    locations[12].add_location("west", &locations[11]);  // Back to Camping

    // Beer Tent Connections (6/7)
    locations[6].add_location("west", &locations[9]);    // Founders -> Food Court
    locations[7].add_location("east", &locations[9]);    // Three Floyds -> Food Court

    // Merch Booths (8)
    locations[8].add_location("southwest", &locations[9]);// Back to Food Court
}




void Game::executeCommand(std::string command, std::vector<std::string> args) {
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);

    if (commands.find(command) != commands.end()) {
        commands[command](this, args);  // Pass 'this' to refer to the current instance of the Game class
    } else {
        std::cout << "Unknown command! Type 'help' for a list of commands." << std::endl;
    }
}


//void Game::drink(std::vector<std::string> args) {
//    if (args.empty()) {
//        std::cout << "Drink what? You need to specify an item.\n";
//        return;
//    }
//
//    std::string drinkName = args[0];
//    auto it = std::find_if(inventory.begin(), inventory.end(), [&](const Item& item) {
//        return item.getName() == drinkName;
//    });
//
//    if (it == inventory.end()) {
//        std::cout << "You don't have a " << drinkName << " to drink.\n";
//        return;
//    }
//
//    // Remove the item from inventory after drinking
//    Item drink = *it;
//    inventory.erase(it);
//
//    // Increase drunkness if it's an alcoholic drink
//    if (drinkName == "Beer1" || drinkName == "Beer2" || drinkName == "Beer3") {
//        drunkness += 1;
//        std::cout << "You drink the " << drinkName << ". You feel a little tipsy. (Drunkness: " << drunkness << ")\n";
//    } else {
//        std::cout << "You drink the " << drinkName << ". Refreshing!\n";
//    }
//}

void Game::look(std::vector<std::string> args) {
    if (currentLocation) {
        std::cout << *currentLocation << std::endl;
    } else {
        std::cout << "You are in an unknown place..." << std::endl;
    }
}

void Game::quit(std::vector<std::string> args) { std::cout << "Quitting game..." << std::endl; exit(0); }

void Game::showHelp(std::vector<std::string> args) {
    std::cout << "Available commands:" << std::endl;
    for (const auto& cmd : commands) {
        std::cout << " - " << cmd.first << std::endl;
    }
}

void Game::showInventory(std::vector<std::string> args) {
    if (inventory.empty()) {
        std::cout << "Your inventory is empty.\n";
    } else {
        std::cout << "Your inventory contains:\n";
        for (const auto& item : inventory) {
            std::cout << "- " << item << std::endl;
        }
    }
    std::cout << "Current weight: " << currentWeight << "lbs\n";
}

void Game::take(std::vector<std::string> args) {
    std::vector<std::string> articles = {"the", "a"};

    if (args.size() > 0 && std::find(articles.begin(), articles.end(), args[0]) != articles.end()) {
        args.erase(args.begin());  // Remove the article
    }

    // Join the remaining arguments into a single string for the item name
    std::string fullItemName = "";
    for (const auto& arg : args) {
        if (!fullItemName.empty()) fullItemName += " ";
        fullItemName += arg;
    }

    // Search for the item in the current location's items
    bool itemFound = false;
    for (auto& item : currentLocation->get_items()) {
        if (item.getName() == fullItemName) {
            itemFound = true;
            inventory.push_back(item);  // Add item to the inventory
            currentLocation->remove_item(item);  // Remove item from the current location
            currentWeight += item.getWeight();
            std::cout << "You have taken the " << fullItemName << "." << std::endl;
            break;
        }
    }

    if (!itemFound) {
        std::cout << "Item not found in this location." << std::endl;
    }
}

void Game::give(std::vector<std::string> target) {

    std::vector<std::string> articles = {"the", "a"};
    if (std::find(articles.begin(), articles.end(), target[0]) != articles.end()) {
        target.erase(target.begin());
    }

    std::string itemName = "";
    for (const auto& arg : target) {
        if (!itemName.empty()) itemName += " ";
        itemName += arg;
    }

    // Find item in inventory
    auto it = std::find_if(inventory.begin(), inventory.end(),
        [&](const Item& i) { return i.getName() == itemName; });

    if (it == inventory.end()) {
        std::cout << "You don't have a " << itemName << " in your inventory.\n";
        return;
    }

    Item item = *it;
    inventory.erase(it);
    currentLocation->add_item(item);
    currentWeight -= item.getWeight();
    std::cout << "You gave the " << itemName << " to the location.\n";

    if (currentLocation->getName() == "VIP Lounge") {
        if (item.getCalories() > 0) {
            caloriesNeeded = std::max(0, caloriesNeeded - item.getCalories());
            std::cout << "Dean slaps the " << itemName << " on to the gutiar it was worth " << item.getCalories()
                      << " awesomeness points). Remaining needed: " << caloriesNeeded << "\n";
        } else {
            std::cout << "Dean didn't think that giving him a " << itemName << " was very funny. You are teleported to a new location!\n";
            currentLocation = randomLocation();
            std::cout << "You are now in: " << currentLocation->getName() << "\n";
        }
    }
}


void Game::go(std::vector<std::string> args) {

  	currentLocation->set_visited();

    if (args.empty()) {
        std::cout << "Go where? Please specify a direction.\n";
        return;
    }

     std::string direction = args[0]; // Assuming the direction is the first argument

    if (direction == "hell" && isInPotty) {
        std::cout << "A swirling vortex opens beneath you... Welcome to Hell!\n";
        currentLocation = &locations[12]; // Move player to Hell
        std::cout << *currentLocation << std::endl;
        return;
    }


    // Check if the current location has a neighbor in that direction
    auto it = currentLocation->neighbors.find(direction);
    if (it == currentLocation->neighbors.end()) {
        std::cout << "You can't go that way.\n";
        return;
    }

    // Move to the new location
    currentLocation = it->second;

    if (currentLocation->getName() == "Porta-Potty") {
        isInPotty = true;
    } else {
        isInPotty = false;
    }

    // Print the new location details
    std::cout << *currentLocation << std::endl;
}

Location* Game::randomLocation() {
    if (locations.empty()) {
        return nullptr;
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, locations.size() - 1);

    return &locations[dist(gen)];
}

void Game::kiss(std::vector<std::string> args) {

    if (!currentLocation) {
        std::cout << "No locations available to talk to." << std::endl;
        return;
    }

    std::vector<NPC>& npcs = currentLocation->get_npcs();
    if (npcs.empty()) {
        std::cout << "There are no NPCs to talk to in this location." << std::endl;
        return;
    }

    if (args.empty()) {
        std::cout << "You need to specify which NPC to talk to." << std::endl;
        return;
    }

    std::string npcName;
    for (size_t i = 0; i < args.size(); ++i) {
        npcName += args[i];
        if (i != args.size() - 1) npcName += " ";
    }

    for (NPC& npc : npcs) {
        if (npc.getName() == npcName) {
            std::cout << "You gingerly kiss " << npc.getName() << "... not very metal of you tbh" << std::endl;
            return;
        }
    }

    // If no NPC is found with the specified name
    std::cout << "No NPC named " << npcName << " in this location." << std::endl;
}

void Game::talk(std::vector<std::string> args) {

    if (!currentLocation) {
        std::cout << "No locations available to talk to." << std::endl;
        return;
    }

    std::vector<NPC>& npcs = currentLocation->get_npcs();
    if (npcs.empty()) {
        std::cout << "There are no NPCs to talk to in this location." << std::endl;
        return;
    }

    if (args.empty()) {
        std::cout << "You need to specify which NPC to talk to." << std::endl;
        return;
    }

    std::string npcName;
    for (size_t i = 0; i < args.size(); ++i) {
        npcName += args[i];
        if (i != args.size() - 1) npcName += " ";
    }

    for (NPC& npc : npcs) {
        if (npc.getName() == npcName) {
            std::cout << "You start a conversation with " << npc.getName() << "..." << std::endl;
            std::cout << npc.getMessage() << std::endl;
            return;
        }
    }

    // If no NPC is found with the specified name
    std::cout << "No NPC named " << npcName << " in this location." << std::endl;
}

void Game::play() {

    std::cout << "Starting the game..." << std::endl;

    std::string input;
    while (true) {
      	 if (caloriesNeeded <= 0) {
        	std::cout << "\n\nDean rummages frantically through the parts, mumbling to himself:\n"
          << "\"Neck joint... needs the Floyd Rose... where's the-\"\n"
          << "*CLANG* He drops a pickup, curses in dead languages, then freezes.\n\n"
          << "\"YES! THIS IS IT!\"\n"
          << "Dean's hands blur as he slams components together - \n"
          << "mahogany body screaming, strings glowing with forbidden energy.\n\n"
          << "He thrusts the finished guitar into your hands:\n"
          << "\"THE HELLAXE! Now go channel the rift before Metalapokolips collapses!\"\n\n"
          << "You stride onto the Main Stage. The crowd's roar becomes silence.\n"
          << "First chord - reality bends. Second chord - skies crack.\n"
          << "By the solo, the very fabric of the festival stabilizes,\n"
          << "pyrotechnics rewriting the laws of physics.\n\n"
          << "When the feedback dies, you're left with:\n"
          << "- A destroyed PA system\n"
          << "- Three record label contracts\n"
          << "- A crowd too hoarse to even whisper 'encore'\n\n"
          << "METALAPOKOLIPS HAS BEEN SAVED. \\m/\n";
        	inProgress = false;
    	 }
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input.empty()) continue;

        // Split input into command and arguments
        std::vector<std::string> args;
        std::string command;
        size_t pos = input.find(' ');
        if (pos != std::string::npos) {
            command = input.substr(0, pos);
            std::string remaining = input.substr(pos + 1);

            size_t argPos;
            while ((argPos = remaining.find(' ')) != std::string::npos) {
                args.push_back(remaining.substr(0, argPos));
                remaining = remaining.substr(argPos + 1);
            }
            if (!remaining.empty()) args.push_back(remaining);
        } else {
            command = input;
        }

        executeCommand(command, args);
    }
}

int main() {
    Game game;
    game.play();
    return 0;
}