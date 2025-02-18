// At a metal festival, have to build the ultimate guitar, EVH bridge pickup from Hell
// James Hetfield breaks his arm and you have to replace him, but in order to be the best
// guitarist of all time you need to put together the *COOL_GUITAR_NAME* to play the biggest show
// of all time, MOSCCOW 2045

// Locations: 3 Floyds Beer Tent, Main Backstage (where you meet James), portapotty to hell (break up sections?)
// side stage1 backstage, side stage2 backstage, side stage2 backstage, Founders Beer Tent, Mysterious camper's tent

// NPCs: Lars, Kirk, EVH, Ozzy, James, Dave Mustaine, Lemmy, Dime & Vinnie, Jason Newsted,

// Items: EVH bridge pickup from hell, Blacktooth Grin, Dimebag's Neck, Tony Iommi's strings,
// Jimmy Page's Neck Pickup, Jerry Cantrell's pickguard, Chuck Schuldiner's Pick, Kurt Cobain's Strings
// Founder's beers (2), 3 floyds beers (2)

// Alternate endings, drinking all the beers gives the best ending

// drunkness meter?

#include "gvzork.h"
#include <functional>
#include <iostream>
#include <map>
#include <string>

Game::Game() {
    commands["drink"] = std::bind(&Game::drink, this, std::placeholders::_1);
    commands["help"] = std::bind(&Game::showHelp, this, std::placeholders::_1);
    commands["look"] = std::bind(&Game::look, this, std::placeholders::_1);
    commands["move"] = std::bind(&Game::move, this, std::placeholders::_1);
    commands["quit"] = std::bind(&Game::quit, this, std::placeholders::_1);
    commands["take"] = std::bind(&Game::take, this, std::placeholders::_1);
    commands["talk"] = std::bind(&Game::talk, this, std::placeholders::_1);

    createWorld();
    currentLocation = &locations[0]; // Set starting location
    drunkness = 0;
}

void Game::createWorld() {
    std::cout << "Creating the world" << std::endl;
    // initialize everything
}

void Game::drink(std::vector<std::string> args) {
    std::cout << "Drinking..." << std::endl;
    drunkness++;
}

void Game::executeCommand(std::string command, std::vector<std::string> args) {
    if (commands.find(command) != commands.end()) {
        commands[command](args);
    } else {
        std::cout << "Unknown command! Type 'help' for a list of commands." << std::endl;
    }
}

void Game::look(std::vector<std::string> args) {
    std::cout << "Looking around..." << std::endl;
}

void Game::move(std::vector<std::string> args) {
    std::cout << "Moving..." << std::endl;
}

void Game::play() {
    // The logic for the play function
    std::cout << "Starting the game..." << std::endl;
    // Your game loop or main logic here
}

void Game::quit(std::vector<std::string> args) {
    std::cout << "Quitting game..." << std::endl;
    exit(0);
}

void Game::showHelp(std::vector<std::string> args) {
    std::cout << "Available commands:" << std::endl;
    for (const auto& cmd : commands) {
        std::cout << " - " << cmd.first << std::endl;
    }
}

void Game::talk(std::vector<std::string> args) {
    std::cout << "Talking..." << std::endl;
}

int main() {
    Game game;
    game.play();
    return 0;
}