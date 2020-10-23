#include "Utils.hpp"

#include "customlogger.hpp"
#include <unistd.h>
#include <chrono>

#define BASEPATH "/sdcard"
#define CUSTOMSONGS_FOLDER "BeatSaberSongs"

static ModInfo modInfo;

const Logger& getLogger() {
    static const Logger logger(modInfo, LoggerOptions(false, false));
    return logger;
}

using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace UnityEngine::Networking;
using namespace System::Collections::Generic;
using namespace System::Collections;
using namespace System::IO;
using namespace System::Threading;
using namespace Tasks;

std::string LawlessBase64 = R"(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)";
std::string LightshowBase64 = R"(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)";
std::string MissingBase64 = R"(iVBORw0KGgoAAAANSUhEUgAAAIAAAACACAYAAADDPmHLAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAALh8AAC4fAXjugV8AAALESURBVHhe7ds9bhNRFMXxjG0+FCSgCF0KVsAC2AElG2EvtOwBGoqICqWmQKwBUVAkUpAs29jvmbnoWkLCjp2ZaXzO/9f4vjiJZN2jO/PeJCcAAAAAAAAAAAAAAAAAAEBCk68W1q0st6q1rsbj8b1cWhjlq7RofMjlTqPRaBLfVxaL9/klefIT4JDG79K0spQl/QH7NH9DPQSyl4Ahmh/ay8HHLCXJpnuoAATlKSD5wYZs/oZqCCx2AdiNAJgjAOYIgDkCYI4AmCMA5giAOQJgjgCYIwDmCIA5AnCAWsrXLOXwNPAw95umWWYtRXICrFarV1kOQrX5gT8IOYDq3wIE7gHMEQBzBMAcATBHAMwRAHMEwBwBMEcA9lhfXT3JUpJsANalvMmyl9HZ2a8sJelOgFq/ZYVbyJ5xhyGeByg/BwjcA5gjAOYIgDkCYI4AmCMAt6i1/sxSlnQA2gbOs+yk3QFeZoljVFarT3EW0FX+GmnShxx1Oj1vTk+/5/LO1A+BAvcA5uQT3meUMwEgjwCYIwDmCIA5AmCOAOxQ5/O3WeKYLa6vX/w91ruj/HF58vvc0KWhDmcAweIS0PZ/muVB1rPZ8yzlWaQ8HDoFaik348nkaS7l2dwExkhfL5cvc7nLI6fm26q1PouJEEop8/Zlkm8BAAAAgDibg6CN2Ppl+R+X499/2RwEReNDLreK90spN7m0wMOgHVymgfyH7NL8DYcQSF8C+jQ/tJcD+X8Nk0543wAE9SkgOwGGaH6otb7OUpJsuocKQFCeAjbbQGxHAMwRAHMEwJxsAOps9i7LXtp7ycdZSuIcYA/OAY5Yu4f/nWUn64uLh1nKkk536DoFaik/xpPJeS5lyQcgdAmB+ujfsNgFRDPbDHzO5T4PXJpvq5TyJabCRnuv8CHfAgAAAAAAAAAAAAAAAAAAAAAAAI7Byckfg4Foou/M6IoAAAAASUVORK5CYII=)";
Sprite* LawlessSprite = nullptr;
Sprite* LightshowSprite = nullptr;
Sprite* MissingSprite = nullptr;

//We dont need everything in questui so just copy it here
Sprite* Base64ToSprite(std::string base64, int width, int height)
{
    Array<uint8_t>* bytes = System::Convert::FromBase64String(il2cpp_utils::createcsstr(base64));
    Texture2D* texture = Texture2D::New_ctor(width, height, TextureFormat::RGBA32, false, false);
    if(ImageConversion::LoadImage(texture, bytes, false))
        return Sprite::Create(texture, UnityEngine::Rect(0.0f, 0.0f, (float)width, (float)height), UnityEngine::Vector2(0.5f,0.5f), 1024.0f, 1u, SpriteMeshType::FullRect, UnityEngine::Vector4(0.0f, 0.0f, 0.0f, 0.0f), false);
    return nullptr;
}

static Array<BeatmapCharacteristicSO*>* CustomChars;

Il2CppString* baseProjectPath = nullptr;
CustomLevelLoader* customLevelLoader = nullptr;
CachedMediaAsyncLoader* _cachedMediaAsyncLoader = nullptr;
AlwaysOwnedContentContainerSO* _alwaysOwnedContentContainer = nullptr;
    
