// DPP is a Discord Library
#include <dpp/dpp.h>

#include <memory>
#include <functional>
#include <fstream>
#include <iostream>
#include <thread>
#include <cstdio>

#include "Bot.hpp"

constexpr int TICK_PERIOD = 1;

std::string read_from_file(const char* filepath);

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "Pass file with bot token as first argument." << std::endl;
        exit(-1);
    }
    if (argc < 3)
    {
        std::cout << "Pass output file as second argument." << std::endl;
        exit(-1);
    }
    std::string bot_token = read_from_file(argv[1]);
    std::uint32_t intents = 0;
    intents += dpp::intents::i_message_content;
    intents += dpp::intents::i_guild_messages;
    std::unique_ptr<Bot> bot_ptr;
    if (argc >= 4)
    {
        std::printf("making the bot with an input file\n");
        bot_ptr = std::make_unique<Bot>(bot_token, intents, argv[2], argv[3]);
    }
    else
    {
        std::printf("making the bot without an input file\n");
        bot_ptr = std::make_unique<Bot>(bot_token, intents, argv[2]);
    }
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(TICK_PERIOD));
        bot_ptr->tick();
    }
}

std::string read_from_file(const char* filepath)
{
    std::string contents;
    std::ifstream file{};
    file.open(filepath);
    std::getline(file, contents);
    file.close();
    return contents;
}
