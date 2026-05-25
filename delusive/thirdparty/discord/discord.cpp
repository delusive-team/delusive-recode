#include "Discord.h"

Discord::~Discord() {
    Unload();
}

bool Discord::Initialize(void (*readyCallback)(const DiscordUser*)) {
    if (isInitialized) return true;

    memset(&handlers, 0, sizeof(handlers));

    handlers.ready = readyCallback;

    Discord_Initialize("1505609490458738890", &handlers, 1, NULL);

    memset(&discordPresence, 0, sizeof(discordPresence));
    discordPresence.startTimestamp = std::time(nullptr);
    discordPresence.largeImageKey = "large_image_key_here";
    discordPresence.largeImageText = "Delusive Software";

    isInitialized = true;
    Update();

    return true;
}

void Discord::Update() {
    if (!isInitialized) return;

    discordPresence.details = "Playing with Delusive - Software"; // ѕерва€ строка
    discordPresence.state = "EAC Suck My COCK"; // ¬тора€ строка, добавл€юща€ больше контекста
    discordPresence.startTimestamp = time(nullptr);
    discordPresence.endTimestamp = 0;
    discordPresence.largeImageText = "Numbani";
    Discord_UpdatePresence(&discordPresence);
}

void Discord::Unload() {
    if (isInitialized) {
        Discord_ClearPresence();
        Discord_Shutdown();
        isInitialized = false;
    }
}
