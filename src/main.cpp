#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/ui/LoadingSpinner.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>

#include "tagDesc.hpp"
#include "levelCell.cpp"
#include "levelInfoLayer.cpp"
#include "betaSearch.hpp"
#include "tagInfo.hpp"

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
