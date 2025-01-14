#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <unordered_map>
#include "CbrData.h"
#include "Metadata.h"


#define rWallDist 1850000
#define lWallDist -1850000
#define maxWallDist 3700000
//(1850000/2)
#define maxXDist 370000
#define maxXDistScreen 925000
//(800000/2)
#define maxYDist 400000 

#define maxProration 10000
#define maxComboTime 200

#define costXDist 1
#define costYDist 1
#define costWallDist 0.3
#define costWallDistCombo 0.5
#define costAiState 0.5
#define costEnemyState 0.2
#define costlastAiState 0.05
#define costlastEnemyState 0.05
#define costAiNeutral 1
#define costEnemyNeutral 0.2
#define costAiAir 1
#define costEnemyAir 0.2
#define costAiWakeup 1
#define costEnemyWakeup 0.2
#define costAiBlocking 1
#define costEnemyBlocking 1
#define costAiHit 1
#define costEnemyHit 1
#define costAiAttack 1
#define costEnemyAttack 0.2
#define costAiHitThisFrame 0.1
#define costEnemyHitThisFrame 0.1
#define costAiBlockThisFrame 0.1
#define costEnemyBlockhisFrame 0.1
#define costAiCrouching 0.1
#define costEnemyCrouching 0.1
#define costAiProration 0.05
#define costEnemyProration 0.05
#define costAiStarterRating 0.05
#define costEnemyStarterRating 0.05
#define costAiComboTime 0.05
#define costEnemyComboTime 0.05
#define costAiOverdriveState 1
#define costEnemyOverdriveState 0.01
#define costMatchState 100


//projectilecosts
#define costHelperType 0.2
#define costHelperPosX 0.1
#define costHelperPosY 0.1
#define costHelperState 0.05
#define costHelperHit 0.05
#define costHelperAttack 0.1
#define costHelperOrder 0.05
#define costHelperSum (costHelperType + costHelperPosX + costHelperPosY + costHelperState + costHelperHit + costHelperAttack + costHelperOrder + costHelperOrder)

//bulletSpecificData
#define costBuHeat 0.1

//litchiSpecific
#define costLcStaff 1
#define costLcStaffActive 0.4

//rachelSpecific
#define costWind 0.1
#define costLowWind 1
#define rachelWindMax 40000
#define rachelWindMin 10000

#define nextBestMulti 1.3
#define nextBestAdd 0.01

#define specialButton 512
#define tauntButton 256
#define DButton 128
#define CButton 64
#define BButton 32
#define AButton 16
/*
var cbrParameters = CBRParameters{
    gameStateMaxChange:       5,    //every frame during gameplay if we check the current gamestate against the currently running case and it got this much worse, we check for a new case
    betterCaseThreshold : 0.30, //when the next case in a sequence would be played, if another case exists that is this much better, switch to that case
    topSelectionThreshold : 0.00, //when a best case is found to avoid always selecting the same case in similar situation choose a random worse. This parameter determines how much worse at most.
    maxXPositionComparison : 300,  //If X position comparisons are more than this amount shifted, similarity for them is 1
    maxYPositionComparison : 200,  //If Y position comparisons are more than this amount shifted, similarity for them is 1
    maxVelocityComparison : 10,   //The max difference in velocity before the comparison function hits max dissimilarity
    curGamestateQueLength : 12,   // how many frames in a row are stored to compare against in the comparison function. Stored in aiData.curGamestate
    maxInputBufferDifference : 30,   //How many frames of input buffering can be off before reaching max dissimilarity
    maxHitstunDifference : 10,   //how many frames of difference of beeing in hitstun is allowed till max dissimilarity
    maxBlockstunDifference : 10,   //how many frames of difference of beeing in blockstun is allowed till max dissimilarity
    maxAttackStateDiff : 20,   //how many frames of beeing in an attack state are allowed till max dissimilarity
    nearWallDist : 0.13, //percent of how close compared to current stage size a character has to be, to be considered near the wall
    repetitionFrames : 60,   //amount of frames after which a case was used where it will be taxed for beeing used again. Multiplied the more a case is used.
    comboLength : 20,

    cps : comparisonParameters{
        //parameters that determine how strongly different comparison functions are evaluated
        XRelativePosition:    1.0,
        YRelativePosition : 1.0,
        xVelocityComparison : 0.25,
        yVelocityComparison : 0.25,
        inputBufferDirection : 1.0,
        inputBufferButton : 1.0,
        airborneState : 1.0,
        lyingDownState : 1.0,
        hitState : 1.0,
        blockState : 1.0,
        attackState : 1.0,
        nearWall : 0.3,
        moveID : 0.5,
        pressureMoveID : 0.8,
        getHit : 1.0,
        didHit : 1.0,
        frameAdv : 0.3,
        frameAdvInitiator : 0.1,
        comboSimilarity : 1.0,

        objectOrder : 0.3,
        caseReuse : 0.5,
        roundState : 100.0,

        helperRelativePositionX : 0.5,
        helperRelativePositionY : 0.5,
        helperXVelocityComparison : 0.25,
        helperYVelocityComparison : 0.25,

        enemyXVelocityComparison : 0.25,
        enemyYVelocityComparison : 0.25,
        enemyAirborneState : 1.0,
        enemyLyingDownState : 1.0,
        enemyHitState : 1.0,
        enemyBlockState : 1.0,
        enemyAttackState : 1.0,
        enemyMoveID : 0.5,
        enemyPressureMoveID : 0.8,

        enemyHelperRelativePositionX : 1.0,
        enemyHelperRelativePositionY : 1.0,
        enemyHelperXVelocityComparison : 0.25,
        enemyHelperYVelocityComparison : 0.25,
    },
}*/

  



CbrData::CbrData()
{
}

CbrData::CbrData(std::string n, std::string p1, int charId) :playerName(n), characterName(p1)
{
    characterIndex = charId;
    replayFiles = {};
    activeReplay = -1;
    activeCase = -1;
    activeFrame = -1;
    enabled = true;
}

void CbrData::setEnabled(bool b) {
    enabled = b;
}
void CbrData::setPlayerName(std::string name) {
    playerName = name;
}
bool CbrData::getEnabled() {
    return enabled;
}
int CbrData::getCharacterIndex() {
    return characterIndex;
}

