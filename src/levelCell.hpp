using namespace geode::prelude;

class $modify(ltLevelCell, LevelCell) {
    struct Fields {
        EventListener<web::WebTask> m_listener;
        std::map<int, std::vector<std::string>> jsonResponse;
    };

    ccColor3B color(const std::string& tag) {
        std::hash<std::string> hasher;
        size_t hashValue = hasher(tag);
        return {
            static_cast<GLubyte>((hashValue >> 16) & 0xFF),
            static_cast<GLubyte>((hashValue >> 8) & 0xFF),
            static_cast<GLubyte>(hashValue & 0xFF)
        };
    };

    void moreTags(CCObject* sender) {
        int currentLevelID = m_level->m_levelID.value();
        if (m_fields->jsonResponse.find(currentLevelID) != m_fields->jsonResponse.end()) {
            auto texts = m_fields->jsonResponse[currentLevelID];
            auto displayText = fmt::format("{}", fmt::join(texts, "  "));
            FLAlertLayer::create(m_level->m_levelName.c_str(), displayText, "OK")->show();
        };
    };

    void adjustPositions() {
        bool levelPlaceExists = m_mainLayer->getChildByID("level-place") != nullptr;
        auto mainMenu = m_mainLayer->getChildByID("main-menu");

        if (auto title = m_mainLayer->getChildByID("level-name")) title->setPositionY(levelPlaceExists ? 42 : 77);
        if (auto copy = m_mainLayer->getChildByID("copy-indicator")) copy->setPositionY(levelPlaceExists ? 40 : 55);
        if (auto highObject = m_mainLayer->getChildByID("high-object-indicator")) highObject->setPositionY(levelPlaceExists ? 40 : 55);
        if (auto ncsIcon = m_mainLayer->getChildByID("ncs-icon")) ncsIcon->setPositionY(levelPlaceExists ? 27.5 : 39.5);
        if (auto creatorName = mainMenu->getChildByID("creator-name")) creatorName->setPositionY(creatorName->getPositionY() + (levelPlaceExists ? 0 : 4));
        if (auto songName = m_mainLayer->getChildByID("song-name")) songName->setPositionY(levelPlaceExists ? 28 : 40);
        if (levelPlaceExists) {
            for (const auto& id : {"coin-icon-1", "coin-icon-2", "coin-icon-3"}) {
                if (auto icon = m_mainLayer->getChildByID(id)) icon->setPositionY(26.5);
            }
        }
        for (const auto& id : {"length-icon", "length-label", "downloads-icon", "downloads-label", "likes-icon", "likes-label", "orbs-icon", "orbs-label"}) {
            if (auto icon = m_mainLayer->getChildByID(id)) icon->setPositionY(levelPlaceExists ? 6 : 10);
        }
    };
    
    void loadCustomLevelCell() {
        LevelCell::loadCustomLevelCell();
        if (!Mod::get()->getSettingValue<bool>("levelcellShow")) return;

        m_fields->m_listener.bind([this](web::WebTask::Event* e) {
            if (auto res = e->getValue()) {
                if (res->ok()) {
                    std::string error;
                    auto json = matjson::parse(res->string().unwrapOr("[]"));
                    if (json.is_null()) json = matjson::Array();
                    if (!error.empty()) log::error("{}", error);
                    auto tags = json.as<std::vector<std::string>>();
                    m_fields->jsonResponse[m_level->m_levelID.value()] = tags;
                    updateTags(false);
                    adjustPositions();
                }
            }
        });

        auto req = web::WebRequest();

        m_fields->m_listener.setFilter(
            req.get(fmt::format("https://raw.githubusercontent.com/KampWskiR/test3/main/{}.json", m_level ? m_level->m_levelID.value() : GameLevelManager::sharedState()->m_dailyID))
        );
    };

