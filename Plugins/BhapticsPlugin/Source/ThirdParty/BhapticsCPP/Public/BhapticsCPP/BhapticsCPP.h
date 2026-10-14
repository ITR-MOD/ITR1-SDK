//Copyright bHaptics Inc. 2022+

#ifndef BHAPTICS_LIBRARY_H
#define BHAPTICS_LIBRARY_H



#if defined _WIN32 || defined __CYGWIN__
#ifdef BHAPTICS_BUILDING_DLL
#ifdef __GNUC__
#define DLL_PUBLIC __attribute__ ((dllexport))
#else
#define DLL_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
#endif
#else
#ifdef __GNUC__
#define DLL_PUBLIC __attribute__ ((dllimport))
#else
#define DLL_PUBLIC __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
#endif
#endif
#define DLL_LOCAL
#else
#if __GNUC__ >= 4
#define DLL_PUBLIC __attribute__ ((visibility ("default")))
#define DLL_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define DLL_PUBLIC
#define DLL_LOCAL
#endif
#endif


#ifdef __cplusplus
extern "C" {
#endif
    DLL_PUBLIC bool registryAndInit(const char* sdkAPIKey, const char* workspaceId, const char* initJsonMessage);
    DLL_PUBLIC bool registryAndInitHost(const char* sdkAPIKey, const char* workspaceId, const char* initJsonMessage, const char* url);
    DLL_PUBLIC bool wsIsConnected();
    DLL_PUBLIC void wsClose();
    DLL_PUBLIC bool reInitMessage(const char* sdkAPIKey, const char* workspaceId, const char* initJsonMessage);

    DLL_PUBLIC int play(const char* key);
    DLL_PUBLIC int playPos(const char* key, int position);
    DLL_PUBLIC int playPosParam(const char* key, int position, float intensity, float duration, float angleX, float offsetY);
    DLL_PUBLIC int playDot(int position, int duration, int* motorValues, int motorValueLen);
    DLL_PUBLIC int playWaveform(int position,int* motorValues, int* playTimeValues, int* shapeValues, int releatCount, int motorLen);
    DLL_PUBLIC int playPath(int positiom, int durationMillis, float* xValues, float* yValues, int* intensityValues, int motorLen);
    DLL_PUBLIC int playLoop(const char* key, float intensity, float duration, float angleX, float offsetY, int interval, int maxCount);

    DLL_PUBLIC bool stop(int requestKey);
    DLL_PUBLIC bool stopByEventId(const char* eventId);
    DLL_PUBLIC bool stopAll();

    DLL_PUBLIC bool isbHapticsConnected(int position);
    DLL_PUBLIC bool isPlaying();
    DLL_PUBLIC bool isPlayingByRequestId(const int requestId);
    DLL_PUBLIC bool isPlayingByEventId(const char* eventId);

    DLL_PUBLIC const char* bHapticsGetHapticMessage(const char* appKey, const char* workspaceId, int lastVerison, int& status);
    DLL_PUBLIC const char* bHapticsGetHapticMappings(const char* appKey, const char* workspaceId, int lastVerison, int& status);

    DLL_PUBLIC bool isPlayerRunning();
    DLL_PUBLIC bool isPlayerInstalled();
    DLL_PUBLIC bool launchPlayer(bool tryLaunch);

    DLL_PUBLIC const char* getDeviceInfoJson();
    DLL_PUBLIC const char* getHapticMappingsJson();

    DLL_PUBLIC bool ping(const char* address);
    DLL_PUBLIC bool pingAll();
    DLL_PUBLIC bool swapPosition(const char* address);
#ifdef __cplusplus
};
#endif

#endif //BHAPTICS_LIBRARY_H