void CbrData::AddReplay(CbrReplayFile f)
{
    replayFiles.push_back(f);
}
void CbrData::deleteReplays(int start, int end) {
    replayFiles.erase(std::next(replayFiles.begin(), start), std::next(replayFiles.begin(), end+1)); 
}
CbrReplayFile* CbrData::getLastReplay() {
    return  &replayFiles[replayFiles.size() - 1];
}
void CbrData::setCharName(std::string s) {
    characterName = s;
}
int CbrData::CBRcomputeNextAction(Metadata* curGamestate) {
    float bestComparison = 9999999;
    int bestReplay = -1;
    int bestCase = -1;
    int bestFrame = -1;
    int bestCompBufferCase = -2;
    int bestCompBufferReplay = -2;

    float bufComparison = 9999999;
    activeFrame++;
    //if we are searching for a case for the first time or the case is done playing or the player is hit/lands a hit search for a new case
    if (switchCaseCheck(curGamestate)){
        auto replayFileSize = replayFiles.size();
        for (std::size_t i = 0; i < replayFileSize; ++i) {
            auto caseBaseSize = replayFiles[i].getCaseBase()->size();
            for (std::size_t j = 0; j < caseBaseSize; ++j) {
                if (caseValidityCheck(curGamestate, i,j)) {
                    auto caseGamestate = replayFiles[i].getCase(j)->getMetadata();
                    bufComparison = comparisonFunction(curGamestate, caseGamestate, replayFiles[i]);
                    if (bufComparison <= bestComparison) {
                        //only doing helperComp if this even has a chance to be the best because it is costly
                        bufComparison += HelperCompMatch(curGamestate, caseGamestate);
                        if (bufComparison <= bestComparison) {
                            if (bufComparison == bestComparison && bestCompBufferCase == (j - 1) && bestCompBufferReplay == i) {
                                bestCompBufferCase = j;
                            }
                            else {
                                bestComparison = bufComparison;
                                bestReplay = i;
                                bestCase = j;
                                bestFrame = replayFiles[i].getCase(j)->getStartingIndex();
                                bestCompBufferCase = j;
                                bestCompBufferReplay = i;
                            }
                        }

                    }
                }
            }
        }

        if (activeReplay != -1 && activeCase +1 <= replayFiles[activeReplay].getCaseBaseLength() -1 ){
            float bufComparison = 9999999;
            float bufComparison2 = 9999999;
            bufComparison = comparisonFunction(curGamestate, replayFiles[activeReplay].getCase(activeCase + 1)->getMetadata(), replayFiles[activeReplay]);
            if (bestReplay == -1 && bestCase == -1) {
                if (bufComparison < bestComparison) {
                    bufComparison2 = 9999999;
                }
            }
            else {
                bufComparison2 = comparisonFunction(curGamestate, replayFiles[bestReplay].getCase(bestCase)->getMetadata(), replayFiles[activeReplay]);
                debugPrint(replayFiles[activeReplay].getCase(activeCase + 1), replayFiles[bestReplay].getCase(bestCase), curGamestate, bufComparison, bufComparison2, activeCase + 1, bestCase, activeReplay, bestReplay);
            }

            if (nextCaseValidityCheck(curGamestate, activeReplay, activeCase + 1) && (bufComparison <= (bestComparison * nextBestMulti + nextBestAdd))) {
                bestReplay = activeReplay;
                bestComparison = bufComparison;
                bestCase = activeCase+1;
                bestFrame = replayFiles[bestReplay].getCase(bestCase)->getStartingIndex();
            }

        }

        activeReplay = bestReplay;
        activeCase = bestCase;
        activeFrame = bestFrame;
        
        if (activeFrame != -1) {
            invertCurCase = replayFiles[bestReplay].getCase(bestCase)->getMetadata()->getFacing() != curGamestate->getFacing();
        }
        
    }
    if (activeFrame == -1) { return 5; }
    auto input = replayFiles[activeReplay].getInput(activeFrame);
    
    //replayFiles[activeReplay].getCase(activeCase)->getMetadata().getFacing() != curGamestate->getFacing()
    if (invertCurCase) {
        input = inverseInput(input);
    }

    return input;
}
void CbrData::resetCbr() {
    activeReplay = -1;
    activeCase = -1;
    activeFrame = -1;
    invertCurCase = false;
}

bool CbrData::switchCaseCheck(Metadata* curGamestate) {
    auto b0 = activeReplay == -1 || activeFrame > replayFiles[activeReplay].getCase(activeCase)->getEndIndex();
    auto b1 = curGamestate->getHitThisFrame()[0] || curGamestate->getBlockThisFrame()[0];
    auto b2 = (curGamestate->getHitThisFrame()[1] || curGamestate->getBlockThisFrame()[1]) && !replayFiles[activeReplay].getCase(activeCase)->getMetadata()->getInputBufferActive();
    if (b2 == true) {
        return true;
    }
    return b0 || b1 || b2;
}
bool CbrData::caseValidityCheck(Metadata* curGamestate, int replayIndex, int caseIndex) {
    auto check1 = (replayIndex != activeReplay || caseIndex != activeCase);
    auto check2 = replayFiles[replayIndex].getCase(caseIndex)->getInputBufferSequence() == false;
    auto check3 = CbrData::nextCaseValidityCheck(curGamestate, replayIndex, caseIndex);

    return check1 && check2 && check3;
}

bool CbrData::nextCaseValidityCheck(Metadata* curGamestate, int replayIndex, int caseIndex) {
    auto check1 = replayFiles[replayIndex].getCase(caseIndex)->heatConsumed <= curGamestate->heatMeter[0];
    auto check2 = replayFiles[replayIndex].getCase(caseIndex)->overDriveConsumed == 0 || curGamestate->overdriveMeter[0] == 100000;
    return check1 && check2;
}

