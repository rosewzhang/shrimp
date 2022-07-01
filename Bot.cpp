#include "Bot.hpp"

#include <cstdio>
#include <thread>
#include <string>
#include <fstream>

namespace
{
    bool starts_with(const char* str, const char* substr)
    {
        if (*substr == '\0')
        {
            return true;
        }
        if (*str != *substr || *str == '\0')
        {
            return false;
        }
        return starts_with(str + 1, substr + 1);
    }
    
    inline char hex_to_nibble(char hex_char)
    {
        return (hex_char >= '0' || hex_char <= '9') ? (hex_char - '0') : (hex_char - 'a' + 10);
    }

    std::uint64_t read_uint64(const std::string& hex_str)
    {
        std::uint64_t result = 0;
        const char* cstr = hex_str.c_str();
        while (*cstr != '\0')
        {
            result <<= 4;
            result += hex_to_nibble(*cstr);
            ++cstr;
        }
        return result;
    }

    inline char nibble_to_hex(char nibble)
    {
        if (0 <= nibble && nibble <= 9)
        {
            return nibble + '0';
        }
        else if (10 <= nibble && nibble <= 16)
        {
            return nibble - 10 + 'a';
        }
        return -1;
    }

    std::string write_uint64(std::uint64_t uint64)
    {
        std::string result = "0000000000000000";
        // 16 = # of nibbles in 64 bits
        for (int i = 15; i >= 0; --i)
        {
            result[i] = nibble_to_hex(uint64 & 0xff);
            uint64 >>= 4;
        }
        return result;
    }
}

Bot::Bot(std::string token, std::uint32_t intents, const std::string& output_filepath)
    : dpp_bot{token, intents}, last_tick{std::time(NULL)}, output_filepath{output_filepath}
{
    // the mutex starts off locked and is only unlocked when on_ready finishes executing so that
    // no early ticks happen
    mutex.lock();
    dpp_bot.on_log(dpp::utility::cout_logger());
    dpp_bot.on_ready([&](const dpp::ready_t& event) {on_ready(event, true);});
    dpp_bot.on_slashcommand([&](const dpp::slashcommand_t& event) {on_slashcommand(event);});
    dpp_bot.on_message_create([&](const dpp::message_create_t& event) {on_message_create(event);});
    dpp_bot.on_message_reaction_add([&](const dpp::message_reaction_add_t& event) {on_message_reaction_add(event);});
    dpp_thread = std::thread{[&](){dpp_bot.start(false);}};
}

void Bot::tick()
{
    std::lock_guard<std::mutex> lock_guard{mutex};
    std::printf("tick!\n");
    write_to_file();
    log_self_debug();
}

void Bot::on_ready(const dpp::ready_t& event, bool unlock)
{
    if (dpp::run_once<struct register_bot_commands>())
    {
        dpp_bot.global_command_create(dpp::slashcommand("ping", "Ping pong!", dpp_bot.me.id));
    }
    if (unlock)
    {
        mutex.unlock();
    }
}

void Bot::on_slashcommand(const dpp::slashcommand_t& event)
{
    if (event.command.get_command_name() == "ping")
    {
        event.reply("Pong!");
    }
}

void Bot::on_message_create(const dpp::message_create_t& event)
{
    const char* msgcstr = event.msg.content.c_str();
    const dpp::snowflake user_id = event.msg.author.id;
    const dpp::snowflake channel_id = event.msg.channel_id;
    if (!starts_with(msgcstr, "!shrimp"))
    {
        return;
    }
    msgcstr += strlen("!shrimp");
    if (msgcstr[0] == '\0')
    {
        send_message("help", channel_id);
        return;
    }
    ++msgcstr;
    if (starts_with(msgcstr, "newreminder"))
    {
        //TODO: implement adding
        send_message("newreminder not yet implemented", channel_id);
        return;
    }
    if (starts_with(msgcstr, "deletereminder"))
    {
        //TODO: implement deleting
        send_message("deletereminder not yet implemented", channel_id);
        return;
    }
    send_message("invalid command:", channel_id);
    send_message(msgcstr, channel_id);
}

void Bot::on_message_reaction_add(const dpp::message_reaction_add_t& event)
{
}

inline dpp::message&& Bot::send_message(const std::string& message, const dpp::snowflake& channel_id)
{
    dpp::message dpp_msg = dpp::message{channel_id, message};
    dpp_bot.message_create(dpp_msg);
    return std::move(dpp_msg);
}

Bot::Bot(std::string token, std::uint32_t intents, const std::string& output_filepath, const std::string& input_filepath)
    : Bot{token, intents, output_filepath}
{
    std::lock_guard<std::mutex> lock_guard{mutex};
    std::ifstream fstream;
    fstream.open(input_filepath);
    std::string line;
    std::getline(fstream, line);
    int num_users = std::atoi(line.c_str());
    for (int i = 0; i < num_users; ++i)
    {
        std::getline(fstream, line);
        std::getline(fstream, line);
        std::uint64_t user_id = read_uint64(line);
        std::getline(fstream, line);
        std::uint64_t channel_id = read_uint64(line);
        std::string name;
        std::getline(fstream, name);
        std::string ping_message;
        std::getline(fstream, ping_message);
        std::string reward_message;
        std::getline(fstream, reward_message);
        std::string shame_message; 
        std::getline(fstream, shame_message);
        ongoing_reminders.push_back(Reminder{user_id, channel_id, name, ping_message, reward_message, shame_message});
    }
    fstream.close();
}

// assumes that the caller has a lock to the mutex
void Bot::write_to_file()
{
    std::ofstream fstream;
    fstream.open(output_filepath);
    fstream << ongoing_reminders.size() << '\n';
    for (const Reminder& reminder : ongoing_reminders)
    {
        fstream << '\n';
        fstream << write_uint64(reminder.get_user_id()) << '\n';
        fstream << write_uint64(reminder.get_channel_id()) << '\n';
        fstream << reminder.get_name() << '\n';
        fstream << reminder.get_ping_message() << '\n';
        fstream << reminder.get_reward_message() << '\n';
        fstream << reminder.get_shame_message() << '\n';
    }
    fstream.close();
}

void Bot::log_self_debug()
{
    std::string str = "bot log: \n\n";
    str += "ongoing reminders: \n\n";
    for (const Reminder& reminder : ongoing_reminders)
    {
        str += write_uint64(reminder.get_user_id());
        str += "\n";
        str += write_uint64(reminder.get_channel_id());
        str += "\n";
        str += reminder.get_name();
        str += "\n";
        str += reminder.get_ping_message();
        str += "\n";
        str += reminder.get_reward_message();
        str += "\n";
        str += reminder.get_shame_message();
        str += "\n\n";
    }
    dpp_bot.log(dpp::loglevel::ll_debug, str);
}
