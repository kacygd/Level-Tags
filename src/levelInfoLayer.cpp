#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include "utils.hpp"
#include "tagDesc.hpp"
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

#include "requestTag.hpp"

class $modify(TagsLevelInfoLayer, LevelInfoLayer) {
    struct Fields {
        EventListener<web::WebTask> m_listener;
        std::map<int, std::vector<std::string>> jsonResponse;
    };

    void request(CCObject* sender) {RequestTag::create({std::to_string(m_level->m_levelID.value()), m_level->isPlatformer()})->show();}

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
                m_fields->jsonResponse[m_level->m_levelID.value()] = result;
                updateTags();
            }
        });

        auto req = web::WebRequest();
        m_fields->m_listener.setFilter(req.get(fmt::format("https://raw.githubusercontent.com/KampWskiR/test3/main/tags/{}.json", m_level->m_levelID.value())));
    };

    CCMenu* createTagContainer() {
        CCSize winSize = CCDirector::get()->getWinSize();

        auto tagMenu = CCMenu::create();
        tagMenu->setContentWidth(winSize.width);
        tagMenu->setLayout(RowLayout::create()->setAutoScale(false)->setGap(2)->setAxisAlignment(AxisAlignment::Center));
        tagMenu->setPosition({winSize.width / 2, 313});
        tagMenu->setID("level-tags");

        return tagMenu;
    };

    void updateTags() {
        if (auto titleLabel = this->getChildByID("title-label")) titleLabel->setPositionY(titleLabel->getPositionY() - 4);

        auto tagMenu = createTagContainer();
        this->addChild(tagMenu);

        if (m_fields->jsonResponse.find(m_level->m_levelID.value()) != m_fields->jsonResponse.end()) {
            auto tags = m_fields->jsonResponse[m_level->m_levelID.value()];
            for (const auto& tag : tags) {
                auto tagNode = CCMenuItemSpriteExtra::create(TagUtils::addTag(tag, 0.35), this, menu_selector(TagDesc::open));
                tagNode->setID(tag);
                tagMenu->addChild(tagNode);
                tagMenu->updateLayout();
            }
        }
    };
};