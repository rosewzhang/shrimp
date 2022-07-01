#ifndef REMINDER_HPP
#define REMINDER_HPP

#include <cstdint>
#include <string>

class Reminder
{
private:
    std::uint64_t user_id;
    std::uint64_t channel_id;
    std::string name;
    std::string ping_message;
    std::string reward_message;
    std::string shame_message;
    int score;
    int last_ping; // TODO: replace this with whatever time type is relevant
public:
    Reminder(std::uint64_t user_id, std::uint64_t channel_id, const std::string& name, const std::string& ping_message,
            const std::string& reward_message, const std::string& shame_message);
    ~Reminder() = default;
    Reminder(const Reminder& other) = default;
    Reminder(Reminder&& other) = default;
    Reminder& operator=(const Reminder &other) = default;
    Reminder& operator=(Reminder &&other) = default;
public:
    // returns true if a ping should be sent
    bool tick(int time_elapsed); // TODO: replace this with whatever time type is relevant
    std::uint64_t get_user_id() const;
    std::uint64_t get_channel_id() const;
    const std::string& get_name() const;
    const std::string& get_ping_message() const;
    const std::string& get_reward_message() const;
    const std::string& get_shame_message() const;
};

#endif