std::string GetCustomLevelHash(StandardLevelInfoSaveData* level, std::string customLevelPath)
{
    LOG_DEBUG("GetCustomLevelHash Start");
    std::string actualPath = customLevelPath + "/Info.dat";
    if (!fileexists(actualPath)) actualPath = customLevelPath + "/info.dat";
        
    std::string hash = "";

    LOG_DEBUG("GetCustomLevelHash Reading all bytes from %s", actualPath.c_str());

    std::vector<char> bytesAsChar = readbytes(actualPath);
    LOG_DEBUG("GetCustomLevelHash Starting reading beatmaps");
    for (int i = 0; i < level->get_difficultyBeatmapSets()->Length(); i++)
    {
        for (int j = 0; j < level->get_difficultyBeatmapSets()->values[i]->get_difficultyBeatmaps()->Length(); j++)
        {
            std::string diffFile = to_utf8(csstrtostr(level->get_difficultyBeatmapSets()->values[i]->get_difficultyBeatmaps()->values[j]->get_beatmapFilename()));
            if (!fileexists(customLevelPath + "/" + diffFile))
            {
                LOG_ERROR("GetCustomLevelHash File %s did not exist", (customLevelPath + "/" + diffFile).c_str());
                continue;
            } 

            std::vector<char> currentDiff = readbytes(customLevelPath + "/" + diffFile);
            bytesAsChar.insert(bytesAsChar.end(), currentDiff.begin(), currentDiff.end());
        }
    }
    Array<uint8_t>* bytes = Array<uint8_t>::NewLength(bytesAsChar.size());
    for(int i = 0;i<bytes->Length();i++){
        bytes->values[i] = bytesAsChar[i];
    }
    LOG_DEBUG("GetCustomLevelHash computing Hash, found %d bytes", bytes->Length());
    hash = to_utf8(csstrtostr(System::BitConverter::ToString(System::Security::Cryptography::SHA1::Create()->ComputeHash(bytes))));

    hash.erase(std::remove(hash.begin(), hash.end(), '-'), hash.end());
        
    LOG_DEBUG("GetCustomLevelHash Stop %s", hash.c_str());
    return hash;
}
int SetChars = 0;
BeatmapCharacteristicSO* RegisterCustomCharacteristic(Sprite* Icon, std::string CharacteristicName, std::string HintText, std::string SerializedName, std::string CompoundIdPartName, bool requires360Movement = false, bool containsRotationEvents = false, int sortingOrder = 99)
{
    BeatmapCharacteristicSO* newChar = ScriptableObject::CreateInstance<BeatmapCharacteristicSO*>();
    newChar->icon = Icon;
    newChar->descriptionLocalizationKey = il2cpp_utils::createcsstr(HintText);
    newChar->serializedName = il2cpp_utils::createcsstr(SerializedName);
    newChar->characteristicNameLocalizationKey = il2cpp_utils::createcsstr(CompoundIdPartName);
    newChar->requires360Movement = requires360Movement;
    newChar->containsRotationEvents = containsRotationEvents;
    newChar->sortingOrder = sortingOrder;
    
    CustomChars->values[SetChars] = newChar;
    SetChars++;
    return newChar;
}

bool CharsCreated = false;

void SetupCustomCharacteristics()
{
    if(CharsCreated) return;
    
    if(LawlessSprite == nullptr)
        LawlessSprite = Base64ToSprite(LawlessBase64, 128, 128);

    if(LightshowSprite == nullptr)
        LightshowSprite = Base64ToSprite(LightshowBase64, 128, 128);

    if(MissingSprite == nullptr)
        MissingSprite = Base64ToSprite(MissingBase64, 128, 128);
    RegisterCustomCharacteristic(MissingSprite, "Missing Characteristic", "Missing Characteristic", "MissingCharacteristic", "MissingCharacteristic", false, false, 1000);
    RegisterCustomCharacteristic(LightshowSprite, "Lightshow", "Lightshow", "Lightshow", "Lightshow", false, false, 100);
    RegisterCustomCharacteristic(LawlessSprite, "Lawless", "Lawless - Anything Goes", "Lawless", "Lawless", false, false, 101);
    CharsCreated = true;
}


StandardLevelInfoSaveData* LoadCustomLevelInfoSaveData(Il2CppString* customLevelPath)
{
    Il2CppString* path = Path::Combine(customLevelPath, il2cpp_utils::createcsstr("Info.dat"));
    if (!File::Exists(path))
        path = Path::Combine(customLevelPath, il2cpp_utils::createcsstr("info.dat"));
    if (File::Exists(path))
        return StandardLevelInfoSaveData::DeserializeFromJSONString(File::ReadAllText(path));
    return nullptr;
}

EnvironmentInfoSO* LoadEnvironmentInfo(Il2CppString* environmentName, bool allDirections)
{
    EnvironmentInfoSO* environmentInfoSO = customLevelLoader->environmentSceneInfoCollection->GetEnviromentInfoBySerializedName(environmentName);
    if (!environmentInfoSO)
        environmentInfoSO = (allDirections ? customLevelLoader->defaultAllDirectionsEnvironmentInfo : customLevelLoader->defaultEnvironmentInfo);
    LOG_DEBUG("LoadEnvironmentInfo: %p", environmentInfoSO);
    return environmentInfoSO;
}

