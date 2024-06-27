#include "confighelpr.h"

#include <LittleFS.h>
#include <Preferences.h>

Preferences preferences;
void StoreData(const char *key, const char *val)
{
    preferences.begin("config", false);
    preferences.putString(key, val);
    preferences.end();
}
String ReadData(const char *val)
{
    preferences.begin("config", false);
    String ret = preferences.getString(val, "null");
    preferences.end();
    return ret;
}

void StoreintData(const char *key, int val)
{
    preferences.begin("config", false);
    preferences.putInt(key, val);
    preferences.end();
}
int ReadintData(const char *val)
{
    preferences.begin("config", false);
    int ret = preferences.getInt(val, 1000);
    preferences.end();
    return ret;
}

void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if (!root) {
        Serial.println("- failed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            Serial.print("  DIR : ");

            Serial.print(file.name());
            time_t t = file.getLastWrite();
            struct tm *tmstruct = localtime(&t);
            Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);

            if (levels) {
                listDir(fs, file.name(), levels - 1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");

            Serial.print(file.size());
            time_t t = file.getLastWrite();
            struct tm *tmstruct = localtime(&t);
            Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
        }
        file = root.openNextFile();
    }
}

void LittleFS_init()
{
    if (!LittleFS.begin()) {
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
    }
    Serial.println("LittleFS init succesful");
}
bool readfsDirlist(std::vector<String> &musiclist)
{
    File file = LittleFS.open("/musiclist.txt", "r");
    if (!file) {
        Serial.println("Failed to open file for reading");
        return false;
    }

    Serial.println("File Content:");
    int count = 0;
    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim();
        musiclist.push_back(line);
        Serial.println(line);
        if (++count >= MAX_MUSIC_NUM) {
            Serial.println("Too many music files, truncated");
            break;
        }
    }
    file.close();
    return true;
}
String musicSubstring(String str)
{
    int lastSlashIndex = str.lastIndexOf('/');
    if (lastSlashIndex != -1) {
        return str.substring(lastSlashIndex + 1);
    }
    return "";
}

String optionsGet(std::vector<String> musiclist)
{
    String options;
    for (int i = 0; i < musiclist.size(); i++) {
        String url = musiclist[i];
        options += musicSubstring(url);
        options += "\n";
    }

    options.trim();
    return options;
}

void printPSRAM(void)
{
    Serial.println("\n\nprintPSRAM .....");
    Serial.printf("Deafult free size: %d\n", heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
    Serial.printf("PSRAM free size: %d\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    Serial.printf("Flash size: %d bytes\n", ESP.getFlashChipSize());
    Serial.printf("before init wifi : free_heap_size = %d\n\n", esp_get_free_heap_size());
}