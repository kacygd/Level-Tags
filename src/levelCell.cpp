#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include "tagsManager.hpp"
#include "tagDesc.hpp"
#include <Geode/modify/LevelCell.hpp>

using namespace geode::prelude;

class $modify(TagsLevelCell, LevelCell) {
    struct Fields {
        EventListener<web::WebTask> m_listener;
        matjson::Value tags;
        bool extended = false;
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
        for (const auto& id : {"length-icon", "length-label", "downloads-icon", "downloads-label", "likes-icon", "likes-label", "orbs-icon", "orbs-label"}) {
            if (auto icon = m_mainLayer->getChildByID(id)) icon->setPositionY(m_compactView ? 6 : 10);
        }
    };

    void loadCustomLevelCell() {
        LevelCell::loadCustomLevelCell();
        if (!Mod::get()->getSettingValue<bool>("levelcellShow")) return;

        if (TagsManager::sharedState()->cachedTags[std::to_string(m_level->m_levelID)].size() != 0) {
            adjustPositions();
            m_fields->tags = TagsManager::sortTags(TagsManager::sharedState()->cachedTags[std::to_string(m_level->m_levelID)]);
            updateTags(false);
            return;
        }

        m_fields->m_listener.bind([this](web::WebTask::Event* e) {
            if (auto res = e->getValue(); res && res->ok()) {
                auto jsonStr = res->string().unwrapOr("{}");
                auto json = matjson::parse(jsonStr).unwrapOr("{}");

                adjustPositions();
                m_fields->tags = TagsManager::sortTags(json);
                TagsManager::sharedState()->cachedTags[std::to_string(m_level->m_levelID)] = json;
                updateTags(false);
            }
        });

        auto req = web::WebRequest();
        m_fields->m_listener.setFilter(req.get(
            fmt::format("https://cps.ps.fhgdps.com/v1/tags/{}.json",m_level ? m_level->m_levelID.value() : GameLevelManager::sharedState()->m_dailyID)
        ));
    };

    CCMenu* createTagContainer(bool extended) {
        auto tagMenu = CCMenu::create();
        tagMenu->setContentSize({230, 12});
        tagMenu->setPosition({m_compactView ? 46.0f : 53.0f, m_compactView ? 21.0f : 31.0f});
        tagMenu->setAnchorPoint({0,1});
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

        m_fields->extended = extended;

        auto tagMenu = createTagContainer(extended);
        m_mainLayer->addChild(tagMenu);

        if (!m_fields->tags.isNull() && m_fields->tags.isArray()) {

            for (auto& tag : m_fields->tags) {
                auto tagNode = CCMenuItemSpriteExtra::create(TagsManager::addTag(tag.asString().unwrapOr(""), m_compactView ? 0.25 : 0.35), this, menu_selector(TagDesc::open));
                tagNode->setID(tag.asString().unwrapOr(""));
                tagMenu->addChild(tagNode);
                tagMenu->updateLayout();
                if (!extended && tagNode->getPositionX() > 125) {
                    tagNode->setAnchorPoint({0.5, 0.5});
                    if (m_fields->tags.size() == tagMenu->getChildrenCount()) break;

                    auto expandSpr = IconButtonSprite::create("tagSquare.png"_spr, CCSprite::createWithSpriteFrameName("PBtn_Arrow_001.png"), "more", "bigFont.fnt");
                    expandSpr->setScale(m_compactView ? 0.25 : 0.35);

                    auto tagExpand = CCMenuItemSpriteExtra::create(expandSpr, this, menu_selector(TagsLevelCell::setExpand));
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
                expandSpr->setScale(m_compactView ? 0.25 : 0.35);

                auto tagExpand = CCMenuItemSpriteExtra::create(expandSpr, this, menu_selector(TagsLevelCell::setExpand));
                tagExpand->setAnchorPoint({0.5, 0.5});
                tagExpand->setColor({255,255,255});
                tagExpand->setOpacity(255);
                tagMenu->addChild(tagExpand);
                tagMenu->updateLayout();
            };

            auto iconIds = {"coin-icon-1", "coin-icon-2", "coin-icon-3", "copy-indicator", "high-object-indicator", "song-name"};
            if ((m_compactView && tagMenu->getContentHeight() > 36) || (!m_compactView && tagMenu->getContentHeight() > 70)) {
                tagMenu->setPositionY(m_compactView ? 48 : 85);
                m_mainLayer->getChildByID("level-name")->setVisible(false);
                m_mainLayer->getChildByID("main-menu")->getChildByID("creator-name")->setVisible(false);
                for (const auto& id : iconIds) if (auto icon = m_mainLayer->getChildByID(id)) icon->setVisible(false);
            } else if (tagMenu->getContentHeight() > 50) {
                tagMenu->setPositionY(m_compactView ? 48 : 62);
                if (m_compactView) tagMenu->setScale(0.7);
                m_mainLayer->getChildByID("level-name")->setVisible(!m_compactView);
                m_mainLayer->getChildByID("main-menu")->getChildByID("creator-name")->setVisible(false);
                for (const auto& id : iconIds) if (auto icon = m_mainLayer->getChildByID(id)) icon->setVisible(false);
            } else if (tagMenu->getContentHeight() > 28) {
                tagMenu->setPositionY(m_compactView ? 32 : 45);
                for (const auto& id : {"coin-icon-1", "coin-icon-2", "coin-icon-3", "song-name"}) if (auto icon = m_mainLayer->getChildByID(id)) icon->setVisible(false);
                for (const auto& id : {"copy-indicator", "high-object-indicator", "level-name"}) if (auto icon = m_mainLayer->getChildByID(id)) icon->setVisible(true);
                m_mainLayer->getChildByID("main-menu")->getChildByID("creator-name")->setVisible(true);
            } else {
                for (const auto& id : iconIds) if (auto icon = m_mainLayer->getChildByID(id)) icon->setVisible(true);
                if (auto name = m_mainLayer->getChildByID("level-name")) name->setVisible(true);
                m_mainLayer->getChildByID("main-menu")->getChildByID("creator-name")->setVisible(true);
            }

            tagMenu->updateLayout();
        }
    };

    void setExpand(CCObject* sender) {
        for (const auto& id : {"length-icon", "length-label", "downloads-icon", "downloads-label", "likes-icon", "likes-label", "orbs-icon", "orbs-label"}) {
            if (auto icon = m_mainLayer->getChildByID(id)) icon->setVisible(m_fields->extended);
        } updateTags(!m_fields->extended);
    }
};