CustomPreviewBeatmapLevel* LoadCustomPreviewBeatmapLevelAsync(Il2CppString* customLevelPath, StandardLevelInfoSaveData* standardLevelInfoSaveData)
{
    if(!standardLevelInfoSaveData) return nullptr;
    LOG_DEBUG("LoadCustomPreviewBeatmapLevelAsync StandardLevelInfoSaveData: ");
    Il2CppString* levelID = il2cpp_utils::createcsstr("custom_level_" + GetCustomLevelHash(standardLevelInfoSaveData, to_utf8(csstrtostr(customLevelPath))));
    _alwaysOwnedContentContainer->alwaysOwnedBeatmapLevelIds->Add(levelID);
    Il2CppString* songName = standardLevelInfoSaveData->songName;
    Il2CppString* songSubName = standardLevelInfoSaveData->songSubName;
    Il2CppString* songAuthorName = standardLevelInfoSaveData->songAuthorName;
    Il2CppString* levelAuthorName = standardLevelInfoSaveData->levelAuthorName;
    float beatsPerMinute = standardLevelInfoSaveData->beatsPerMinute;
    float songTimeOffset = standardLevelInfoSaveData->songTimeOffset;
    float shuffle = standardLevelInfoSaveData->shuffle;
    float shufflePeriod = standardLevelInfoSaveData->shufflePeriod;
    float previewStartTime = standardLevelInfoSaveData->previewStartTime;
    float previewDuration = standardLevelInfoSaveData->previewDuration;
    LOG_DEBUG("levelID: %s", to_utf8(csstrtostr(levelID)).c_str());
    LOG_DEBUG("songName: %s", to_utf8(csstrtostr(songName)).c_str());
    LOG_DEBUG("songSubName: %s", to_utf8(csstrtostr(songSubName)).c_str());
    LOG_DEBUG("songAuthorName: %s", to_utf8(csstrtostr(songAuthorName)).c_str());
    LOG_DEBUG("levelAuthorName: %s", to_utf8(csstrtostr(levelAuthorName)).c_str());
    LOG_DEBUG("beatsPerMinute: %f", beatsPerMinute);
    LOG_DEBUG("songTimeOffset: %f", songTimeOffset);
    LOG_DEBUG("shuffle: %f", shuffle);
    LOG_DEBUG("shufflePeriod: %f", shufflePeriod);
    LOG_DEBUG("previewStartTime: %f", previewStartTime);
    LOG_DEBUG("previewDuration: %f", previewDuration);

    EnvironmentInfoSO* environmentInfo = LoadEnvironmentInfo(standardLevelInfoSaveData->environmentName, false);
    EnvironmentInfoSO* allDirectionsEnvironmentInfo = LoadEnvironmentInfo(standardLevelInfoSaveData->allDirectionsEnvironmentName, true);
    List<PreviewDifficultyBeatmapSet*>* list = List<PreviewDifficultyBeatmapSet*>::New_ctor();
    Array<StandardLevelInfoSaveData::DifficultyBeatmapSet*>* difficultyBeatmapSets = standardLevelInfoSaveData->difficultyBeatmapSets;
    for (int i = 0;i<difficultyBeatmapSets->Length();i++)
    {
        StandardLevelInfoSaveData::DifficultyBeatmapSet* difficultyBeatmapSet = difficultyBeatmapSets->values[i];
        BeatmapCharacteristicSO* beatmapCharacteristicBySerializedName = customLevelLoader->beatmapCharacteristicCollection->GetBeatmapCharacteristicBySerializedName(difficultyBeatmapSet->beatmapCharacteristicName);
        LOG_DEBUG("beatmapCharacteristicBySerializedName: %p", beatmapCharacteristicBySerializedName);
        LOG_DEBUG("beatmapCharacteristicBySerializedName: %s", to_utf8(csstrtostr(difficultyBeatmapSet->beatmapCharacteristicName)).c_str());
        if (beatmapCharacteristicBySerializedName)
        {
            Array<BeatmapDifficulty>* array = Array<BeatmapDifficulty>::NewLength(difficultyBeatmapSet->difficultyBeatmaps->Length());
            for (int j = 0; j < difficultyBeatmapSet->difficultyBeatmaps->Length(); j++)
            {
                BeatmapDifficulty beatmapDifficulty;
                BeatmapDifficultySerializedMethods::BeatmapDifficultyFromSerializedName(difficultyBeatmapSet->difficultyBeatmaps->values[j]->difficulty, beatmapDifficulty);
                array->values[j] = beatmapDifficulty;
            }
            list->Add(PreviewDifficultyBeatmapSet::New_ctor(beatmapCharacteristicBySerializedName, array));
        }
    }
    return CustomPreviewBeatmapLevel::New_ctor(customLevelLoader->defaultPackCover, standardLevelInfoSaveData, customLevelPath, reinterpret_cast<IAudioClipAsyncLoader*>(_cachedMediaAsyncLoader), reinterpret_cast<ISpriteAsyncLoader*>(_cachedMediaAsyncLoader), levelID, songName, songSubName, songAuthorName, levelAuthorName, beatsPerMinute, songTimeOffset, shuffle, shufflePeriod, previewStartTime, previewDuration, environmentInfo, allDirectionsEnvironmentInfo, list->ToArray());
}