float CbrData::comparisonFunction(Metadata* curGamestate, Metadata* caseGamestate, CbrReplayFile& caseReplay) {
    float compValue = 0;

    compValue += compRelativePosX(curGamestate->getPosX(), caseGamestate->getPosX()) * costXDist;
    compValue += compRelativePosY(curGamestate->getPosY(), caseGamestate->getPosY()) * costYDist;
    compValue += compStateHash(curGamestate->getCurrentActionHash()[0], caseGamestate->getCurrentActionHash()[0]) *costAiState;
    compValue += compStateHash(curGamestate->getCurrentActionHash()[1], caseGamestate->getCurrentActionHash()[1]) * costEnemyState;
    compValue += compStateHash(curGamestate->getLastActionHash()[0], caseGamestate->getLastActionHash()[0]) * costlastAiState;
    compValue += compStateHash(curGamestate->getLastActionHash()[1], caseGamestate->getLastActionHash()[1]) * costlastEnemyState;
    compValue += compNeutralState(curGamestate->getNeutral()[0], caseGamestate->getNeutral()[0]) * costAiNeutral;
    compValue += compNeutralState(curGamestate->getNeutral()[1], caseGamestate->getNeutral()[1]) * costEnemyNeutral;
    compValue += compBool(curGamestate->getAttack()[0], caseGamestate->getAttack()[0]) * costAiAttack;
    compValue += compBool(curGamestate->getAttack()[1], caseGamestate->getAttack()[1]) * costEnemyAttack;
    compValue += compAirborneState(curGamestate->getAir()[0], caseGamestate->getAir()[0]) * costAiAir;
    compValue += compAirborneState(curGamestate->getAir()[1], caseGamestate->getAir()[1]) * costEnemyAir;
    compValue += compWakeupState(curGamestate->getWakeup()[0], caseGamestate->getWakeup()[0]) * costAiWakeup;
    compValue += compWakeupState(curGamestate->getWakeup()[1], caseGamestate->getWakeup()[1]) * costEnemyWakeup;
    compValue += compBlockState(curGamestate->getBlocking()[0], caseGamestate->getBlocking()[0]) * costAiBlocking;
    compValue += compBlockState(curGamestate->getBlocking()[1], caseGamestate->getBlocking()[1]) * costEnemyBlocking;
    compValue += compHitState(curGamestate->getHit()[0], caseGamestate->getHit()[0]) * costAiHit;
    compValue += compHitState(curGamestate->getHit()[1], caseGamestate->getHit()[1]) * costEnemyHit;
    compValue += compGetHitThisFrameState(curGamestate->getHitThisFrame()[0], caseGamestate->getHitThisFrame()[0]) * costAiHitThisFrame;
    compValue += compGetHitThisFrameState(curGamestate->getHitThisFrame()[1], caseGamestate->getHitThisFrame()[1]) * costEnemyHitThisFrame;
    compValue += compBlockingThisFrameState(curGamestate->getBlockThisFrame()[0], caseGamestate->getBlockThisFrame()[0]) * costAiBlockThisFrame;
    compValue += compBlockingThisFrameState(curGamestate->getBlockThisFrame()[1], caseGamestate->getBlockThisFrame()[1]) * costEnemyBlockhisFrame;
    compValue += compCrouching(curGamestate->getCrouching()[0], caseGamestate->getCrouching()[0]) * costAiCrouching;
    compValue += compCrouching(curGamestate->getCrouching()[1], caseGamestate->getCrouching()[1]) * costEnemyCrouching;
    compValue += compIntState(curGamestate->matchState, caseGamestate->matchState) * costMatchState;
    compValue += compBool(curGamestate->overdriveTimeleft[0] > 0, caseGamestate->overdriveTimeleft[0] > 0) * costAiOverdriveState;
    compValue += compBool(curGamestate->overdriveTimeleft[1] > 0, caseGamestate->overdriveTimeleft[1] > 0) * costEnemyOverdriveState;
    compValue += comboSpecificComp(curGamestate, caseGamestate);
    switch (caseReplay.getCharIds()[0])
    {
    case 21://bullet
        compValue += compInt(curGamestate->CharSpecific1[0], caseGamestate->CharSpecific1[0], 2) * costBuHeat; //heat
        break;
    case 22://Azrael
        compValue += compBool(curGamestate->CharSpecific1[0], caseGamestate->CharSpecific1[0]) * 0.1;
        compValue += compBool(curGamestate->CharSpecific2[0], caseGamestate->CharSpecific2[0]) * 0.1;
        compValue += compIntState(curGamestate->CharSpecific3[0], caseGamestate->CharSpecific3[0]) * 0.3;
        break;
    case 6://litchi
        if (curGamestate->CharSpecific1[0] == 17 || caseGamestate->CharSpecific1[0] == 17) {
            compValue += compIntState(curGamestate->CharSpecific1[0], caseGamestate->CharSpecific1[0]) * costLcStaff;
        }
        else {
            compValue += compIntState(curGamestate->CharSpecific1[0], caseGamestate->CharSpecific1[0]) * costLcStaffActive;
        }
        break;
    case 3://rachel
        if (curGamestate->CharSpecific1[0] < rachelWindMin) {
            compValue += compInt(curGamestate->CharSpecific1[0], caseGamestate->CharSpecific1[0], rachelWindMax) * costLowWind;
        }
        else {
            compValue += compInt(curGamestate->CharSpecific1[0], caseGamestate->CharSpecific1[0], rachelWindMax) * costWind;
        }
        break;
    case 5://Tager
        compValue += compBool(curGamestate->CharSpecific1[0], caseGamestate->CharSpecific1[0]) * 0.1;
        compValue += compBool(curGamestate->CharSpecific2[0], caseGamestate->CharSpecific2[0]) * 0.2;
        break;
    case 33://ES
        compValue += compBool(curGamestate->CharSpecific1[0], caseGamestate->CharSpecific1[0]) * 0.2;
        break;
    case 11://Nu
        compValue += compBool(curGamestate->CharSpecific1[0], caseGamestate->CharSpecific1[0]) * 0.1;
        break;
    case 27://Nu
        compValue += compBool(curGamestate->CharSpecific1[0], caseGamestate->CharSpecific1[0]) * 0.1;
        break;
    case 13://Hazama
        compValue += compInt(curGamestate->CharSpecific1[0], caseGamestate->CharSpecific1[0], 2) * 0.2;
        break;
    case 26://Celica
        compValue += compBool(curGamestate->CharSpecific1[0] >0, caseGamestate->CharSpecific1[0] > 0) * 0.2;
        compValue += compInt(curGamestate->CharSpecific1[0], caseGamestate->CharSpecific1[0], 10000) * 1;
        break;
    case 16://Valk
        compValue += compInt(curGamestate->CharSpecific1[0], caseGamestate->CharSpecific1[0], 10000) * 0.1;
        compValue += compIntState(curGamestate->CharSpecific2[0], caseGamestate->CharSpecific2[0]) * 1;
        break;
    case 17://Plat
        compValue += compIntState(curGamestate->CharSpecific1[0], caseGamestate->CharSpecific1[0]) * 0.3;
        compValue += compInt(curGamestate->CharSpecific2[0], caseGamestate->CharSpecific2[0],5) * 0.01;
        compValue += compIntState(curGamestate->CharSpecific3[0], caseGamestate->CharSpecific3[0]) * 0.05;
        break;
    case 18://Relius
        compValue += compIntState(curGamestate->CharSpecific1[0], caseGamestate->CharSpecific1[0]) * 0.1;
        compValue += compBool(curGamestate->CharSpecific1[0]!=0, caseGamestate->CharSpecific1[0] != 0) * 0.6;
        compValue += compInt(curGamestate->CharSpecific2[0], caseGamestate->CharSpecific2[0], 10000) * 0.1;
        compValue += compBool(curGamestate->CharSpecific3[0] != 0, caseGamestate->CharSpecific3[0] != 0) * 1;
        break;
    case 32://Susanoo
        compValue += compBool(curGamestate->CharSpecific1[0]>0, caseGamestate->CharSpecific1[0] > 0) * 0.1;
        compValue += compIntState(curGamestate->CharSpecific1[0], caseGamestate->CharSpecific1[0]) * 0.05;
        compValue += compBool(curGamestate->CharSpecific2[0] > 0, caseGamestate->CharSpecific2[0] > 0) * 0.1;
        compValue += compIntState(curGamestate->CharSpecific2[0], caseGamestate->CharSpecific2[0]) * 0.05;
        compValue += compBool(curGamestate->CharSpecific3[0] > 0, caseGamestate->CharSpecific3[0] > 0) * 0.1;
        compValue += compIntState(curGamestate->CharSpecific3[0], caseGamestate->CharSpecific3[0]) * 0.05;
        compValue += compBool(curGamestate->CharSpecific4[0] > 0, caseGamestate->CharSpecific4[0] > 0) * 0.1;
        compValue += compIntState(curGamestate->CharSpecific4[0], caseGamestate->CharSpecific4[0]) * 0.05;
        compValue += compBool(curGamestate->CharSpecific5[0] > 0, caseGamestate->CharSpecific5[0] > 0) * 0.1;
        compValue += compIntState(curGamestate->CharSpecific5[0], caseGamestate->CharSpecific5[0]) * 0.05;
        compValue += compBool(curGamestate->CharSpecific6[0] > 0, caseGamestate->CharSpecific6[0] > 0) * 0.1;
        compValue += compIntState(curGamestate->CharSpecific6[0], caseGamestate->CharSpecific6[0]) * 0.05;
        compValue += compBool(curGamestate->CharSpecific7[0] > 0, caseGamestate->CharSpecific7[0] > 0) * 0.1;
        compValue += compIntState(curGamestate->CharSpecific7[0], caseGamestate->CharSpecific7[0]) * 0.05;
        compValue += compBool(curGamestate->CharSpecific8[0] > 0, caseGamestate->CharSpecific8[0] > 0) * 0.1;
        compValue += compIntState(curGamestate->CharSpecific8[0], caseGamestate->CharSpecific8[0]) * 0.05;
        compValue += compIntState(curGamestate->CharSpecific9[0], caseGamestate->CharSpecific9[0]) * 0.1;
        break;
    case 35://Jubei
        compValue += compBool(curGamestate->CharSpecific1[0], caseGamestate->CharSpecific1[0]) * 0.3;
        compValue += compBool(curGamestate->CharSpecific2[0], caseGamestate->CharSpecific2[0]) * 0.1;
        break;
    case 31://Izanami
        compValue += compBool(curGamestate->CharSpecific1[0], caseGamestate->CharSpecific1[0]) * 0.2;
        compValue += compBool(curGamestate->CharSpecific2[0], caseGamestate->CharSpecific2[0]) * 0.2;
        compValue += compBool(curGamestate->CharSpecific3[0], caseGamestate->CharSpecific3[0]) * 0.2;
        compValue += compBool(curGamestate->CharSpecific4[0], caseGamestate->CharSpecific4[0]) * 0.2;
        break;
    case 29://Nine
        compValue += compIntState(curGamestate->CharSpecific1[0], caseGamestate->CharSpecific1[0]) * 0.3;
        compValue += compIntState(curGamestate->CharSpecific2[0], caseGamestate->CharSpecific2[0]) * 0.1;
        compValue += compIntState(curGamestate->CharSpecific3[0], caseGamestate->CharSpecific3[0]) * 0.1;
        compValue += compIntState(curGamestate->CharSpecific4[0], caseGamestate->CharSpecific4[0]) * 0.05;
        break;
    case 9://Carl
        compValue += compBool(curGamestate->CharSpecific1[0], caseGamestate->CharSpecific1[0]) * 1;
        compValue += compInt(curGamestate->CharSpecific2[0], caseGamestate->CharSpecific2[0], 5000) * 0.1;
        break;
    case 12://Tsubaki
        compValue += compInt(curGamestate->CharSpecific1[0], caseGamestate->CharSpecific1[0],5) * 1;
        break;
    case 24://Kokonoe
        compValue += compInt(curGamestate->CharSpecific1[0], caseGamestate->CharSpecific1[0], 8) * 0.1;
        compValue += compBool(curGamestate->CharSpecific1[0] > 0, caseGamestate->CharSpecific1[0] > 0) * 0.3;
        break;
    case 19://Izayoi
        compValue += compBool(curGamestate->CharSpecific1[0], caseGamestate->CharSpecific1[0]) * 0.3;
        compValue += compInt(curGamestate->CharSpecific2[0], caseGamestate->CharSpecific2[0],8) * 0.8;
        compValue += compBool(curGamestate->CharSpecific3[0], caseGamestate->CharSpecific3[0]) * 0.4;
        break;
    case 7://Arakune
        compValue += compInt(curGamestate->CharSpecific1[0], caseGamestate->CharSpecific1[0], 6000) * 0.1;
        compValue += compBool(curGamestate->CharSpecific2[0], caseGamestate->CharSpecific2[0]) * 0.8;
        break;
    case 8://Bang
        compValue += compBool(curGamestate->CharSpecific1[0], caseGamestate->CharSpecific1[0]) * 0.05;
        compValue += compBool(curGamestate->CharSpecific2[0], caseGamestate->CharSpecific2[0]) * 0.05;
        compValue += compBool(curGamestate->CharSpecific3[0], caseGamestate->CharSpecific3[0]) * 0.05;
        compValue += compBool(curGamestate->CharSpecific4[0], caseGamestate->CharSpecific4[0]) * 0.05;
        compValue += compInt(curGamestate->CharSpecific5[0], caseGamestate->CharSpecific5[0], 12) * 0.4;
        break;
    case 20://Amane
        compValue += compInt(curGamestate->CharSpecific1[0], caseGamestate->CharSpecific1[0],6000) * 0.4;
        compValue += compBool(curGamestate->CharSpecific2[0], caseGamestate->CharSpecific2[0]) * 0.2;
        break;
    default:
        break;
    }
    switch (caseReplay.getCharIds()[1])
    {
    case 22://Azrael
        compValue += compBool(curGamestate->CharSpecific1[1], caseGamestate->CharSpecific1[1]) * 0.1;
        compValue += compBool(curGamestate->CharSpecific2[1], caseGamestate->CharSpecific2[1]) * 0.1;
        compValue += compIntState(curGamestate->CharSpecific3[1], caseGamestate->CharSpecific3[1]) * 0.3;
        break;
    case 5://Tager
        compValue += compBool(curGamestate->CharSpecific1[1], caseGamestate->CharSpecific1[1]) * 0.1;
        compValue += compBool(curGamestate->CharSpecific2[1], caseGamestate->CharSpecific2[1]) * 0.2;
        break;
    case 13://Hazama
        compValue += compInt(curGamestate->CharSpecific1[1], caseGamestate->CharSpecific1[1], 2) * 0.2;
        break;
    case 16://Valk
        compValue += compInt(curGamestate->CharSpecific1[1], caseGamestate->CharSpecific1[1], 10000) * 0.1;
        compValue += compIntState(curGamestate->CharSpecific2[1], caseGamestate->CharSpecific2[1]) * 1;
        break;
    case 17://Plat
        compValue += compIntState(curGamestate->CharSpecific1[1], caseGamestate->CharSpecific1[1]) * 0.3;
        compValue += compInt(curGamestate->CharSpecific2[1], caseGamestate->CharSpecific2[1], 5) * 0.01;
        compValue += compIntState(curGamestate->CharSpecific3[1], caseGamestate->CharSpecific3[1]) * 0.05;
        break;
    case 18://Relius
        compValue += compIntState(curGamestate->CharSpecific1[1], caseGamestate->CharSpecific1[1]) * 0.1;
        compValue += compBool(curGamestate->CharSpecific1[1] != 0, caseGamestate->CharSpecific1[1] != 0) * 0.6;
        compValue += compInt(curGamestate->CharSpecific2[1], caseGamestate->CharSpecific2[1], 10000) * 0.1;
        compValue += compBool(curGamestate->CharSpecific3[1], caseGamestate->CharSpecific3[1]) * 1;
        break;
    case 32://Susanoo
        compValue += compBool(curGamestate->CharSpecific1[1] > 0, caseGamestate->CharSpecific1[1] > 0) * 0.1;
        compValue += compIntState(curGamestate->CharSpecific1[1], caseGamestate->CharSpecific1[1]) * 0.05;
        compValue += compBool(curGamestate->CharSpecific2[1] > 0, caseGamestate->CharSpecific2[1] > 0) * 0.1;
        compValue += compIntState(curGamestate->CharSpecific2[1], caseGamestate->CharSpecific2[1]) * 0.05;
        compValue += compBool(curGamestate->CharSpecific3[1] > 0, caseGamestate->CharSpecific3[1] > 0) * 0.1;
        compValue += compIntState(curGamestate->CharSpecific3[1], caseGamestate->CharSpecific3[1]) * 0.05;
        compValue += compBool(curGamestate->CharSpecific4[1] > 0, caseGamestate->CharSpecific4[1] > 0) * 0.1;
        compValue += compIntState(curGamestate->CharSpecific4[1], caseGamestate->CharSpecific4[1]) * 0.05;
        compValue += compBool(curGamestate->CharSpecific5[1] > 0, caseGamestate->CharSpecific5[1] > 0) * 0.1;
        compValue += compIntState(curGamestate->CharSpecific5[1], caseGamestate->CharSpecific5[1]) * 0.05;
        compValue += compBool(curGamestate->CharSpecific6[1] > 0, caseGamestate->CharSpecific6[1] > 0) * 0.1;
        compValue += compIntState(curGamestate->CharSpecific6[1], caseGamestate->CharSpecific6[1]) * 0.05;
        compValue += compBool(curGamestate->CharSpecific7[1] > 0, caseGamestate->CharSpecific7[1] > 0) * 0.1;
        compValue += compIntState(curGamestate->CharSpecific7[1], caseGamestate->CharSpecific7[1]) * 0.05;
        compValue += compBool(curGamestate->CharSpecific8[1] > 0, caseGamestate->CharSpecific8[1] > 0) * 0.1;
        compValue += compIntState(curGamestate->CharSpecific8[1], caseGamestate->CharSpecific8[1]) * 0.05;
        break;
    case 35://Jubei
        compValue += compBool(curGamestate->CharSpecific1[1], caseGamestate->CharSpecific1[1]) * 0.3;//Buff
        compValue += compBool(curGamestate->CharSpecific2[1], caseGamestate->CharSpecific2[1]) * 0.1;//Mark
        break;
    case 31://Izanami
        compValue += compBool(curGamestate->CharSpecific1[1], caseGamestate->CharSpecific1[1]) * 0.2;
        compValue += compBool(curGamestate->CharSpecific2[1], caseGamestate->CharSpecific2[1]) * 1;
        compValue += compBool(curGamestate->CharSpecific3[1], caseGamestate->CharSpecific3[1]) * 0.2;
        compValue += compBool(curGamestate->CharSpecific4[1], caseGamestate->CharSpecific4[1]) * 0.2;
        break;
    case 29://Nine
        compValue += compIntState(curGamestate->CharSpecific1[1], caseGamestate->CharSpecific1[1]) * 0.3;
        compValue += compIntState(curGamestate->CharSpecific2[1], caseGamestate->CharSpecific2[1]) * 0.1;
        compValue += compIntState(curGamestate->CharSpecific3[1], caseGamestate->CharSpecific3[1]) * 0.1;
        compValue += compIntState(curGamestate->CharSpecific4[1], caseGamestate->CharSpecific4[1]) * 0.05;
        break;
    case 9://Carl
        compValue += compBool(curGamestate->CharSpecific1[1], caseGamestate->CharSpecific1[1]) * 1;
        compValue += compInt(curGamestate->CharSpecific2[1], caseGamestate->CharSpecific2[1], 5000) * 0.1;
        break;
    case 12://Tsubaki
        compValue += compInt(curGamestate->CharSpecific1[1], caseGamestate->CharSpecific1[1], 5) * 0.2;
        break;
    case 24://Kokonoe
        compValue += compInt(curGamestate->CharSpecific1[1], caseGamestate->CharSpecific1[1], 8) * 0.05;
        compValue += compBool(curGamestate->CharSpecific1[1]>0, caseGamestate->CharSpecific1[1]>0) * 0.1;
        break;
    case 19://Izayoi
        compValue += compBool(curGamestate->CharSpecific1[1], caseGamestate->CharSpecific1[1]) * 0.1;
        compValue += compInt(curGamestate->CharSpecific2[1], caseGamestate->CharSpecific2[1], 8) * 0.2;
        compValue += compBool(curGamestate->CharSpecific3[1], caseGamestate->CharSpecific3[1]) * 0.1;
        break;
    case 7://Arakune
        compValue += compInt(curGamestate->CharSpecific1[1], caseGamestate->CharSpecific1[1],6000) * 0.1;
        compValue += compBool(curGamestate->CharSpecific2[1], caseGamestate->CharSpecific2[1]) * 0.8;
        break;
    case 8://Bang
        compValue += compBool(curGamestate->CharSpecific1[1] , caseGamestate->CharSpecific1[1]) * 0.05;
        compValue += compBool(curGamestate->CharSpecific2[1], caseGamestate->CharSpecific2[1]) * 0.05;
        compValue += compBool(curGamestate->CharSpecific3[1], caseGamestate->CharSpecific3[1]) * 0.05;
        compValue += compBool(curGamestate->CharSpecific4[1], caseGamestate->CharSpecific4[1]) * 0.05;
        compValue += compInt(curGamestate->CharSpecific5[1], caseGamestate->CharSpecific5[1], 12) * 0.1;
        break;
    case 20://Amane
        compValue += compInt(curGamestate->CharSpecific1[1], caseGamestate->CharSpecific1[1], 6000) * 0.4;
        compValue += compBool(curGamestate->CharSpecific2[1], caseGamestate->CharSpecific2[1]) * 0.1;
        break;
    default:
        break;
    }
    return compValue;

}	

