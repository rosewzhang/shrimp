#ifndef BOT_HPP
#define BOT_HPP

#include <dpp/dpp.h>
#include <functional>
#include <ctime>
#include <cstdint>
#include <vector>
#include <mutex>

#include "Reminder.hpp"

struct BotException
{
    std::string message;
};

class Bot
{
private:
    std::mutex mutex;
    dpp::cluster dpp_bot;
    std::vector<Reminder> ongoing_reminders;
    std::time_t last_tick;
    std::thread dpp_thread;
    std::string output_filepath;
public:
    Bot(std::string token, std::uint32_t intents, const std::string& output_filepath);
    Bot(std::string token, std::uint32_t intents, const std::string& output_filepath, const std::string& input_filepath);
    ~Bot() = default;
    Bot(const Bot& other) = delete;
    Bot(Bot&& other) = delete;
    Bot& operator=(const Bot& other) = delete;
    Bot& operator=(Bot&& other) = delete;
public: 
    void log_self_debug();
    void tick(); 
private:
    void on_ready(const dpp::ready_t& event, bool unlock);
    void on_slashcommand(const dpp::slashcommand_t& event);
    void on_message_create(const dpp::message_create_t& event);
    void on_message_reaction_add(const dpp::message_reaction_add_t& event);
    dpp::message&& send_message(const std::string& message, const dpp::snowflake& channel_id);
    void write_to_file();
    std::string string_repr();
};

#endif
