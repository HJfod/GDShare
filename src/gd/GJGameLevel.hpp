#pragma once
#ifndef GJGAMELEVEL_HPP
#define GJGAMELEVEL_HPP

#include <cocos2d.h>

enum GJLevelType { Local = 1, Editor = 2, Saved = 3 };

class GJGameLevel : public cocos2d::CCNode
{
public:
  cocos2d::CCDictionary *lastBuildSave;

  int levelID_rand; // levelID + seed = levelID_rand
  int levelID_seed;
  int levelID;

  std::string levelName;
  std::string levelDesc;
  std::string levelString;
  std::string userName;
  std::string recordString;

  std::string uploadDate;
  std::string updateDate;

  int userID_rand;
  int userID_seed;
  int userID;

  int accountID_rand;
  int accountID_seed;
  int accountID;

  int difficulty;
  int audioTrack;
  int songID;

  int levelRev;

  bool unlisted;

  int objectCount_rand;
  int objectCount_seed;
  int objectCount;
  int averageDifficulty;
  int ratings;
  int ratingsSum;

  int downloads;

  bool isEditable;
  bool gauntletLevel;
  bool gauntletLevel2;

  int workingTime;
  int workingTime2;

  bool lowDetailMode;
  bool lowDetailModeToggled;

  int isVerified_rand;
  int isVerified_seed;
  bool isVerified;
  bool isUploaded;
  bool hasBeenModified;

  int levelVersion;
  int gameVersion;

  int attempts_rand;
  int attempts_seed;
  int attempts;

  int jumps_rand;
  int jumps_seed;
  int jumps;

  int clicks_rand;
  int clicks_seed;
  int clicks;

  int attemptTime_rand;
  int attemptTime_seed;
  int attemptTime;

  int chk;

  bool isChkValid;
  bool isCompletionLegitimate;

  int normalPercent; // yes, it is out of order
  int normalPercent_seed;
  int normalPercent_rand;

  int orbCompletion_rand;
  int orbCompletion_seed;
  int orbCompletion;

  int newNormalPercent2_rand;
  int newNormalPercent2_seed;
  int newNormalPercent2;

  int practicePercent;

  int likes;
  int dislikes;
  int levelLength;
  int featured;

  bool isEpic;
  bool levelFavorited;
  int levelFolder;

  int dailyID_rand;
  int dailyID_seed;
  int dailyID;

  int demon_rand;
  int demon_seed;
  int demon;
  int demonDifficulty;
  int stars_rand;
  int stars_seed;
  int stars;

  bool autoLevel;
  int coins;
  int coinsVerified_rand;
  int coinsVerified_seed;
  int coinsVerified;

  int password_rand;
  int password_seed;

  int originalLevel_rand;
  int originalLevel_seed;
  int originalLevel;

  bool twoPlayerMode;

  int failedPasswordAttempts;

  int firstCoinVerified_rand;
  int firstCoinVerified_seed;
  int firstCoinVerified;

  int secondCoinVerified_rand;
  int secondCoinVerified_seed;
  int secondCoinVerified;

  int thirdCoinVerified_rand;
  int thirdCoinVerified_seed;
  int thirdCoinVerified;

  int starsRequested;

  bool showedSongWarning;
  int starRatings;
  int starRatingsSum;
  int maxStarRatings;
  int minStarRatings;
  int demonVotes;
  int rateStars;
  int rateFeature;

  std::string rateUser;

  bool dontSave;
  bool levelDownloaded;

  int requiredCoins;
  bool isUnlocked;

  cocos2d::CCPoint lastCameraPos;

  float lastEditorZoom;
  int lastBuildTab;
  int lastBuildPage;
  int lastBuildGroupID;

  GJLevelType levelType;

  int M_ID;
  std::string tempName;
  std::string capacityString;

  bool highObjectsEnabled;
  std::string personalBests;

  // this function is inlined on pc builds
  static GJGameLevel *create() {
    auto pRet = new GJGameLevel();

    if (pRet && pRet->init()) {
      pRet->autorelease();
      return pRet;
    } else {
      delete pRet;
      pRet = nullptr;
      return nullptr;
    }
  }
};

#endif