int CbrData::makeCharacterID(std::string charName) {
    if (charName == "es") { return 0; };//es
    if (charName == "ny") { return 1; };//nu
    if (charName == "ma") { return 2; };// mai
    if (charName == "tb") { return 3; };// tsubaki
    if (charName == "rg") { return 4; };//ragna
    if (charName == "hz") { return 5; };//hazama
    if (charName == "jn") { return 6; };//jin
    if (charName == "mu") { return 7; };//mu
    if (charName == "no") { return 8; };//noel
    if (charName == "mk") { return 9; };//makoto
    if (charName == "rc") { return 10; };//rachel
    if (charName == "vh") { return 11; };//valk
    if (charName == "tk") { return 12; };//taokaka
    if (charName == "pt") { return 13; };//platinum
    if (charName == "tg") { return 14; };//tager
    if (charName == "rl") { return 15; };//relius
    if (charName == "lc") { return 16; };//litchi
    if (charName == "iz") { return 17; };//izayoi
    if (charName == "ar") { return 18; };//arakune
    if (charName == "am") { return 19; };//amane
    if (charName == "bn") { return 20; };//bang
    if (charName == "bl") { return 21; };//bullet
    if (charName == "ca") { return 22; };//carl
    if (charName == "az") { return 23; };//azrael
    if (charName == "ha") { return 24; };//hakumen
    if (charName == "kg") { return 25; };//kagura
    if (charName == "kk") { return 26; };//koko
    if (charName == "rm") { return 27; };//lambda
    if (charName == "hb") { return 28; };//hibiki
    if (charName == "tm") { return 29; };//terumi
    if (charName == "ph") { return 30; };//nine
    if (charName == "ce") { return 31; };//Celica
    if (charName == "nt") { return 32; };//naoto
    if (charName == "mi") { return 33; };//izanami
    if (charName == "su") { return 34; };//susan
    if (charName == "jb") { return 35; };//jubei
    return -1;
}