Array<CustomPreviewBeatmapLevel*>* LoadCustomPreviewBeatmapLevelsAsync(Il2CppString* customLevelPackPath) {
    List<CustomPreviewBeatmapLevel*>* customPreviewBeatmapLevels = List<CustomPreviewBeatmapLevel*>::New_ctor();
    Array<Il2CppString*>* directories = Directory::GetDirectories(customLevelPackPath);
    if(directories->Length() < 1)
        return customPreviewBeatmapLevels->ToArray();
    for (int i = 0;i<directories->Length(); i++)
    {
        Il2CppString* customLevelPath = directories->values[i];
        CustomPreviewBeatmapLevel* customPreviewBeatmapLevel = LoadCustomPreviewBeatmapLevelAsync(customLevelPath, LoadCustomLevelInfoSaveData(customLevelPath));
        LOG_DEBUG("LoadCustomPreviewBeatmapLevelsAsync customPreviewBeatmapLevel %p", customPreviewBeatmapLevel);
        if (customPreviewBeatmapLevel && customPreviewBeatmapLevel->get_previewDifficultyBeatmapSets()->Length() != 0)
        {
            customPreviewBeatmapLevels->Add(customPreviewBeatmapLevel);
        }
    }
    return customPreviewBeatmapLevels->ToArray();
}

CustomBeatmapLevelCollection* LoadCustomBeatmapLevelCollectionAsync(Il2CppString* customLevelPackPath) {
    return CustomBeatmapLevelCollection::New_ctor(LoadCustomPreviewBeatmapLevelsAsync(customLevelPackPath));
}

CustomBeatmapLevelPack* LoadCustomBeatmapLevelPackAsync(Il2CppString* customLevelPackPath, Il2CppString* packName) {
    CustomBeatmapLevelCollection* customBeatmapLevelCollection = LoadCustomBeatmapLevelCollectionAsync(customLevelPackPath);
    if (customBeatmapLevelCollection->get_beatmapLevels()->Length() == 0)
        return nullptr;
    return CustomBeatmapLevelPack::New_ctor(il2cpp_utils::createcsstr("custom_levelpack_" + to_utf8(csstrtostr(customLevelPackPath))), packName, packName, customLevelLoader->defaultPackCover, customBeatmapLevelCollection);
}

bool CheckPathExists(Il2CppString* path) {
    return Directory::Exists(path);
}

struct CustomPackFolderInfo
{
    Il2CppString* folderName;
    Il2CppString* packName;
};

Array<IBeatmapLevelPack*>* LoadCustomPreviewBeatmapLevelPacksAsync(std::vector<CustomPackFolderInfo> customPackFolderInfos) {
    int numberOfPacks = customPackFolderInfos.size();
    List<IBeatmapLevelPack*>* customBeatmapLevelPacks = List<IBeatmapLevelPack*>::New_ctor(numberOfPacks);
    for (int i = 0; i < numberOfPacks; i++)
    {
        Il2CppString* customLevelPackPath = Path::Combine(baseProjectPath, customPackFolderInfos[i].folderName);
        if (CheckPathExists(customLevelPackPath))
        {
            CustomBeatmapLevelPack* customBeatmapLevelPack = LoadCustomBeatmapLevelPackAsync(customLevelPackPath, customPackFolderInfos[i].packName);
            if (customBeatmapLevelPack && customBeatmapLevelPack->get_beatmapLevelCollection()->get_beatmapLevels()->Length() != 0)
            {
                _alwaysOwnedContentContainer->alwaysOwnedPacksIds->Add(customBeatmapLevelPack->packID);
                customBeatmapLevelPacks->Add(reinterpret_cast<IBeatmapLevelPack*>(customBeatmapLevelPack));
            }
        }
    }
    return customBeatmapLevelPacks->ToArray();
}

std::vector<CustomPackFolderInfo> GetSubFoldersInfosAsync(std::string rootPath)
{
    std::vector<CustomPackFolderInfo> subDirFolderInfo;
    Il2CppString* rootPathCS = Path::Combine(baseProjectPath, il2cpp_utils::createcsstr(rootPath));
    Array<Il2CppString*>* directories = Directory::GetDirectories(rootPathCS);
    for (int i = 0;i<directories->Length(); i++)
    {
        Il2CppString* path = directories->values[i];
        subDirFolderInfo.push_back(CustomPackFolderInfo{Path::GetFullPath(path), DirectoryInfo::New_ctor(path)->get_Name()});
    }
    return subDirFolderInfo;
}

