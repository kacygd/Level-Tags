using namespace geode::prelude;

#include <Geode/modify/PauseLayer.hpp>
#include <Geode/utils/web.hpp>
#include "tagsManager.hpp"
#include "tagDesc.hpp"
#include "moreTags.hpp"

class $modify(TagsPauseLayer, PauseLayer) {
    struct Fields {
        EventListener<web::WebTask> m_listener;
        int levelId = GameManager::sharedState()->m_gameLayer->get()->m_level->m_levelID;
        matjson::Value tags;
    };
    $override
    virtual void customSetup() {
        PauseLayer::customSetup();
        if (!Mod::get()->getSettingValue<bool>("pauseLayerShow")) return;

        if (TagsManager::sharedState()->cachedTags[std::to_string(m_fields->levelId)].size() != 0) {
            m_fields->tags = TagsManager::sortTags(TagsManager::sharedState()->cachedTags[std::to_string(m_fields->levelId)]);
            updateTags();
            return;
        }

        m_fields->m_listener.bind([this](web::WebTask::Event* e) {
            if (auto res = e->getValue(); res && res->ok()) {
                auto jsonStr = res->string().unwrapOr("{}");
                auto json = matjson::parse(jsonStr).unwrapOr("{}");

                m_fields->tags = TagsManager::sortTags(json);
                TagsManager::sharedState()->cachedTags[std::to_string(m_fields->levelId)] = json;
                updateTags();
            }
        });

        auto req = web::WebRequest();
        m_fields->m_listener.setFilter(req.get(fmt::format("https://cps.ps.fhgdps.com/v1/tags/{}.json", m_fields->levelId)));
    }
    void updateTags() {
        CCSize winSize = CCDirector::get()->getWinSize();

        auto tagMenu = CCMenu::create();
        tagMenu->setLayout(RowLayout::create()->setAutoScale(false)->setGap(2)->setAxisAlignment(AxisAlignment::Center));
        tagMenu->setPosition({winSize.width / 2, 310});
        tagMenu->setID("level-tags");
        this->addChild(tagMenu);

        if (!m_fields->tags.isNull() && m_fields->tags.isArray()) {
            for (const auto& tag : m_fields->tags) {
                auto tagNode = CCMenuItemSpriteExtra::create(TagsManager::addTag(tag.asString().unwrapOr(""), 0.35), this, menu_selector(TagDesc::open));
                tagNode->setID(tag.asString().unwrapOr(""));
                tagMenu->addChild(tagNode);
                tagMenu->updateLayout();
                if (tagNode->getPositionX() > 450) {
                    auto expandSpr = IconButtonSprite::create("tagSquare.png"_spr, CCSprite::createWithSpriteFrameName("PBtn_Arrow_001.png"), "more", "bigFont.fnt");
                    expandSpr->setScale(0.35);

                    auto tagExpand = CCMenuItemSpriteExtra::create(expandSpr, this, menu_selector(TagsPauseLayer::moreTags));
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
    void moreTags(CCObject* sender) {MoreTags::create(m_fields->tags)->show();}
};
