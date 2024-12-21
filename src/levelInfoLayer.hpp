#include "requestTag.hpp"

using namespace geode::prelude;

class $modify(ltLevelInfoLayer, LevelInfoLayer) {
    struct Fields {
        EventListener<web::WebTask> m_listener;
        std::map<int, std::vector<std::string>> jsonResponse;
    };

    void request(CCObject* sender) {
        requestTag::create({std::to_string(m_level->m_levelID.value()), m_level->isPlatformer()})->show();
        //CCDirector::sharedDirector()->pushScene(levelTags::scene(m_level->m_levelID.value()));
    };

    CCMenuItemSpriteExtra* requestBtn() {
        auto request = CCMenuItemSpriteExtra::create(
            CircleButtonSprite::createWithSprite("icon.png"_spr, 1.2, CircleBaseColor::DarkPurple), this, menu_selector(ltLevelInfoLayer::request)
        );
        request->setID("tag-request");
        return request;
    }

    $override
    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;

                        auto menu = this->getChildByID("left-side-menu");
                auto request = requestBtn();
                menu->addChild(request);
                menu->updateLayout();

        loadCustomLevelInfoLayer();
        return true;
    };

    void loadCustomLevelInfoLayer() {
        m_fields->m_listener.bind([this](web::WebTask::Event* e) {
            if (auto res = e->getValue(); res && res->ok()) {
                auto json = matjson::parse(res->string().unwrapOr("[]")).unwrapOr("[]");
                m_fields->jsonResponse[m_level->m_levelID.value()] = json.as<std::vector<std::string>>().unwrap();
                updateTags();
            }
        });

        auto req = web::WebRequest();
        m_fields->m_listener.setFilter(req.get(fmt::format("https://raw.githubusercontent.com/KampWskiR/test3/main/{}.json", m_level->m_levelID.value())));
    };

    CCMenu* createTagContainer() {
        CCSize winSize = CCDirector::get()->getWinSize();

        auto tagMenu = CCMenu::create();
        tagMenu->setContentWidth(winSize.width);
        tagMenu->setLayout(RowLayout::create()->setAutoScale(false)->setGap(3)->setAxisAlignment(AxisAlignment::Center));
        tagMenu->setPosition({winSize.width / 2,winSize.height / 1.022f});
        tagMenu->setScale(0.9);
        tagMenu->setID("level-tags");

        return tagMenu;
    };

    void updateTags() {
        if (auto titleLabel = this->getChildByID("title-label")) titleLabel->setPositionY(titleLabel->getPositionY() - 4);

        auto tagMenu = createTagContainer();
        this->addChild(tagMenu);

        int currentLevelID = m_level->m_levelID.value();
        if (m_fields->jsonResponse.find(currentLevelID) != m_fields->jsonResponse.end()) {
            auto tags = m_fields->jsonResponse[currentLevelID];
            
            for (const auto& tag : tags) {
                IconButtonSprite* tagNode = tagUtils::addTag(tag);
                tagMenu->addChild(tagNode);
                tagMenu->updateLayout();
            }
            tagMenu->updateLayout();
        }
    };
};