BeatmapData* LoadBeatmapDataBeatmapData(Il2CppString* customLevelPath, Il2CppString* difficultyFileName, StandardLevelInfoSaveData* standardLevelInfoSaveData)
{
    LOG_DEBUG("LoadBeatmapDataBeatmapData Start");
    BeatmapData* data = nullptr;
    Il2CppString* path = Path::Combine(customLevelPath, difficultyFileName);
    if (File::Exists(path))
    {
        Il2CppString* json = File::ReadAllText(path);
        data = BeatmapDataLoader::New_ctor()->GetBeatmapDataFromJson(json, standardLevelInfoSaveData->beatsPerMinute, standardLevelInfoSaveData->shuffle, standardLevelInfoSaveData->shufflePeriod);
    }
    LOG_DEBUG("LoadBeatmapDataBeatmapData Stop");
    return data;
}

CustomDifficultyBeatmap* LoadDifficultyBeatmapAsync(Il2CppString* customLevelPath, CustomBeatmapLevel* parentCustomBeatmapLevel, CustomDifficultyBeatmapSet* parentDifficultyBeatmapSet, StandardLevelInfoSaveData* standardLevelInfoSaveData, StandardLevelInfoSaveData::DifficultyBeatmap* difficultyBeatmapSaveData)
{
    LOG_DEBUG("LoadDifficultyBeatmapAsync Start");
    BeatmapData* beatmapData = LoadBeatmapDataBeatmapData(customLevelPath, difficultyBeatmapSaveData->beatmapFilename, standardLevelInfoSaveData);
    BeatmapDifficulty difficulty;
    BeatmapDifficultySerializedMethods::BeatmapDifficultyFromSerializedName(difficultyBeatmapSaveData->difficulty, difficulty);
    LOG_DEBUG("LoadDifficultyBeatmapAsync Stop");
    return CustomDifficultyBeatmap::New_ctor(reinterpret_cast<IBeatmapLevel*>(parentCustomBeatmapLevel), reinterpret_cast<IDifficultyBeatmapSet*>(parentDifficultyBeatmapSet), difficulty, difficultyBeatmapSaveData->difficultyRank, difficultyBeatmapSaveData->noteJumpMovementSpeed, difficultyBeatmapSaveData->noteJumpStartBeatOffset, beatmapData);
}

IDifficultyBeatmapSet* LoadDifficultyBeatmapSetAsync(Il2CppString* customLevelPath, CustomBeatmapLevel* customBeatmapLevel, StandardLevelInfoSaveData* standardLevelInfoSaveData, StandardLevelInfoSaveData::DifficultyBeatmapSet* difficultyBeatmapSetSaveData)
{
    LOG_DEBUG("LoadDifficultyBeatmapSetAsync Start");
    BeatmapCharacteristicSO* beatmapCharacteristicBySerializedName = customLevelLoader->beatmapCharacteristicCollection->GetBeatmapCharacteristicBySerializedName(difficultyBeatmapSetSaveData->beatmapCharacteristicName);
    Array<CustomDifficultyBeatmap*>* difficultyBeatmaps = Array<CustomDifficultyBeatmap*>::NewLength(difficultyBeatmapSetSaveData->difficultyBeatmaps->Length());
    CustomDifficultyBeatmapSet* difficultyBeatmapSet = CustomDifficultyBeatmapSet::New_ctor(beatmapCharacteristicBySerializedName);
    for (int i = 0; i < difficultyBeatmapSetSaveData->difficultyBeatmaps->Length(); i++)
    {
        CustomDifficultyBeatmap* customDifficultyBeatmap = LoadDifficultyBeatmapAsync(customLevelPath, customBeatmapLevel, difficultyBeatmapSet, standardLevelInfoSaveData, difficultyBeatmapSetSaveData->difficultyBeatmaps->values[i]);
        difficultyBeatmaps->values[i] = customDifficultyBeatmap;
    }
    difficultyBeatmapSet->SetCustomDifficultyBeatmaps(difficultyBeatmaps);
    LOG_DEBUG("LoadDifficultyBeatmapSetAsync Stop");
    return reinterpret_cast<IDifficultyBeatmapSet*>(difficultyBeatmapSet);
}

Array<IDifficultyBeatmapSet*>* LoadDifficultyBeatmapSetsAsync(Il2CppString* customLevelPath, CustomBeatmapLevel* customBeatmapLevel, StandardLevelInfoSaveData* standardLevelInfoSaveData)
{
    LOG_DEBUG("LoadDifficultyBeatmapSetsAsync Start");
    Array<IDifficultyBeatmapSet*>* difficultyBeatmapSets = Array<IDifficultyBeatmapSet*>::NewLength(standardLevelInfoSaveData->difficultyBeatmapSets->Length());
    for (int i = 0; i < difficultyBeatmapSets->Length(); i++)
    {
        IDifficultyBeatmapSet* difficultyBeatmapSet = LoadDifficultyBeatmapSetAsync(customLevelPath, customBeatmapLevel, standardLevelInfoSaveData, standardLevelInfoSaveData->difficultyBeatmapSets->values[i]);
        difficultyBeatmapSets->values[i] = difficultyBeatmapSet;
    }
    LOG_DEBUG("LoadDifficultyBeatmapSetsAsync Stop");
    return difficultyBeatmapSets;
}