float CbrData::comboSpecificComp(Metadata* curGamestate, Metadata* caseGamestate) {
    float compValue = 0;
    if (curGamestate->getHit()[0] == true) {
        compValue += compInt(curGamestate->getComboProration()[0], caseGamestate->getComboProration()[0], maxProration) * costAiProration;
        compValue += compIntState(curGamestate->getStarterRating()[0], caseGamestate->getStarterRating()[0]) * costAiStarterRating;
        compValue += compInt(curGamestate->getComboTime()[0], caseGamestate->getComboTime()[0], maxComboTime) * costAiComboTime;

    }
    if (curGamestate->getHit()[1] == true) {
        compValue += compInt(curGamestate->getComboProration()[1], caseGamestate->getComboProration()[1], maxProration) * costEnemyProration;
        compValue += compIntState(curGamestate->getStarterRating()[1], caseGamestate->getStarterRating()[1]) * costEnemyStarterRating;
        compValue += compInt(curGamestate->getComboTime()[1], caseGamestate->getComboTime()[1], maxComboTime) * costEnemyComboTime;
        compValue += compDistanceToWall(curGamestate->getPosX(), caseGamestate->getPosX(), curGamestate->getFacing(), caseGamestate->getFacing()) * costWallDistCombo;
    }
    else {
        compValue += compDistanceToWall(curGamestate->getPosX(), caseGamestate->getPosX(), curGamestate->getFacing(), caseGamestate->getFacing()) * costWallDist;
    }
    return compValue;
}

