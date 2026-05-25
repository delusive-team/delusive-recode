#pragma once
#include "include/discord_register.h"
#include "include/discord_rpc.h"
#include <Windows.h>
#include <string>
#include <ctime>

class Discord {
public:
    // Получение единственного экземпляра
    static Discord& GetInstance() {
        static Discord instance;
        return instance;
    }

    // Удаляем методы копирования
    Discord(const Discord&) = delete;
    Discord& operator=(const Discord&) = delete;

    // Основные методы
    bool Initialize(void (*readyCallback)(const DiscordUser*) = nullptr);
    void Update();
    void Unload();

private:
    // Приватный конструктор
    Discord() = default;
    ~Discord();

    // Храним стейт, чтобы не пересоздавать каждый раз
    DiscordRichPresence discordPresence{};
    DiscordEventHandlers handlers{};
    bool isInitialized = false;
};