AudioClip* GetPreviewAudioClipAsync(CustomPreviewBeatmapLevel* customPreviewBeatmapLevel) {
    LOG_DEBUG("GetPreviewAudioClipAsync Start %p", customPreviewBeatmapLevel->previewAudioClip);
    if (!customPreviewBeatmapLevel->previewAudioClip && !System::String::IsNullOrEmpty(customPreviewBeatmapLevel->standardLevelInfoSaveData->songFilename))
    {
        Il2CppString* path = Path::Combine(customPreviewBeatmapLevel->customLevelPath, customPreviewBeatmapLevel->standardLevelInfoSaveData->songFilename);
        AudioType audioType = (to_utf8(csstrtostr(Path::GetExtension(path)->ToLower())) == ".wav") ? AudioType::WAV : AudioType::OGGVORBIS;
        UnityWebRequest* www = UnityWebRequestMultimedia::GetAudioClip(FileHelpers::GetEscapedURLForFilePath(path), audioType);
        ((DownloadHandlerAudioClip*)www->m_DownloadHandler)->set_streamAudio(true);
        UnityWebRequestAsyncOperation* request = www->SendWebRequest();
        while (!request->get_isDone())
        {
            LOG_DEBUG("GetPreviewAudioClipAsync Delay");
            usleep(100* 1000);
        }
        LOG_DEBUG("GetPreviewAudioClipAsync ErrorStatus %d %d", www->get_isHttpError(), www->get_isNetworkError());
        if(!www->get_isHttpError() && !www->get_isNetworkError())
            customPreviewBeatmapLevel->previewAudioClip = DownloadHandlerAudioClip::GetContent(www);
    }
    LOG_DEBUG("GetPreviewAudioClipAsync Stop %p", customPreviewBeatmapLevel->previewAudioClip);
    return customPreviewBeatmapLevel->previewAudioClip;
}

BeatmapLevelData* LoadBeatmapLevelDataAsync(Il2CppString* customLevelPath, CustomBeatmapLevel* customBeatmapLevel, StandardLevelInfoSaveData* standardLevelInfoSaveData, CancellationToken cancellationToken)
{
    LOG_DEBUG("LoadBeatmapLevelDataAsync Start");
    Array<IDifficultyBeatmapSet*>* difficultyBeatmapSets = LoadDifficultyBeatmapSetsAsync(customLevelPath, customBeatmapLevel, standardLevelInfoSaveData);
    AudioClip* audioClip = GetPreviewAudioClipAsync(reinterpret_cast<CustomPreviewBeatmapLevel*>(customBeatmapLevel));
    if (!audioClip)
        return nullptr;
    LOG_DEBUG("LoadBeatmapLevelDataAsync Stop");
    return BeatmapLevelData::New_ctor(audioClip, difficultyBeatmapSets);
}

CustomBeatmapLevel* LoadCustomBeatmapLevelAsync(CustomPreviewBeatmapLevel* customPreviewBeatmapLevel, CancellationToken cancellationToken)
{
    LOG_DEBUG("LoadCustomBeatmapLevelAsync Start");
    StandardLevelInfoSaveData* standardLevelInfoSaveData = customPreviewBeatmapLevel->standardLevelInfoSaveData;
    Il2CppString* customLevelPath = customPreviewBeatmapLevel->customLevelPath;
    AudioClip* previewAudioClip = GetPreviewAudioClipAsync(customPreviewBeatmapLevel);
    CustomBeatmapLevel* customBeatmapLevel = CustomBeatmapLevel::New_ctor(customPreviewBeatmapLevel, previewAudioClip);
    BeatmapLevelData* beatmapLevelData = LoadBeatmapLevelDataAsync(customLevelPath, customBeatmapLevel, standardLevelInfoSaveData, cancellationToken);
    customBeatmapLevel->SetBeatmapLevelData(beatmapLevelData);
    LOG_DEBUG("LoadCustomBeatmapLevelAsync Stop");
    return customBeatmapLevel;
}

MAKE_HOOK_OFFSETLESS(BeatmapLevelsModel_ClearLoadedBeatmapLevelsCaches, void, BeatmapLevelsModel* self)
{
    LOG_DEBUG("BeatmapLevelsModel_ClearLoadedBeatmapLevelsCaches");
    if(_cachedMediaAsyncLoader)
        _cachedMediaAsyncLoader->ClearCache();
    BeatmapLevelsModel_ClearLoadedBeatmapLevelsCaches(self);
}