float CbrData::HelperCompMatch(Metadata* curGamestate, Metadata* caseGamestate) {
    auto curHelpers = curGamestate->getHelpers()[0];
    auto caseHelpers = caseGamestate->getHelpers()[0];
    float ret = 0;
    int curSize = curHelpers.size();
    int caseSize = caseHelpers.size();
    bool opponent = false;
    bool skip = false;
    if (curSize <= 0 && caseSize <= 0) {
        ret += 0;
        skip = true;
    }
    if (skip == false && curSize <= 0 && caseSize >0) {
        ret += costHelperSum * caseSize;
        skip = true;
    }
    if (skip == false && curSize <= 0 && caseSize > 0) {
        ret += costHelperSum * caseSize;
        skip = true;
    }

    if (skip == false) {
        std::vector<HelperMapping> hMap;

        for (int i = 0; i < curHelpers.size() + 1; ++i) {

            for (int j = 0; j < caseHelpers.size() + 1; ++j) {
                HelperMapping hM{};

                if (j == caseHelpers.size()) {
                    hM.caseHelperIndex = -1;
                }
                else {
                    hM.caseHelperIndex = j;
                }
                if (i == curHelpers.size()) {
                    hM.curHelperIndex = -1;
                }
                else {
                    hM.curHelperIndex = i;
                }
                if (hM.caseHelperIndex == -1 || hM.curHelperIndex == -1) {
                    if (hM.caseHelperIndex == -1 && hM.curHelperIndex == -1) {
                        hM.comparisonValue = 0;
                    }
                    else {
                        Helper* standin = nullptr;
                        if (hM.curHelperIndex == -1) {
                            hM.comparisonValue = HelperComp(curGamestate, caseGamestate, standin, standin, true, opponent);
                        }
                        else {
                            hM.comparisonValue = HelperComp(curGamestate, caseGamestate, curHelpers[i].get(), standin, true, opponent);
                        }
                    }
                }
                else {
                    hM.comparisonValue = HelperComp(curGamestate, caseGamestate, curHelpers[i].get(), caseHelpers[j].get(), false, opponent);
                }

                hMap.push_back(hM);
            }
        }
        std::unordered_map<int, int> usedUpCaseIndex = {};
        std::unordered_map<int, int> usedUpCurIndex = {};
        auto& result1 = findBestHelperMapping(hMap, usedUpCaseIndex, usedUpCurIndex, 0);
        ret += result1.compValue;
    }
    

    //do it again for opponent
    curHelpers = curGamestate->getHelpers()[1];
    caseHelpers = caseGamestate->getHelpers()[1];
    opponent = true;
    curSize = curHelpers.size();
    caseSize = caseHelpers.size();
    std::vector<HelperMapping> hMap = {};
    skip = false;
    if (curSize <= 0 && caseSize <= 0) {
        ret += 0;
        skip = true;
    }
    if (skip == false && curSize <= 0 && caseSize > 0) {
        ret += costHelperSum * caseSize;
        skip = true;
    }
    if (skip == false && curSize <= 0 && caseSize > 0) {
        ret += costHelperSum * caseSize;
        skip = true;
    }
    if (skip == false) {
        for (int i = 0; i < curHelpers.size() + 1; ++i) {

            for (int j = 0; j < caseHelpers.size() + 1; ++j) {
                HelperMapping hM{};

                if (j == caseHelpers.size()) {
                    hM.caseHelperIndex = -1;
                }
                else {
                    hM.caseHelperIndex = j;
                }
                if (i == curHelpers.size()) {
                    hM.curHelperIndex = -1;
                }
                else {
                    hM.curHelperIndex = i;
                }
                if (hM.caseHelperIndex == -1 || hM.curHelperIndex == -1) {
                    if (hM.caseHelperIndex == -1 && hM.curHelperIndex == -1) {
                        hM.comparisonValue = 0;
                    }
                    else {
                        Helper* standin = nullptr;
                        if (hM.curHelperIndex == -1) {
                            hM.comparisonValue = HelperComp(curGamestate, caseGamestate, standin, standin, true, opponent);
                        }
                        else {
                            hM.comparisonValue = HelperComp(curGamestate, caseGamestate, curHelpers[i].get(), standin, true, opponent);
                        }
                        
                    }
                }
                else {
                    hM.comparisonValue = HelperComp(curGamestate, caseGamestate, curHelpers[i].get(), caseHelpers[j].get(), false, opponent);
                }

                hMap.push_back(hM);
            }
        }
        std::unordered_map<int, int> usedUpCaseIndex = {};
        std::unordered_map<int, int> usedUpCurIndex = {};
        auto& result2 = findBestHelperMapping(hMap, usedUpCaseIndex, usedUpCurIndex, 0);
        ret += result2.compValue;
    }
    return ret;
}


recurseHelperMapping CbrData::findBestHelperMapping(std::vector<HelperMapping>& hMap, std::unordered_map<int, int>& usedUpCaseIndex, std::unordered_map<int, int>& usedUpCurIndex, int depth) {
    auto startingIndex = -2;
    float bestCompVal = -1;
    depth++;
    recurseHelperMapping ret;

    for (int i = 0; i < hMap.size(); ++i) {
        auto& val = hMap[i];
        auto& found = usedUpCaseIndex.find(val.caseHelperIndex);
        auto ok = (found != usedUpCaseIndex.end() && found->second <= depth);
        found = usedUpCurIndex.find(val.curHelperIndex);
        auto ok2 = (found != usedUpCurIndex.end() && found->second <= depth);
        auto addable = (!ok || (val.caseHelperIndex == -1 && val.curHelperIndex != -1)) && (!ok2 || val.curHelperIndex == -1);
        if (addable && (val.caseHelperIndex == startingIndex || startingIndex == -2)) {
            startingIndex = val.caseHelperIndex;

            usedUpCaseIndex[val.caseHelperIndex] = depth;
            usedUpCurIndex[val.curHelperIndex] = depth;
            auto& recRet = findBestHelperMapping(hMap, usedUpCaseIndex, usedUpCurIndex, depth);
            auto compVal = recRet.compValue;
            if (compVal < 0) { compVal = 0; }
            compVal += val.comparisonValue;
            if (compVal < bestCompVal || bestCompVal == -1) {
                bestCompVal = compVal;
                ret.hMaps.push_back(&val);
                ret.hMaps.insert(ret.hMaps.end(), recRet.hMaps.begin(), recRet.hMaps.end());
            }
        }
    }
    ret.compValue = bestCompVal;
    return ret;
}

float CbrData::HelperComp(Metadata* curGamestate, Metadata* caseGamestate, Helper* curHelper, Helper* caseHelper, bool autoFail, bool opponent) {
    float compValue = 0;
    double multiplier = 1;

    if (curHelper != nullptr && curHelper->proximityScale) {
        auto distance = fmin((float)abs(curGamestate->getPosX()[!opponent] - curHelper->posX) / maxXDistScreen, 1);
        multiplier = fmax(-0.01731221 + (0.9791619 - -0.01731221) / (1 + pow((distance / 0.364216), (4.017896))), 0) ;
    }

    if (autoFail) {
        return costHelperSum * multiplier;
    }
    compValue += compStateHash(curHelper->typeHash, caseHelper->typeHash) * costHelperType;
    compValue += compRelativePosX(curGamestate->getPosX()[0], curHelper->posX, caseGamestate->getPosX()[0], caseHelper->posX) * costHelperPosX;
    compValue += compRelativePosY(curGamestate->getPosY()[0], curHelper->posY, caseGamestate->getPosY()[0], caseHelper->posY) * costHelperPosY;
    compValue += compStateHash(curHelper->currentActionHash, caseHelper->currentActionHash) * costHelperState;
    compValue += compBool(curHelper->hit, caseHelper->hit) * costHelperHit;
    compValue += compBool(curHelper->attack, caseHelper->attack) * costHelperAttack;
    compValue += compHelperOrder(curGamestate->getPosX()[1], curGamestate->getPosX()[0], curHelper->posX, caseGamestate->getPosX()[1], caseGamestate->getPosX()[0], caseHelper->posX) * costHelperOrder;
    compValue += compHelperOrder(curGamestate->getPosX()[0], curGamestate->getPosX()[1], curHelper->posX, caseGamestate->getPosX()[0], caseGamestate->getPosX()[1], caseHelper->posX) * costHelperOrder;
    compValue = compValue * multiplier;

    return compValue;
}

/*    std::array< int, 2>posX;
    std::array< int, 2>posY;
    bool facing;*/
float  CbrData::compRelativePosX(std::array< int, 2>curPosX, std::array< int, 2>casePosX) {
    auto dif1 = abs(curPosX[0] - curPosX[1]);
    auto dif2 = abs(casePosX[0] - casePosX[1]);
    auto dif3 = abs(dif1 - dif2);
    auto dif4 = fmin((float)dif3 / maxXDist, 1);
    return dif4;
}

float  CbrData::compRelativePosX(int curP1, int curP2, int caseP1, int caseP2) {
    auto dif1 = abs(curP1 - curP2);
    auto dif2 = abs(caseP1 - caseP2);
    auto dif3 = abs(dif1 - dif2);
    auto dif4 = fmin((float)dif3 / maxXDist, 1);
    return dif4;
}

float  CbrData::compRelativePosY(std::array< int, 2>curPosX, std::array< int, 2>casePosX) {
    auto dif1 = (curPosX[0] - curPosX[1]);
    auto dif2 = (casePosX[0] - casePosX[1]);
    auto dif3 = abs(dif1 - dif2);
    auto dif4 = fmin((float)dif3 / maxYDist, 1);
    return dif4;
}

