#include <iostream>
#include <map>
#include <algorithm>
#include <cpr/cpr.h>
#include <json/json.h>
#include <tgbot/tgbot.h>
#include "candle.h"

struct UserState {
    bool waitingForName = false;
    bool waitingForReason = false;
    std::string name;
};

std::map<int64_t, UserState> userStates;

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, ".UTF-8");

    candle c("log.txt");
    TgBot::Bot bot("YOUR TOKEN");

    bot.getEvents().onCommand("start", [&bot, &c](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Hello. In this bot you can light a candle for free.");
        bot.getApi().sendMessage(message->chat->id, "Author: @MaxRayButler");
        userStates[message->chat->id] = UserState();
        });

    bot.getEvents().onCommand("postSV", [&bot, &c](TgBot::Message::Ptr message) {
        userStates[message->chat->id] = UserState();
        userStates[message->chat->id].waitingForName = true;
        bot.getApi().sendMessage(message->chat->id, "Who are we lighting a candle for?");
        });

    bot.getEvents().onAnyMessage([&bot, &c](TgBot::Message::Ptr message) {
        if (message->from->id == 7035683268 and message->text == "exit")
        {
            c.addToDatabase(message->from->id, "ADMIN SENT EXIT COMMAND");
        }

        if (StringTools::startsWith(message->text, "/")) {
            return;
        }

        int64_t chatId = message->chat->id;

        c.addToDatabase(message->from->id, message->text);

        if (userStates[chatId].waitingForName) {
            userStates[chatId].name = message->text;
            userStates[chatId].waitingForName = false;
            userStates[chatId].waitingForReason = true;
            bot.getApi().sendMessage(chatId, "For what reason do we put a candle for " + message->text + "?");
        }

        else if (userStates[chatId].waitingForReason) {
            std::string reason = message->text;
            int result = c.lightCandle(userStates[chatId].name, reason);
            if (result)
            {
                bot.getApi().sendMessage(chatId, "The candle for " + userStates[chatId].name +
                    " for a reason '" + reason + "' was lighted!");
            }
            else { bot.getApi().sendMessage(chatId, "Internal error: Message not sent"); }

            userStates[chatId] = UserState();
        }

        else {
            bot.getApi().sendMessage(chatId, "To light a candle, use command: /postSV");
        }
        });

    try {
        std::cout << "Bot username: " << bot.getApi().getMe()->username << std::endl;
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            std::cout << "Long poll started" << std::endl;
            c.flushLogs();
            longPoll.start();
        }
    }
    catch (const TgBot::TgException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