MAKE_HOOK_OFFSETLESS(BeatmapLevelsModel_ReloadCustomLevelPackCollectionAsync, Task_1<IBeatmapLevelPackCollection*>*, BeatmapLevelsModel* self, CancellationToken cancellationToken) {
    LOG_DEBUG("BeatmapLevelsModel_ReloadCustomLevelPackCollectionAsync Start");
    baseProjectPath = il2cpp_utils::createcsstr(BASEPATH);
    customLevelLoader = Resources::FindObjectsOfTypeAll<CustomLevelLoader*>()->values[0];
    _cachedMediaAsyncLoader = Resources::FindObjectsOfTypeAll<CachedMediaAsyncLoader*>()->values[0];
    _alwaysOwnedContentContainer = Resources::FindObjectsOfTypeAll<AlwaysOwnedContentContainerSO*>()->values[0];

    std::vector<CustomPackFolderInfo> folders = GetSubFoldersInfosAsync(CUSTOMSONGS_FOLDER);
    folders.insert(folders.begin(), CustomPackFolderInfo{il2cpp_utils::createcsstr(CUSTOMSONGS_FOLDER), il2cpp_utils::createcsstr("Custom Levels")});
    auto start = std::chrono::high_resolution_clock::now(); 
    Array<IBeatmapLevelPack*>* beatmapLevelPacks = LoadCustomPreviewBeatmapLevelPacksAsync(folders);
    std::chrono::microseconds duration = duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start); 
    LOG_INFO("BeatmapLevelsModel_ReloadCustomLevelPackCollectionAsync Loading time %d", duration);
    self->customLevelPackCollection = reinterpret_cast<IBeatmapLevelPackCollection*>(BeatmapLevelPackCollection::New_ctor(beatmapLevelPacks));
    self->UpdateLoadedPreviewLevels();
    LOG_DEBUG("BeatmapLevelsModel_ReloadCustomLevelPackCollectionAsync Stop");
    return Task_1<IBeatmapLevelPackCollection*>::New_ctor(self->customLevelPackCollection);
}

MAKE_HOOK_OFFSETLESS(BeatmapLevelsModel_GetBeatmapLevelAsync, Task_1<BeatmapLevelsModel::GetBeatmapLevelResult>*, BeatmapLevelsModel* self, Il2CppString* levelID, CancellationToken cancellationToken) {
    LOG_DEBUG("BeatmapLevelsModel_GetBeatmapLevelAsync Start %s", to_utf8(csstrtostr(levelID)).c_str());
    Task_1<BeatmapLevelsModel::GetBeatmapLevelResult>* result = BeatmapLevelsModel_GetBeatmapLevelAsync(self, levelID, cancellationToken);
    if(result->get_Result().isError) {
        IPreviewBeatmapLevel* previewBeatmapLevel = self->loadedPreviewBeatmapLevels->get_Item(levelID);
        if(previewBeatmapLevel) {
            LOG_DEBUG("BeatmapLevelsModel_GetBeatmapLevelAsync previewBeatmapLevel %p", previewBeatmapLevel);
            if (il2cpp_functions::class_is_subclass_of(classof(CustomPreviewBeatmapLevel*), il2cpp_functions::object_get_class(reinterpret_cast<Il2CppObject*>(previewBeatmapLevel)), true))
            {
                CustomBeatmapLevel* customBeatmapLevel = LoadCustomBeatmapLevelAsync(reinterpret_cast<CustomPreviewBeatmapLevel*>(previewBeatmapLevel), cancellationToken);
                LOG_DEBUG("BeatmapLevelsModel_GetBeatmapLevelAsync");
                if (!customBeatmapLevel || !customBeatmapLevel->get_beatmapLevelData()){
                    LOG_DEBUG("BeatmapLevelsModel_GetBeatmapLevelAsync Stop");
                    return Task_1<BeatmapLevelsModel::GetBeatmapLevelResult>::New_ctor(BeatmapLevelsModel::GetBeatmapLevelResult(true, nullptr));
                }
                self->loadedBeatmapLevels->PutToCache(levelID, reinterpret_cast<IBeatmapLevel*>(customBeatmapLevel));
                LOG_DEBUG("BeatmapLevelsModel_GetBeatmapLevelAsync Stop %p", customBeatmapLevel);
                return Task_1<BeatmapLevelsModel::GetBeatmapLevelResult>::New_ctor(BeatmapLevelsModel::GetBeatmapLevelResult(false, reinterpret_cast<IBeatmapLevel*>(customBeatmapLevel)));
            }
        }
    }
    LOG_DEBUG("BeatmapLevelsModel_GetBeatmapLevelAsync Stop");
    return result;
}

MAKE_HOOK_OFFSETLESS(BeatmapLevelsModel_UpdateAllLoadedBeatmapLevelPacks, void, BeatmapLevelsModel* self) {
    LOG_DEBUG("BeatmapLevelsModel_UpdateAllLoadedBeatmapLevelPacks Start");
    List<IBeatmapLevelPack*>* list = List<IBeatmapLevelPack*>::New_ctor();
    list->AddRange(reinterpret_cast<IEnumerable_1<IBeatmapLevelPack*>*>(self->ostAndExtrasPackCollection->get_beatmapLevelPacks()));
    list->AddRange(reinterpret_cast<IEnumerable_1<IBeatmapLevelPack*>*>(self->dlcLevelPackCollectionContainer->beatmapLevelPackCollection->get_beatmapLevelPacks()));
    if (self->customLevelPackCollection)
    {
        list->AddRange(reinterpret_cast<IEnumerable_1<IBeatmapLevelPack*>*>(self->customLevelPackCollection->get_beatmapLevelPacks()));
    }
    self->allLoadedBeatmapLevelPackCollection = reinterpret_cast<IBeatmapLevelPackCollection*>(BeatmapLevelPackCollection::New_ctor(list->ToArray()));
    LOG_DEBUG("BeatmapLevelsModel_UpdateAllLoadedBeatmapLevelPacks Stop");
}