    CCMenu* createTagContainer(bool extended) {
        bool levelPlaceExists = m_mainLayer->getChildByID("level-place") != nullptr;

        auto tagMenu = CCMenu::create();
        tagMenu->setContentSize({210, 12});
        tagMenu->setPosition({levelPlaceExists ? 41.0f : 53.0f, levelPlaceExists ? 11.0f : 25.0f});
        if (extended) {
            tagMenu->setLayout(RowLayout::create()->setGrowCrossAxis(true)->setAutoScale(false)->setGap(5)->setAxisAlignment(AxisAlignment::Start));
            tagMenu->setAnchorPoint({0,0.8});
        } else {
            tagMenu->setLayout(RowLayout::create()->setAutoScale(false)->setGap(5)->setAxisAlignment(AxisAlignment::Start));
            tagMenu->setAnchorPoint({0,0.5});
        }
        tagMenu->setScale(levelPlaceExists ? 0.75 : 1);
        tagMenu->setID("level-tags");

        return tagMenu;
    };

    void updateTags(bool extended) {
        if (m_mainLayer->getChildByID("level-tags")) m_mainLayer->removeChildByID("level-tags");

        auto tagMenu = createTagContainer(extended);
        m_mainLayer->addChild(tagMenu);

        int currentLevelID = m_level->m_levelID.value();
        if (m_fields->jsonResponse.find(currentLevelID) != m_fields->jsonResponse.end()) {
            auto texts = m_fields->jsonResponse[currentLevelID];

            for (const auto& tag : texts) {
                std::string displayText = tag;

                auto tagNode = IconButtonSprite::create("tagSquare.png"_spr, CCSprite::createWithSpriteFrameName("GJ_noteIcon_001.png"), displayText.c_str(), "bigFont.fnt");
                tagNode->setAnchorPoint({1, 0.5});
                tagNode->setScale(0.3);
                tagNode->setColor(color(tag));
                tagNode->setOpacity(255);
                tagMenu->addChild(tagNode);
                tagMenu->updateLayout();
                if (!extended) {
                    if (tagNode->getPositionX() > 150) {
                        tagNode->setAnchorPoint({0.5, 0.5});

                        auto expandSpr = IconButtonSprite::create("tagSquare.png"_spr, CCSprite::createWithSpriteFrameName("PBtn_Arrow_001.png"), "more", "bigFont.fnt");
                        expandSpr->setScale(0.3);
                        auto tagExpand = CCMenuItemSpriteExtra::create(
                            expandSpr,
                            this,
                            menu_selector(ltLevelCell::expandTags)
                        );
                        tagExpand->setAnchorPoint({0.5, 0.5});
                        tagExpand->setColor({255,255,255});
                        tagExpand->setOpacity(255);
                        tagMenu->addChild(tagExpand);
                        tagMenu->updateLayout();

                        break;
                    };
                }
            }
            if (extended) {
                        auto arrow = CCSprite::createWithSpriteFrameName("PBtn_Arrow_001.png");
                        arrow->setRotation(180);
                        auto expandSpr = IconButtonSprite::create("tagSquare.png"_spr, arrow, "less", "bigFont.fnt");
                        expandSpr->setScale(0.3);
                        auto tagExpand = CCMenuItemSpriteExtra::create(
                            expandSpr,
                            this,
                            menu_selector(ltLevelCell::foldTags)
                        );
                        tagExpand->setAnchorPoint({0.5, 0.5});
                        tagExpand->setColor({255,255,255});
                        tagExpand->setOpacity(255);
                        tagMenu->addChild(tagExpand);
                        tagMenu->updateLayout();
            };
        }
    };

    void expandTags(CCObject* sender) {
        for (const auto& id : {"length-icon", "length-label", "downloads-icon", "downloads-label", "likes-icon", "likes-label", "orbs-icon", "orbs-label"}) {
            if (auto icon = m_mainLayer->getChildByID(id)) icon->setVisible(false);
        }
        updateTags(true);
    };
    void foldTags(CCObject* sender) {
        for (const auto& id : {"length-icon", "length-label", "downloads-icon", "downloads-label", "likes-icon", "likes-label", "orbs-icon", "orbs-label"}) {
            if (auto icon = m_mainLayer->getChildByID(id)) icon->setVisible(true);
        }
        updateTags(false);
    };
};