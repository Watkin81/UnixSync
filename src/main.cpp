// v1.3.0
#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/binding/GameStatsManager.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/binding/PlayLayer.hpp>
#include <Geode/modify/LevelOptionsLayer.hpp>
#include <Geode/binding/TextGameObject.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/GJBaseGameLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <chrono>
#include <ctime>
#include <random>

using namespace geode::prelude;

int currentTime_t = 0;
int currentTime_unix = 0;
bool is_enabled = false;
bool uses_unix = false;
//int timeZoneOffset = -5;
int lastUnix = 99999999999;
GameObject* object;

bool checkForEditorText() {
    for (auto* obj: CCArrayExt<GameObject*>(GameManager::sharedState()->getEditorLayer()->m_objects)) {
        if (obj->m_objectID == 914) {
            auto* text = static_cast<TextGameObject*>(obj);
            if (text && text->m_text == std::string_view("~!ThisTextEnablesUnix!~")) {
                //log::info("checkforeditortext found object: {}", obj);
                object = obj;
                return true;
            }
        }
    }
    return false;
};

CCSprite* testEnabled(CCSprite* toggleOn, CCSprite* toggleOff) {
    return (is_enabled) ? toggleOn : toggleOff;
};

class $modify(UnixTimeSwap, LevelOptionsLayer) {

    void setupOptions() {
        LevelOptionsLayer::setupOptions();
        CCMenu* menu = CCMenu::create();
        auto toggleOn = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
        auto toggleOff = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
        CCMenuItemToggler* btn = CCMenuItemToggler::create(testEnabled(toggleOn, toggleOff), testEnabled(toggleOff, toggleOn), this, menu_selector(UnixTimeSwap::enable_unix));
        
        if (checkForEditorText() && !is_enabled) {
            btn->activate();
        }
        else if (!checkForEditorText() && is_enabled) {
            btn->activate();
        }

        CCLabelBMFont* label = CCLabelBMFont::create("Unix Time", "bigFont.fnt");

        //btn->setPosition({316,182.8}); sorry Xanii D:
        btn->setPosition({125,80});
        btn->setScale(0.82);

        label->setPosition({btn->getPositionX() + 54, btn->getPositionY()});
        label->setScale(0.4);
        addChild(label);

        menu->setPosition({0,0});
        menu->addChild(btn);
        addChild(menu);
        handleTouchPriority(this);
    }

    void enable_unix(CCObject*) {
        is_enabled = !is_enabled;
        if (is_enabled) {
            object = GameManager::sharedState()->getEditorLayer()->createObject(914, ccp(-500000, 500000), true);
            TextGameObject* textObject = static_cast<TextGameObject*>(object);
            textObject->m_text = std::string("~!ThisTextEnablesUnix!~");
        }
        else if (!is_enabled) {
            for (auto* obj: CCArrayExt<GameObject*>(GameManager::sharedState()->getEditorLayer()->m_objects)) {
                if (obj->m_objectID == 914) {
                    auto* text = static_cast<TextGameObject*>(obj);
                    if (text && text->m_text == std::string_view("~!ThisTextEnablesUnix!~")) {
                        log::info("found UNIX text object, deleting the object on this level!");
                        //log::info("object is: {}", obj);
                        GameManager::sharedState()->getEditorLayer()->removeObject(obj, true);
                    }
                }
            }        
        }
    }
};

class $modify(PlayLayer) {
    bool init(GJGameLevel* p0, bool p1, bool p2) {
        if (!PlayLayer::init(p0,p1,p2)) {
            return false;
        }

        uses_unix = false;
        for (auto* obj: CCArrayExt<GameObject*>(GameManager::sharedState()->getPlayLayer()->m_objects)) {
            if (obj->m_objectID == 914) {
                auto* text = static_cast<TextGameObject*>(obj);
                if (text && text->m_text == std::string_view("~!ThisTextEnablesUnix!~")) {
                    log::info("found UNIX text object, enabling UnixSync functionality on this level!");
                    uses_unix = true;
                }
            }
        }

        if (uses_unix) {
            auto winSize = CCDirector::sharedDirector()->getWinSize();

            auto enabledDisplay = CCLabelBMFont::create("<text here>", "bigFont.fnt");
            if (Mod::get()->getSettingValue<bool>("showunix")) {
                enabledDisplay->setPosition(60, 5);
            }
            else {
                enabledDisplay->setPosition(19, 5);
            }
            enabledDisplay->setScale(0.25);
            enabledDisplay->setOpacity(127);
            auto node = CCNode::create();
            node->setZOrder(10);
            node->addChild(enabledDisplay);
            node->setID("unixtimestamp");
            this->addChild(node);
        }
        return true;
    }
};

