#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include "utils.hpp"
#include "tagDesc.hpp"
#include <Geode/modify/LevelCell.hpp>

using namespace geode::prelude;

class $modify(TagsLevelCell, LevelCell) {
    struct Fields {
        EventListener<web::WebTask> m_listener;
        std::map<int, std::vector<std::string>> jsonResponse;
    };

    void adjustPositions() {
        auto mainMenu = m_mainLayer->getChildByID("main-menu");
        if (auto title = m_mainLayer->getChildByID("level-name")) title->setPositionY(m_compactView ? 42 : 77);
        if (auto copy = m_mainLayer->getChildByID("copy-indicator")) copy->setPositionY(m_compactView ? 40 : 55);
        if (auto highObject = m_mainLayer->getChildByID("high-object-indicator")) highObject->setPositionY(m_compactView ? 40 : 55);
        if (auto ncsIcon = m_mainLayer->getChildByID("ncs-icon")) ncsIcon->setPositionY(m_compactView ? 27.5 : 39.5);
        if (auto creatorName = mainMenu->getChildByID("creator-name")) creatorName->setPositionY(creatorName->getPositionY() + (m_compactView ? 0 : 4));
        if (auto songName = m_mainLayer->getChildByID("song-name")) songName->setPositionY(m_compactView ? 28 : 40);
        if (m_compactView) for (const auto& id : {"coin-icon-1", "coin-icon-2", "coin-icon-3"}) if (auto icon = m_mainLayer->getChildByID(id)) icon->setPositionY(26.5);
        for (const auto& id : {"length-icon", "length-label", "downloads-icon", "downloads-label", "likes-icon", "likes-label", "orbs-icon", "orbs-label"}) if (auto icon = m_mainLayer->getChildByID(id)) icon->setPositionY(m_compactView ? 6 : 10);
    };

    void loadCustomLevelCell() {
        LevelCell::loadCustomLevelCell();
        if (!Mod::get()->getSettingValue<bool>("levelcellShow")) return;

        m_fields->m_listener.bind([this](web::WebTask::Event* e) {
            if (auto res = e->getValue(); res && res->ok()) {
                auto jsonStr = res->string().unwrapOr("{}");
                auto json = matjson::parse(jsonStr).unwrapOr("{}");
                std::vector<std::string> result;
                std::vector<std::string> order;
                if (Mod::get()->getSettingValue<std::string>("tags-order") == "style, theme, meta, gameplay") {
                    order = {"style", "theme", "meta", "gameplay"};
                } else if (Mod::get()->getSettingValue<std::string>("tags-order") == "meta, gameplay, style, theme") {
                    order = {"meta", "gameplay", "style", "theme"};
                } else if (Mod::get()->getSettingValue<std::string>("tags-order") == "gameplay, meta, style, theme") {
                    order = {"gameplay", "meta", "style", "theme"};
                }
                for (auto key : order) {
                    if (json.contains(key)) {
                        for (auto i = 0; i < json[key].size(); i++) {
                            result.push_back(json[key][i].as<std::string>().unwrap());
                        }
                    }
                }
                adjustPositions();
                m_fields->jsonResponse[m_level->m_levelID.value()] = result;
                updateTags(false);
            }
        });

        auto req = web::WebRequest();
        m_fields->m_listener.setFilter(req.get(
            fmt::format("https://raw.githubusercontent.com/KampWskiR/test3/main/tags/{}.json",m_level ? m_level->m_levelID.value() : GameLevelManager::sharedState()->m_dailyID)
        ));
    };

    CCMenu* createTagContainer(bool extended) {
        auto tagMenu = CCMenu::create();
        tagMenu->setContentSize({230, 12});
        tagMenu->setPosition({m_compactView ? 46.0f : 53.0f, m_compactView ? 21.0f : 31.0f});
        tagMenu->setAnchorPoint({0,1});
        tagMenu->setScale(m_compactView ? 0.75 : 1);
        tagMenu->setID("level-tags");
        
        if (extended) {
            tagMenu->setLayout(RowLayout::create()->setGrowCrossAxis(true)->setAutoScale(false)->setGap(m_compactView ? 1 : 3)->setAxisAlignment(AxisAlignment::Start));
        } else {
            tagMenu->setLayout(RowLayout::create()->setAutoScale(false)->setGap(m_compactView ? 1 : 3)->setAxisAlignment(AxisAlignment::Start));
        }
        if (m_mainLayer->getChildByID("level-place") && m_mainLayer->getChildByID("level-place")->isVisible()) tagMenu->setPositionX(66);

        return tagMenu;
    };
    