float  CbrData::compRelativePosY(int curP1, int curP2, int caseP1, int caseP2) {
    auto dif1 = abs(curP1 - curP2);
    auto dif2 = abs(caseP1 - caseP2);
    auto dif3 = abs(dif1 - dif2);
    auto dif4 = fmin((float)dif3 / maxYDist, 1);
    return dif4;
}
float CbrData::compHelperOrder(int curPosOpponent, int curPosChar, int curPosHelper, int casePosOpponent, int casePosChar, int casePosHelper) {
    bool curInFront;
    bool caseInFront;
    auto curFacing = curPosChar >= curPosOpponent;
    if (curFacing) {
         curInFront = curPosChar > curPosHelper;
    }
    else {
         curInFront = curPosChar <= curPosHelper;
    }
    auto caseFacing = casePosChar >= casePosOpponent;
    if (caseFacing) {
        caseInFront = casePosChar > casePosHelper;
    }
    else {
        caseInFront = casePosChar <= casePosHelper;
    }

    return curInFront != caseInFront;
    
}

float  CbrData::compDistanceToWall(std::array< int, 2>curPosX, std::array< int, 2>casePosX, bool curFacing, bool caseFacing) {
    int dif1;
    int dif2;
    if (curFacing) {
        dif1 = abs(curPosX[0] - lWallDist);
    }
    else {
        dif1 = abs(curPosX[0] - rWallDist);
    }
    if (caseFacing) {
        dif2 = abs(casePosX[0] - lWallDist);
    }
    else {
        dif2 = abs(casePosX[0] - rWallDist);
    }
    
    auto dif3 = abs(dif1 - dif2);
    auto dif4 = fmin((float)dif3 / maxWallDist, 1);
    return dif4;
}

float  CbrData::compState(std::string curS, std::string caseS) {
    if (curS != caseS) {
        return 1;
    }
    return 0;
}

float  CbrData::compStateHash(size_t curS, size_t caseS) {
    if (curS != caseS) {
        return 1;
    }
    return 0;
}

float CbrData::compInt(int cur, int cas, int max) {
    auto dif = abs(cur - cas);
    float dif2 = fmin(((float)dif / max), 1);
    return dif2;
}

float CbrData::compIntState(int cur, int cas) {
    return cur != cas;
}

float CbrData::compBool(bool cur, bool cas) {
    if (cur != cas) { return 1; }
    return 0;
}

float CbrData::compNeutralState(bool cur, bool cas) {
    if (cur != cas) { return 1; }
    return 0;
}
float CbrData::compAirborneState(bool cur, bool cas) {
    if (cur != cas) { return 1; }
    return 0;
}
float CbrData::compWakeupState(bool cur, bool cas) {
    if (cur != cas) { return 1; }
    return 0;
}
float CbrData::compBlockState(bool cur, bool cas) {
    if (cur != cas) { return 1; }
    return 0;
}
float CbrData::compHitState(bool cur, bool cas) {
    if (cur != cas) { return 1; }
    return 0;
}
float CbrData::compGetHitThisFrameState(bool cur, bool cas) {
    if (cur != cas) { return 1; }
    return 0;
}
float CbrData::compBlockingThisFrameState(bool cur, bool cas) {
    if (cur != cas) { return 1; }
    return 0;
}

float CbrData::compCrouching(bool cur, bool cas) {
    if (cur != cas) { return 1; }
    return 0;
}


int CbrData::inverseInput(int input) {
    auto buffer = input;
    auto test = buffer - specialButton;
    if (test > 0) { buffer = test; }
    test = buffer - tauntButton;
    if (test > 0) { buffer = test; }
    test = buffer - DButton;
    if (test > 0) { buffer = test; }
    test = buffer - CButton;
    if (test > 0) { buffer = test; }
    test = buffer - BButton;
    if (test > 0) { buffer = test; }
    test = buffer - AButton;
    if (test > 0) { buffer = test; }

    if (buffer == 6 || buffer == 3 || buffer == 9) {
        return input - 2;
    }

    if (buffer == 4 || buffer == 7 || buffer == 1) {
        return input + 2;
    }
    return input;
}


void CbrData::debugPrint(CbrCase* nextC, CbrCase* bestC, Metadata* curM, float nextCost, float bestCost, int nextCI, int bestCI, int nextRI, int bestRI) {
    std::string str = "\n--------------------------------\n";
    str += "CI: " + std::to_string(debugTextArr.size()+1) +"\n";
    str += curM->PrintState();
    str += "\nNext Cost: " + std::to_string(nextCost);
    str +=  "\nReplay: " + std::to_string(nextRI)+ " - Case: " + std::to_string(nextCI) + " - Start: " + std::to_string(nextC->getStartingIndex()) + " - End: " + std::to_string(nextC->getEndIndex()) + "\n";
    str += debugPrintCompValues(nextC, curM);


    //str += "\n" + nextC->getMetadata()->PrintState();

    str += "\nBest Cost: " + std::to_string(bestCost);
    str += "\nReplay: " + std::to_string(bestRI) + " - Case: " + std::to_string(bestCI) + " - Start: " + std::to_string(bestC->getStartingIndex()) + " - End: " + std::to_string(bestC->getEndIndex()) + "\n";
    str += debugPrintCompValues(bestC, curM);

    debugTextArr.push_back(str);

}