class $modify(MyBaseGameLayer, GJBaseGameLayer) {
    void update(float delta) override {
        GJBaseGameLayer::update(delta);

        //log::info("uses: {}", uses_unix);
        if (uses_unix) {
            if (currentTime_unix != lastUnix) { // run once a second
                oncePerSecond(1.f);
            }
 
            auto currentTime = std::chrono::system_clock::now();
            currentTime_t = std::chrono::system_clock::to_time_t(currentTime);
            currentTime_unix = currentTime_t;
            int currentTime_full = currentTime_t % 1000000;
            currentTime_t = currentTime_t % 10000;
            int digit1 = currentTime_t % 10;
            int digit2 = (currentTime_t / 10) % 10;
            int digit3 = (currentTime_t / 100) % 10;
            int digit4 = (currentTime_t / 1000) % 10;
            m_effectManager->updateCountForItem(7501, currentTime_t);
            updateCounters(7501, currentTime_t);
            m_effectManager->updateCountForItem(7505, digit1);
            updateCounters(7505, digit1);
            m_effectManager->updateCountForItem(7504, digit2);
            updateCounters(7504, digit2);
            m_effectManager->updateCountForItem(7503, digit3);
            updateCounters(7503, digit3);
            m_effectManager->updateCountForItem(7502, digit4);
            updateCounters(7502, digit4);
            if (currentTime_full > 999999) {
                currentTime_full = 999999;
            }
            m_effectManager->updateCountForItem(7506, currentTime_full);
                updateCounters(7506, currentTime_full);
            //log::info("ItemIDs Updated");
            //log::info("last: {}", lastUnix);
            //log::info("current: {}", currentTime_unix);

            if (PlayLayer::get() != nullptr) {
                auto playLayer = PlayLayer::get();
                if (auto unixNode = typeinfo_cast<CCNode*>(playLayer->getChildByID("unixtimestamp"))) {
                    auto unixLabel = static_cast<CCLabelBMFont*>(unixNode->getChildren()->objectAtIndex(0));
                    if (Mod::get()->getSettingValue<bool>("showunix")) {
                        unixLabel->setString(fmt::format("Unix Enabled ({})", currentTime_unix).c_str());
                    } else {
                        unixLabel->setString("Unix On");
                    }
                }
            }
        }
    }

    void oncePerSecond(float) {
        //log::info("Once Per Second");
        std::mt19937 rng(currentTime_unix);
        std::uniform_int_distribution<int> distribution(0, 99);
        int pseudo_random = distribution(rng);
        m_effectManager->updateCountForItem(7507, pseudo_random);
        updateCounters(7507, pseudo_random);
        time_t time_seconds = static_cast<time_t>(currentTime_unix);
        tm* timeInfoUTC = gmtime(&time_seconds);
        int currentYear = timeInfoUTC->tm_year + 1900;
        int currentMonth = timeInfoUTC->tm_mon + 1;
        int currentDay = timeInfoUTC->tm_mday;
        int currentHourEST = (timeInfoUTC->tm_hour);
        int currentMinute = timeInfoUTC->tm_min;
        int currentSecond = timeInfoUTC->tm_sec;
        m_effectManager->updateCountForItem(7508, currentYear);
        updateCounters(7508, currentYear);
        m_effectManager->updateCountForItem(7509, currentMonth);
        updateCounters(7509, currentMonth);
        m_effectManager->updateCountForItem(7510, currentDay);
        updateCounters(7510, currentDay);
        m_effectManager->updateCountForItem(7511, currentHourEST);
        updateCounters(7511, currentHourEST);
        m_effectManager->updateCountForItem(7512, currentMinute);
        updateCounters(7512, currentMinute);
        m_effectManager->updateCountForItem(7513, currentSecond);
        updateCounters(7513, currentSecond);
        lastUnix = currentTime_unix;
    }
};

// gaming