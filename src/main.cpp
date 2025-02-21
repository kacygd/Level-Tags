#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/ui/LoadingSpinner.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>

#include "tagsManager.hpp"
#include "tagDesc.hpp"
#include "levelCell.cpp"
#include "levelInfoLayer.cpp"
#include "betaSearch.hpp"
#include "modTools/modTools.hpp"
#include "tagInfo.hpp"
#include "modTools/modManager.hpp"

using namespace geode::prelude;

class $modify(TagsLevelSearchLayer, LevelSearchLayer) {
    $override
    bool init(int p0) {
        if (!LevelSearchLayer::init(p0)) return false;
        if (!Mod::get()->getSettingValue<bool>("beta-search")) return true;

        if (auto menu = this->getChildByID("other-filter-menu")) {
            auto searchBtn = CCMenuItemSpriteExtra::create(
                CircleButtonSprite::createWithSprite("icon.png"_spr, 1.2, CircleBaseColor::DarkPurple), this, menu_selector(TagsLevelSearchLayer::menu)
            );
            menu->addChild(searchBtn);
            menu->updateLayout();
        }

        return true;
    };
    void menu(CCObject* sender) {BetaSearch::create("")->show();}
};

// class $modify(TagsMenuLayer, MenuLayer) {
//     struct Fields {
//         EventListener<web::WebTask> m_listener;
//     };
//     $override
//     bool init() {
//         if (!MenuLayer::init()) return false;

//         if (!ModManager::sharedState()->authorized && Mod::get()->hasSavedValue("modKey")) {
//             std::string key = Mod::get()->getSavedValue<std::string>("modKey");

//             ModManager::sharedState()->logout();

//             m_fields->m_listener.bind([this, key](web::WebTask::Event* e) {
//                 if (auto res = e->getValue(); res && res->ok()) {
//                     auto jsonStr = res->string().unwrapOr("{}");

//                     ModManager::sharedState()->uiUnlocked = true;

//                     if (jsonStr == "1" || jsonStr == "2" || jsonStr == "3" || jsonStr == "4") {
//                         ModManager::sharedState()->login(key, std::stoi(jsonStr));
//                         Notification::create("Successfully Authorized to Level Tags", NotificationIcon::Success, 1)->show();
//                     } else {
//                         Notification::create("Failed to Authorize", NotificationIcon::Error, 1)->show();
//                     }
//                 }
//             });
//             auto req = web::WebRequest();
//             req.param("action", "login");
//             req.param("i", GJAccountManager::sharedState()->m_accountID);
//             req.param("n", GJAccountManager::sharedState()->m_username);
//             req.param("k", key);
//             m_fields->m_listener.setFilter(req.get(""));
//         }

//         auto menu = this->getChildByID("bottom-menu");
//         auto request = CCMenuItemSpriteExtra::create(CircleButtonSprite::createWithSprite("icon.png"_spr, 1.2, CircleBaseColor::DarkPurple), this, menu_selector(TagsMenuLayer::btn));
//         request->setID("tag-request");
//         menu->addChild(request);
//         menu->updateLayout();

//         return true;
//     };
//     void btn(CCObject* sender) {
//         if (!Mod::get()->getSavedValue<std::string>("modKey").empty()) {
//             ModTools::create(1)->show();
//         } else {
//             ModTools::create(0)->show();
//         }
//     }
// };