std::string CbrData::debugPrintCompValues(CbrCase* nextC, Metadata* curM) {
    std::string str = "";
    str += "Cost PosX: " + std::to_string(compRelativePosX(curM->getPosX(), nextC->getMetadata()->getPosX()) * costXDist) + " - ";
    str += " PosX: " + std::to_string(nextC->getMetadata()->getPosX()[0]) + " - " + std::to_string(nextC->getMetadata()->getPosX()[1]) + " - " + std::to_string(abs(nextC->getMetadata()->getPosX()[0] - nextC->getMetadata()->getPosX()[1])) + "\n";
    str += "Cost PosY: " + std::to_string(compRelativePosY(curM->getPosY(), nextC->getMetadata()->getPosY()) * costYDist) + " - ";
    str += " PosY: " + std::to_string(nextC->getMetadata()->getPosY()[0]) + " - " + std::to_string(nextC->getMetadata()->getPosY()[1]) + " - " + std::to_string(abs(nextC->getMetadata()->getPosY()[0] - nextC->getMetadata()->getPosY()[1])) + "\n";

    str += "Cost CurAction: " + std::to_string(compStateHash(curM->getCurrentActionHash()[0], nextC->getMetadata()->getCurrentActionHash()[0]) * costAiState) + " - " + std::to_string(compStateHash(curM->getCurrentActionHash()[1], nextC->getMetadata()->getCurrentActionHash()[1]) * costEnemyState);
    str += " CurAction: " + nextC->getMetadata()->getCurrentAction()[0] + " - " + nextC->getMetadata()->getCurrentAction()[1] + "\n";

    str += "Cost LastAction: " + std::to_string(compStateHash(curM->getLastActionHash()[0], nextC->getMetadata()->getLastActionHash()[0]) * costlastAiState) + " - " + std::to_string(compStateHash(curM->getLastActionHash()[1], nextC->getMetadata()->getLastActionHash()[1]) * costlastEnemyState);
    str += " LastAction: " + nextC->getMetadata()->getCurrentAction()[0] + " - " + nextC->getMetadata()->getCurrentAction()[1] + "\n";

    str += "Cost Air: " + std::to_string(compAirborneState(curM->getAir()[0], nextC->getMetadata()->getAir()[0]) * costAiAir) + " - " + std::to_string(compAirborneState(curM->getAir()[1], nextC->getMetadata()->getAir()[1]) * costEnemyAir);
    str += " Air: " + std::to_string(nextC->getMetadata()->getAir()[0]) + " - " + std::to_string(nextC->getMetadata()->getAir()[1]) + "\n";

    str += "Cost Attack: " + std::to_string(compBool(curM->getAttack()[0], nextC->getMetadata()->getAttack()[0]) * costAiAttack) + " - " + std::to_string(compBool(curM->getAttack()[1], nextC->getMetadata()->getAttack()[1]) * costEnemyAttack);
    str += " Attack: " + std::to_string(nextC->getMetadata()->getAttack()[0]) + " - " + std::to_string(nextC->getMetadata()->getAttack()[1]) + "\n";

    str += "Cost Crouching: " + std::to_string(compCrouching(curM->getCrouching()[0], nextC->getMetadata()->getCrouching()[0]) * costAiCrouching) + " - " + std::to_string(compCrouching(curM->getCrouching()[1], nextC->getMetadata()->getCrouching()[1]) * costEnemyCrouching);
    str += " Crouching: " + std::to_string(nextC->getMetadata()->getCrouching()[0]) + " - " + std::to_string(nextC->getMetadata()->getCrouching()[1]) + "\n";

    str += "Cost Neutral: " + std::to_string(compNeutralState(curM->getNeutral()[0], nextC->getMetadata()->getNeutral()[0]) * costAiNeutral) + " - " + std::to_string(compNeutralState(curM->getNeutral()[1], nextC->getMetadata()->getNeutral()[1]) * costEnemyNeutral);
    str += " Neutral: " + std::to_string(nextC->getMetadata()->getNeutral()[0]) + " - " + std::to_string(nextC->getMetadata()->getNeutral()[1]) + "\n";
    //str += "AtkType: " + std::to_string(attackType[0]) + " - " + std::to_string(attackType[1]) + "\n";

    str += "Cost Wakeup: " + std::to_string(compWakeupState(curM->getWakeup()[0], nextC->getMetadata()->getWakeup()[0]) * costAiWakeup) + " - " + std::to_string(compWakeupState(curM->getWakeup()[1], nextC->getMetadata()->getWakeup()[1]) * costEnemyWakeup);
    str += " Wakeup: " + std::to_string(nextC->getMetadata()->getWakeup()[0]) + " - " + std::to_string(nextC->getMetadata()->getWakeup()[1]) + "\n";

    str += "Cost Blocking: " + std::to_string(compBlockState(curM->getBlocking()[0], nextC->getMetadata()->getBlocking()[0]) * costAiBlocking) + " - " + std::to_string(compBlockState(curM->getBlocking()[1], nextC->getMetadata()->getBlocking()[1]) * costEnemyBlocking);
    str += " Blocking: " + std::to_string(nextC->getMetadata()->getBlocking()[0]) + " - " + std::to_string(nextC->getMetadata()->getBlocking()[1]) + "\n";

    str += "Cost BlockingTF: " + std::to_string(compBlockingThisFrameState(curM->getBlockThisFrame()[0], nextC->getMetadata()->getBlockThisFrame()[0]) * costAiBlockThisFrame) + " - " + std::to_string(compBlockingThisFrameState(curM->getBlockThisFrame()[1], nextC->getMetadata()->getBlockThisFrame()[1]) * costEnemyBlockhisFrame);
    str += " BlockingTF: " + std::to_string(nextC->getMetadata()->getBlockThisFrame()[0]) + " - " + std::to_string(nextC->getMetadata()->getBlockThisFrame()[1]) + "\n";

    str += "Cost Hit: " + std::to_string(compHitState(curM->getHit()[0], nextC->getMetadata()->getHit()[0]) * costAiHit) + " - " + std::to_string(compHitState(curM->getHit()[1], nextC->getMetadata()->getHit()[1]) * costEnemyHit);
    str += " Hit: " + std::to_string(nextC->getMetadata()->getHit()[0]) + " - " + std::to_string(nextC->getMetadata()->getHit()[1]) + "\n";

    str += "Cost HitTF: " + std::to_string(compGetHitThisFrameState(curM->getHitThisFrame()[0], nextC->getMetadata()->getHitThisFrame()[0]) * costAiHitThisFrame) + " - " + std::to_string(compGetHitThisFrameState(curM->getHitThisFrame()[1], nextC->getMetadata()->getHitThisFrame()[1]) * costEnemyHitThisFrame);
    str += " HitTF: " + std::to_string(nextC->getMetadata()->getHitThisFrame()[0]) + " - " + std::to_string(nextC->getMetadata()->getHitThisFrame()[1]) + "\n";

    str += "Cost RoundState: " + std::to_string(compIntState(curM->matchState, nextC->getMetadata()->matchState) * costMatchState);
    str += " RoundState: " + std::to_string(nextC->getMetadata()->matchState) + "\n";

    str += "Cost ODState: " + std::to_string(compBool(curM->overdriveTimeleft[0] > 0, nextC->getMetadata()->overdriveTimeleft[0] > 0) * costAiOverdriveState) + " - " + std::to_string(compBool(curM->overdriveTimeleft[1] > 0, nextC->getMetadata()->overdriveTimeleft[1] > 0) * costEnemyOverdriveState);
    str += " ODState: " + std::to_string(nextC->getMetadata()->overdriveTimeleft[0]) + " - " + std::to_string(nextC->getMetadata()->overdriveTimeleft[1]) + "\n";

    if (curM->getHit()[0] == true) {

        str += "Cost comboPror: " + std::to_string(compInt(curM->getComboProration()[0], nextC->getMetadata()->getComboProration()[0], maxProration) * costAiProration);
        str += " comboPror: " + std::to_string(nextC->getMetadata()->getComboProration()[0]) + "\n";

        str += "Cost starterRating: " + std::to_string(compIntState(curM->getStarterRating()[0], nextC->getMetadata()->getStarterRating()[0]) * costAiStarterRating);
        str += " starterRating: " + std::to_string(nextC->getMetadata()->getStarterRating()[0]) + "\n";

        str += "Cost starterRating: " + std::to_string(compInt(curM->getComboTime()[0], nextC->getMetadata()->getComboTime()[0], maxComboTime) * costAiComboTime);
        str += " comboTime: " + std::to_string(nextC->getMetadata()->getComboTime()[0]) + "\n";

    }
    if (curM->getHit()[1] == true) {

        str += "Cost OPcomboPror: " + std::to_string(compInt(curM->getComboProration()[1], nextC->getMetadata()->getComboProration()[1], maxProration) * costEnemyProration);
        str += " OPcomboPror: " + std::to_string(nextC->getMetadata()->getComboProration()[1]) + "\n";

        str += "Cost OPstarterRating: " + std::to_string(compIntState(curM->getStarterRating()[1], nextC->getMetadata()->getStarterRating()[1]) * costEnemyStarterRating);
        str += " OPstarterRating: " + std::to_string(nextC->getMetadata()->getStarterRating()[1]) + "\n";

        str += "Cost OPstarterRating: " + std::to_string(compInt(curM->getComboTime()[1], nextC->getMetadata()->getComboTime()[1], maxComboTime) * costEnemyComboTime);
        str += " OPcomboTime: " + std::to_string(nextC->getMetadata()->getComboTime()[1]) + "\n";

        str += "Cost DistanceToWallCombo: " + std::to_string(compDistanceToWall(curM->getPosX(), nextC->getMetadata()->getPosX(), curM->getFacing(), nextC->getMetadata()->getFacing()) * costWallDistCombo);
        str += " DistanceToWallCombo: " + std::to_string(nextC->getMetadata()->getFacing()) + "\n";
    }
    else {
        str += "Cost DistanceToWall: " + std::to_string(compDistanceToWall(curM->getPosX(), nextC->getMetadata()->getPosX(), curM->getFacing(), nextC->getMetadata()->getFacing()) * costWallDist);
        str += " DistanceToWall: " + std::to_string(nextC->getMetadata()->getFacing()) + "\n";
    }

    switch (characterIndex)
    {
    default:
        break;
    }


    return str;
}

std::string CbrData::getCharName() {
    return characterName;
}

std::string CbrData::getPlayerName() {
    return playerName;
}

int CbrData::getReplayCount() {
    return replayFiles.size();
}