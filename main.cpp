#include <jni.h>
#include <android/log.h>
#include <string>

#define LOG_TAG "Freecam-Native"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

struct Vec3 {
    float x, y, z;
};

// Global States
static bool g_FreecamEnabled = false;
static Vec3 g_FreecamPos = {0.0f, 0.0f, 0.0f};

// Original Function Pointers for Hooks
typedef void (*sendChatMessage_t)(void* localPlayer, const std::string& message);
static sendChatMessage_t o_sendChatMessage = nullptr;

typedef Vec3* (*getRenderPosition_t)(void* self, Vec3* outPos, float partialTicks);
static getRenderPosition_t o_getRenderPosition = nullptr;

// Hook 1: Camera Decouple & Block Noclip
Vec3* hk_getRenderPosition(void* self, Vec3* outPos, float partialTicks) {
    Vec3* res = o_getRenderPosition(self, outPos, partialTicks);
    
    if (res && g_FreecamEnabled) {
        // Return virtual freecam position directly (bypasses block raycasts / noclip)
        *outPos = g_FreecamPos;
    } else if (res) {
        // Sync starting freecam position with player's real position when OFF
        g_FreecamPos = *res;
    }
    return res;
}

// Hook 2: Chat Trigger Interception ("fc")
void hk_sendChatMessage(void* localPlayer, const std::string& message) {
    // Check if player typed "fc" in chat
    if (message == "fc" || message == ".fc") {
        g_FreecamEnabled = !g_FreecamEnabled;
        
        if (g_FreecamEnabled) {
            LOGI("[Freecam] Activated! Noclip Enabled.");
        } else {
            LOGI("[Freecam] Deactivated!");
        }
        
        // Return without calling original function so "fc" isn't sent to server chat
        return;
    }

    // Pass all other normal chat messages to server
    if (o_sendChatMessage) {
        o_sendChatMessage(localPlayer, message);
    }
}

// Module Constructor
__attribute__((constructor))
void mod_init() {
    LOGI("LeviPack Freecam with Chat Toggle & Noclip Loaded!");
}

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    return JNI_VERSION_1_6;
}