    void updateTags(bool extended) {
        if (m_mainLayer->getChildByID("level-tags")) m_mainLayer->removeChildByID("level-tags");

        auto tagMenu = createTagContainer(extended);
        m_mainLayer->addChild(tagMenu);

        if (m_fields->jsonResponse.find(m_level->m_levelID.value()) != m_fields->jsonResponse.end()) {
            auto tags = m_fields->jsonResponse[m_level->m_levelID.value()];

            for (const auto& tag : tags) {
                auto tagNode = CCMenuItemSpriteExtra::create(TagUtils::addTag(tag, 0.35), this, menu_selector(TagDesc::open));
                tagNode->setID(tag);
                tagMenu->addChild(tagNode);
                tagMenu->updateLayout();

                if (!extended && tagNode->getPositionX() > 150) {
                    tagNode->setAnchorPoint({0.5, 0.5});
                    if (tags.size() == tagMenu->getChildrenCount()) break;

                    auto expandSpr = IconButtonSprite::create("tagSquare.png"_spr, CCSprite::createWithSpriteFrameName("PBtn_Arrow_001.png"), "more", "bigFont.fnt");
                    expandSpr->setScale(0.35);

                    auto tagExpand = CCMenuItemSpriteExtra::create(expandSpr, this, menu_selector(TagsLevelCell::expandTags));
                    tagExpand->setAnchorPoint({0.5, 0.5});
                    tagExpand->setColor({255,255,255});
                    tagExpand->setOpacity(255);
                    tagMenu->addChild(tagExpand);
                    tagMenu->updateLayout();
                    break;
                }
            };

            if (extended) {
                auto arrow = CCSprite::createWithSpriteFrameName("PBtn_Arrow_001.png");
                arrow->setRotation(180);
                auto expandSpr = IconButtonSprite::create("tagSquare.png"_spr, arrow, "less", "bigFont.fnt");
                expandSpr->setScale(0.35);

                auto tagExpand = CCMenuItemSpriteExtra::create(expandSpr, this, menu_selector(TagsLevelCell::collapseTags));
                tagExpand->setAnchorPoint({0.5, 0.5});
                tagExpand->setColor({255,255,255});
                tagExpand->setOpacity(255);
                tagMenu->addChild(tagExpand);
                tagMenu->updateLayout();
            };
            
            if (tagMenu->getContentHeight() > 28) {
                if (tagMenu->getContentHeight() > 50) {
                    if (tagMenu->getContentHeight() > 70) {
                        tagMenu->setPositionY(m_compactView ? 48 : 85);
                        if (m_compactView) tagMenu->setScale(0.7);
                        m_mainLayer->getChildByID("level-name")->setVisible(false);
                        m_mainLayer->getChildByID("main-menu")->getChildByID("creator-name")->setVisible(false);
                        for (const auto& id : {"coin-icon-1", "coin-icon-2", "coin-icon-3", "copy-indicator", "high-object-indicator", "song-name"}) if (auto icon = m_mainLayer->getChildByID(id)) icon->setVisible(false);
                    } else {
                        tagMenu->setPositionY(m_compactView ? 48 : 62);
                        if (m_compactView) tagMenu->setScale(0.7);
                        m_mainLayer->getChildByID("level-name")->setVisible(!m_compactView);
                        m_mainLayer->getChildByID("main-menu")->getChildByID("creator-name")->setVisible(false);
                        for (const auto& id : {"coin-icon-1", "coin-icon-2", "coin-icon-3", "copy-indicator", "high-object-indicator", "song-name"}) if (auto icon = m_mainLayer->getChildByID(id)) icon->setVisible(false);
                    }
                } else {
                    tagMenu->setPositionY(m_compactView ? 35 : 45);
                    for (const auto& id : {"coin-icon-1", "coin-icon-2", "coin-icon-3", "song-name"}) if (auto icon = m_mainLayer->getChildByID(id)) icon->setVisible(false);
                    for (const auto& id : {"copy-indicator", "high-object-indicator", "level-name"}) if (auto icon = m_mainLayer->getChildByID(id)) icon->setVisible(true);
                    m_mainLayer->getChildByID("main-menu")->getChildByID("creator-name")->setVisible(true);
                }
            } else {
                for (const auto& id : {"coin-icon-1", "coin-icon-2", "coin-icon-3", "copy-indicator", "high-object-indicator", "song-name", "level-name"}) if (auto icon = m_mainLayer->getChildByID(id)) icon->setVisible(true);
                m_mainLayer->getChildByID("main-menu")->getChildByID("creator-name")->setVisible(true);
            }
            tagMenu->updateLayout();
        }
    };

    void expandTags(CCObject* sender) {
        for (const auto& id : {"length-icon", "length-label", "downloads-icon", "downloads-label", "likes-icon", "likes-label", "orbs-icon", "orbs-label"}) if (auto icon = m_mainLayer->getChildByID(id)) icon->setVisible(false);
        updateTags(true);
    };

    void collapseTags(CCObject* sender) {
        for (const auto& id : {"length-icon", "length-label", "downloads-icon", "downloads-label", "likes-icon", "likes-label", "orbs-icon", "orbs-label"}) if (auto icon = m_mainLayer->getChildByID(id)) icon->setVisible(true);
        updateTags(false);
    };
};