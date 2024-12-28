using namespace geode::prelude;

#include "requestTag.hpp"

class $modify(ltLevelInfoLayer, LevelInfoLayer) {
    struct Fields {
        EventListener<web::WebTask> m_listener;
        std::map<int, std::vector<std::string>> jsonResponse;
    };

    void request(CCObject* sender) {requestTag::create({std::to_string(m_level->m_levelID.value()), m_level->isPlatformer()})->show();}

    $override
    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;

        auto menu = this->getChildByID("left-side-menu");
        auto request = CCMenuItemSpriteExtra::create(CircleButtonSprite::createWithSprite("icon.png"_spr, 1.2, CircleBaseColor::DarkPurple), this, menu_selector(ltLevelInfoLayer::request));
        request->setID("tag-request");
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
                auto tagNode = CCMenuItemSpriteExtra::create(tagUtils::addTag(tag, 0.35), this, menu_selector(ltLevelCell::tagDesc));
                tagNode->setID(tag);
                tagMenu->addChild(tagNode);
                tagMenu->updateLayout();
            }
        }
    };
};