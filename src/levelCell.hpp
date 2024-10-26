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

        if (auto copy = m_mainLayer->getChildByID("copy-indicator")) copy->setPositionY(levelPlaceExists ? 40 : 55);
        if (auto highObject = m_mainLayer->getChildByID("high-object-indicator")) highObject->setPositionY(levelPlaceExists ? 40 : 55);
        if (auto ncsIcon = m_mainLayer->getChildByID("ncs-icon")) ncsIcon->setPositionY(levelPlaceExists ? 27.5 : 39.5);
        if (auto creatorName = mainMenu->getChildByID("creator-name")) creatorName->setPositionY(levelPlaceExists ? -119 : creatorName->getPositionY()+4);
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
                    updateTags();
                }
            }
        });

        auto req = web::WebRequest();

        m_fields->m_listener.setFilter(
            req.get(fmt::format("https://raw.githubusercontent.com/KampWskiR/test3/main/{}.json", m_level ? m_level->m_levelID.value() : GameLevelManager::sharedState()->m_dailyID))
        );
    };

    CCMenu* createTagContainer() {
        bool levelPlaceExists = m_mainLayer->getChildByID("level-place") != nullptr;

        auto tagMenu = CCMenu::create();
        tagMenu->setContentSize({210, 12});
        tagMenu->setPosition({levelPlaceExists ? 15.0f : 50.0f, levelPlaceExists ? 9.5f : 19.5f});
        tagMenu->setScale(levelPlaceExists ? 0.75 : 1);
        tagMenu->setID("level-tags");

        auto spr = CCSprite::create("GJ_progressBar_001.png");
        spr->setScale(0.625);

        auto container = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ltLevelCell::moreTags));
        container->setColor({0, 0, 0});
        container->setPosition({105,5});
        container->setAnchorPoint({0.5,0.5});
        container->setID("tags-container");
        container->setOpacity(128);
        tagMenu->addChild(container);

        auto containerMenu = CCMenu::create();
        containerMenu->setID("tags");
        containerMenu->setPosition({0, 2});
        containerMenu->setAnchorPoint({0, 0});
        containerMenu->setLayout(AxisLayout::create()->setAutoScale(false));
        containerMenu->setContentSize({212, 12});
        containerMenu->updateLayout();
        container->addChild(containerMenu);

        return tagMenu;
    };

    void updateTags() {
        adjustPositions();

        auto tagMenu = createTagContainer();
        m_mainLayer->addChild(tagMenu);

        auto container = tagMenu->getChildByID("tags-container");
        auto containerMenu = container->getChildByID("tags");

        int currentLevelID = m_level->m_levelID.value();
        if (m_fields->jsonResponse.find(currentLevelID) != m_fields->jsonResponse.end()) {
            auto texts = m_fields->jsonResponse[currentLevelID];

            float xPos = 7;
            for (const auto& tag : texts) {
                std::string displayText = tag;

                auto tagNode = CCLabelBMFont::create(displayText.c_str(), "bigFont.fnt");
                tagNode->setAnchorPoint({0.5, 0.5});
                tagNode->setScale(0.3);
                tagNode->setColor(color(tag));
                tagNode->setOpacity(255);

                float tagWidth = tagNode->getContentSize().width * tagNode->getScale();

                if (xPos + tagWidth > 200) {
                    float remainingWidth = 200 - xPos;

                    float charWidth = tagNode->getContentSize().width / displayText.length();
                    int maxChars = static_cast<int>(remainingWidth / (tagNode->getScale() * charWidth));

                    if (maxChars > 3) {
                        displayText = displayText.substr(0, maxChars - 3) + "...";
                        tagNode->setString(displayText.c_str());
                    } else {
                        displayText = "...";
                        tagNode->setString(displayText.c_str());
                    }
                    containerMenu->addChild(tagNode);
                    break;
                }
                containerMenu->addChild(tagNode);
                xPos += tagWidth;
            }
            containerMenu->updateLayout();
        }
    };
};