MAKE_HOOK_OFFSETLESS(LevelFilteringNavigationController_Setup, void, LevelFilteringNavigationController* self, SongPackMask* songPackMask, IBeatmapLevelPack* levelPackToBeSelectedAfterPresent, SelectLevelCategoryViewController::LevelCategory startLevelCategory, bool hidePacksIfOneOrNone, bool enableCustomLevels) {
    SetupCustomCharacteristics();
    LOG_DEBUG("LevelFilteringNavigationController_Setup");
    LevelFilteringNavigationController_Setup(self, songPackMask, levelPackToBeSelectedAfterPresent, startLevelCategory, hidePacksIfOneOrNone, enableCustomLevels);
    self->enableCustomLevels = true;
}

MAKE_HOOK_OFFSETLESS(FileHelpers_GetEscapedURLForFilePath, Il2CppString*, Il2CppString* filePath)
{
    LOG_DEBUG("FileHelpers_GetEscapedURLForFilePath");
    return il2cpp_utils::createcsstr("file:///" + to_utf8(csstrtostr(filePath)));
}


MAKE_HOOK_OFFSETLESS(BeatmapCharacteristicCollectionSO_GetBeatmapCharacteristicBySerializedName, BeatmapCharacteristicSO*, BeatmapCharacteristicCollectionSO* self, Il2CppString* serializedName)
{
    
    BeatmapCharacteristicSO* Result = BeatmapCharacteristicCollectionSO_GetBeatmapCharacteristicBySerializedName(self, serializedName);
    std::string _serializedName = to_utf8(csstrtostr(serializedName));

    if(Result == nullptr)
    {
        if(Any(CustomChars, [_serializedName](BeatmapCharacteristicSO* x) { return to_utf8(csstrtostr(x->serializedName)) == _serializedName;})) {
            getLogger().info("Any Found");
            Result = First(CustomChars, [_serializedName](BeatmapCharacteristicSO* x) { return to_utf8(csstrtostr(x->serializedName)) == _serializedName;});
        }else{
            getLogger().info("Any NOTFound");
            Result = First(CustomChars, [_serializedName](BeatmapCharacteristicSO* x) { return to_utf8(csstrtostr(x->serializedName)) == "MissingCharacteristic";});
        }
    }

    return Result;
}

extern "C" void setup(ModInfo& info) {
    modInfo.id = "SongLoader";
    modInfo.version = VERSION;
    info = modInfo;
}

extern "C" void load() {
    LOG_INFO("Starting SongLoader installation...");
    il2cpp_functions::Init();
    INSTALL_HOOK_OFFSETLESS(BeatmapLevelsModel_ClearLoadedBeatmapLevelsCaches, il2cpp_utils::FindMethodUnsafe("", "BeatmapLevelsModel", "ClearLoadedBeatmapLevelsCaches", 0));
    INSTALL_HOOK_OFFSETLESS(BeatmapLevelsModel_ReloadCustomLevelPackCollectionAsync, il2cpp_utils::FindMethodUnsafe("", "BeatmapLevelsModel", "ReloadCustomLevelPackCollectionAsync", 1));
    INSTALL_HOOK_OFFSETLESS(BeatmapLevelsModel_GetBeatmapLevelAsync, il2cpp_utils::FindMethodUnsafe("", "BeatmapLevelsModel", "GetBeatmapLevelAsync", 2));
    INSTALL_HOOK_OFFSETLESS(BeatmapLevelsModel_UpdateAllLoadedBeatmapLevelPacks, il2cpp_utils::FindMethodUnsafe("", "BeatmapLevelsModel", "UpdateAllLoadedBeatmapLevelPacks", 0));
    INSTALL_HOOK_OFFSETLESS(LevelFilteringNavigationController_Setup, il2cpp_utils::FindMethodUnsafe("", "LevelFilteringNavigationController", "Setup", 5));
    INSTALL_HOOK_OFFSETLESS(FileHelpers_GetEscapedURLForFilePath, il2cpp_utils::FindMethodUnsafe("", "FileHelpers", "GetEscapedURLForFilePath", 1));
    INSTALL_HOOK_OFFSETLESS(BeatmapCharacteristicCollectionSO_GetBeatmapCharacteristicBySerializedName, il2cpp_utils::FindMethodUnsafe("", "BeatmapCharacteristicCollectionSO", "GetBeatmapCharacteristicBySerializedName", 1));
    LOG_INFO("Successfully installed SongLoader!");
    CustomChars = Array<BeatmapCharacteristicSO*>::NewLength(3);
}