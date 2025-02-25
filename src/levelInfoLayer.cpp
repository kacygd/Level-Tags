#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include "tagsManager.hpp"
#include "tagDesc.hpp"
#include "moreTags.hpp"
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

#include "requestTag.hpp"

class $modify(TagsLevelInfoLayer, LevelInfoLayer) {
    struct Fields {
        EventListener<web::WebTask> m_listener;
        matjson::Value tags;
    };

    void request(CCObject* sender) {RequestTag::create({std::to_string(m_level->m_levelID.value()), m_level->isPlatformer()})->show();}

    void moreTags(CCObject* sender) {MoreTags::create(m_fields->tags)->show();}

    $override
    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;

        if (!Mod::get()->getSettingValue<bool>("hideRequest")) {
            auto menu = this->getChildByID("left-side-menu");
            auto request = CCMenuItemSpriteExtra::create(CircleButtonSprite::createWithSprite("icon.png"_spr, 1.2, CircleBaseColor::DarkPurple), this, menu_selector(TagsLevelInfoLayer::request));
            request->setID("tag-request");
            menu->addChild(request);
            menu->updateLayout();
        }

        loadCustomLevelInfoLayer();
        return true;
    };

    void loadCustomLevelInfoLayer() {
        if (!Mod::get()->getSettingValue<bool>("levelInfoShow")) return;

        if (TagsManager::sharedState()->cachedTags[std::to_string(m_level->m_levelID)].size() != 0) {
            m_fields->tags = TagsManager::sortTags(TagsManager::sharedState()->cachedTags[std::to_string(m_level->m_levelID)]);
            updateTags();
            return;
        }

        m_fields->m_listener.bind([this](web::WebTask::Event* e) {
            if (auto res = e->getValue(); res && res->ok()) {
                auto jsonStr = res->string().unwrapOr("{}");
                auto json = matjson::parse(jsonStr).unwrapOr("{}");

                m_fields->tags = TagsManager::sortTags(json);
                TagsManager::sharedState()->cachedTags[std::to_string(m_level->m_levelID)] = json;
                updateTags();
            }
        });

        auto req = web::WebRequest();
        m_fields->m_listener.setFilter(req.get(fmt::format("https://cps.ps.fhgdps.com/v1/tags/{}.json", m_level->m_levelID.value())));
    };

    CCMenu* createTagContainer() {
        CCSize winSize = CCDirector::get()->getWinSize();

        auto tagMenu = CCMenu::create();
        tagMenu->setLayout(RowLayout::create()->setAutoScale(false)->setGap(2)->setAxisAlignment(AxisAlignment::Center));
        tagMenu->setPosition({winSize.width / 2, 313});
        tagMenu->setID("level-tags");

        return tagMenu;
    };

    void updateTags() {
        if (auto titleLabel = this->getChildByID("title-label")) titleLabel->setPositionY(titleLabel->getPositionY() - 4);

        auto tagMenu = createTagContainer();
        this->addChild(tagMenu);

        if (!m_fields->tags.isNull() && m_fields->tags.isArray()) {
            for (const auto& tag : m_fields->tags) {
                auto tagNode = CCMenuItemSpriteExtra::create(TagsManager::addTag(tag.asString().unwrapOr(""), 0.35), this, menu_selector(TagDesc::open));
                tagNode->setID(tag.asString().unwrapOr(""));
                tagMenu->addChild(tagNode);
                tagMenu->updateLayout();
                if (tagNode->getPositionX() > 400) {
                    auto expandSpr = IconButtonSprite::create("tagSquare.png"_spr, CCSprite::createWithSpriteFrameName("PBtn_Arrow_001.png"), "more", "bigFont.fnt");
                    expandSpr->setScale(0.35);

                    auto tagExpand = CCMenuItemSpriteExtra::create(expandSpr, this, menu_selector(TagsLevelInfoLayer::moreTags));
                    tagExpand->setAnchorPoint({0.5, 0.5});
                    tagExpand->setColor({255,255,255});
                    tagExpand->setOpacity(255);
                    tagMenu->addChild(tagExpand);
                    tagMenu->updateLayout();
                    break;
                }
            }
        }
    };
};
