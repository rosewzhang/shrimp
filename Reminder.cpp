#include "Reminder.hpp"

Reminder::Reminder(std::uint64_t user_id, std::uint64_t channel_id,
        const std::string& name, const std::string& ping_message, const std::string& reward_message, const std::string& shame_message) 
    : user_id{user_id}, channel_id{channel_id}, name{name}, ping_message{ping_message}, reward_message{reward_message}, shame_message{shame_message}
{
    last_ping = 0;
    // TODO: update this whenever i do ping times
}

bool Reminder::tick(int time_elapsed)
{
    return true;
}

std::uint64_t Reminder::get_user_id() const
{
    return user_id;
}

std::uint64_t Reminder::get_channel_id() const
{
    return channel_id;
}

const std::string& Reminder::get_name() const
{
    return name;
}

const std::string& Reminder::get_ping_message() const
{
    return ping_message;
}

const std::string& Reminder::get_reward_message() const
{
    return reward_message;
}

const std::string& Reminder::get_shame_message() const
{
    return shame_message;
}
