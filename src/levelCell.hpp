using namespace geode::prelude;

class $modify(ltLevelCell, LevelCell) {
    struct Fields {
        EventListener<web::WebTask> m_listener;
        std::map<int, std::vector<std::string>> jsonResponse;
    };
    void moreTags(CCObject* sender) {
        int currentLevelID = m_level->m_levelID.value();
        if (m_fields->jsonResponse.find(currentLevelID) != m_fields->jsonResponse.end()) {
            auto texts = m_fields->jsonResponse[currentLevelID];
            auto displayText = fmt::format("{}", fmt::join(texts, "  "));
            FLAlertLayer::create(m_level->m_levelName.c_str(), displayText, "OK")->show();
        } else {
            geode::createQuickPopup(
                m_level->m_levelName.c_str(),
                "This level doesn't have tags, yet.",
                "OK", "Request Tags",  
                [](auto, bool btn2) {
                    if (btn2) {
                        web::openLinkInBrowser("https://forms.gle/ox2VzydErPFdQd5S6");
                    }
                }
            );
        }
    };

    void adjustPositions() {
        bool levelPlaceExists = m_mainLayer->getChildByID("level-place") != nullptr;
        auto mainMenu = m_mainLayer->getChildByID("main-menu");

        if (auto copy = m_mainLayer->getChildByID("copy-indicator")) copy->setPositionY(levelPlaceExists ? 40 : 55);
        if (auto highObject = m_mainLayer->getChildByID("high-object-indicator")) highObject->setPositionY(levelPlaceExists ? 40 : 55);
        if (auto ncsIcon = m_mainLayer->getChildByID("ncs-icon")) ncsIcon->setPositionY(levelPlaceExists ? 27.5 : 39.5);
        if (auto creatorName = mainMenu->getChildByID("creator-name")) {
            creatorName->setPositionY(levelPlaceExists ? -119 : -103);
            creatorName->setAnchorPoint({0.5, 0.5});
        }
        if (auto songName = m_mainLayer->getChildByID("song-name")) {
            songName->setPositionY(levelPlaceExists ? 28.0f : 40.0f);
            songName->setAnchorPoint({0, 0.5});
        }
        if (levelPlaceExists) {
            for (const auto& id : {"coin-icon-1", "coin-icon-2", "coin-icon-3"}) {
                if (auto icon = m_mainLayer->getChildByID(id)) icon->setPositionY(26.5);
            }
        }
        for (const auto& id : {"length-icon", "length-label", "downloads-icon", "downloads-label", "likes-icon", "likes-label", "orbs-icon", "orbs-label"}) {
            if (auto icon = m_mainLayer->getChildByID(id)) icon->setPositionY(levelPlaceExists ? 6 : 10);
        }
    }

    void loadCustomLevelCell() {
        LevelCell::loadCustomLevelCell();
        if (!Mod::get()->getSettingValue<bool>("levelcellShow")) return;
        adjustPositions();

        auto containerMenu = createTagContainer();
        m_mainLayer->addChild(containerMenu);

        m_fields->m_listener.bind([this](web::WebTask::Event* e) {
            if (auto res = e->getValue()) {
                if (res->ok()) {
                    try {
                        auto json = matjson::parse(res->string().unwrapOr("[]"));
                        auto tags = json.as<std::vector<std::string>>();
                        m_fields->jsonResponse[m_level->m_levelID.value()] = tags;
                        updateTags();
                    } catch (const std::runtime_error& ex) {
                        log::error("{}", ex.what());
                    };
                } else {
                    showNoTagsMessage();
                }
            }
        });

        auto req = web::WebRequest();
        if (m_level) {
            auto levels = req.get(fmt::format("https://raw.githubusercontent.com/KampWskiR/test3/main/{}.json", m_level->m_levelID.value()));
            m_fields->m_listener.setFilter(levels);
        } else {
            auto levels = req.get(fmt::format("https://raw.githubusercontent.com/KampWskiR/test3/main/{}.json", GameLevelManager::sharedState()->m_dailyID));
            m_fields->m_listener.setFilter(levels);
        }
    }

    CCMenu* createTagContainer() {
        bool levelPlaceExists = m_mainLayer->getChildByID("level-place") != nullptr;

        auto tagMenu = CCMenu::create();
        tagMenu->setContentSize({210, 12});
        tagMenu->setPositionX(levelPlaceExists ? 15 : 50);
        tagMenu->setPositionY(levelPlaceExists ? 9.5 : 19.5);
        tagMenu->setScale(levelPlaceExists ? 0.75 : 1);
        tagMenu->setID("level-tags");
        tagMenu->setAnchorPoint({1, 1});

        auto spr = CCSprite::create("GJ_progressBar_001.png");
        spr->setScale(0.625);

        auto container = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ltLevelCell::moreTags));
        container->setColor({0, 0, 0});
        container->setPosition({105,5});
        container->setAnchorPoint({0.5,0.5});
        container->setID("tags-container");
        container->setOpacity({128});
        tagMenu->addChild(container);

        auto containerMenu = CCMenu::create();
        containerMenu->setID("tags");
        containerMenu->setPosition({0, 2});
        containerMenu->setAnchorPoint({0, 0});
        containerMenu->setLayout(AxisLayout::create()->setAutoScale(false));
        containerMenu->setContentSize({212, 12});
        containerMenu->updateLayout();
        container->addChild(containerMenu);

        auto loadingText = CCLabelBMFont::create("Loading...", "bigFont.fnt");
        loadingText->setPosition(85, 2);
        loadingText->setAnchorPoint({0, 0});
        loadingText->setScale(0.3);
        loadingText->setID("loading");
        loadingText->setColor({255, 255, 255});
        loadingText->setOpacity(255);
        container->addChild(loadingText);

        return tagMenu;
    }

ccColor3B color(const std::string& tag) {
    std::hash<std::string> hasher;
    size_t hashValue = hasher(tag);
    GLubyte r = static_cast<GLubyte>((hashValue & 0xFF0000) >> 16);
    GLubyte g = static_cast<GLubyte>((hashValue & 0x00FF00) >> 8);
    GLubyte b = static_cast<GLubyte>(hashValue & 0x0000FF);
    return ccColor3B{r, g, b};
}

void updateTags() {
    auto contain = m_mainLayer->getChildByID("level-tags");
    if (!contain) return;

    auto container = contain->getChildByID("tags-container");
    container->removeChildByID("loading");

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
    } else {
        showNoTagsMessage();
    }
}
    void showNoTagsMessage() {
        if (Mod::get()->getSettingValue<bool>("HideNoTags")) {
            bool levelPlaceExists = m_mainLayer->getChildByID("level-place") != nullptr;
            m_mainLayer->removeChildByID("level-tags");
            if (auto creatorName = m_mainLayer->getChildByID("main-menu")->getChildByID("creator-name")) creatorName->setPositionY(levelPlaceExists ? -119 : -107);
            if (auto songName = m_mainLayer->getChildByID("song-name")) songName->setPositionY(levelPlaceExists ? 24 : 33);
            if (auto ncsIcon = m_mainLayer->getChildByID("ncs-icon")) ncsIcon->setPositionY(levelPlaceExists ? 23 : 32);
            if (auto copy = m_mainLayer->getChildByID("copy-indicator")) copy->setPositionY(levelPlaceExists ? 40 : 52);
            if (auto highObject = m_mainLayer->getChildByID("high-object-indicator")) highObject->setPositionY(levelPlaceExists ? 40 : 52);
            for (const auto& id : {"length-icon", "length-label", "downloads-icon", "downloads-label", "likes-icon", "likes-label", "orbs-icon", "orbs-label"}) {
            if (auto icon = m_mainLayer->getChildByID(id)) icon->setPositionY(levelPlaceExists ? 8.5 : 14);
            } return;
        };

        auto containerMenu = m_mainLayer->getChildByID("level-tags");
        if (!containerMenu) return;
        auto container = containerMenu->getChildByID("tags-container");
        container->removeChildByID("loading");

        auto tags = container->getChildByID("tags");

        auto noTags = CCLabelBMFont::create("This level doesn't have tags, yet", "bigFont.fnt");
        noTags->setPosition(17, 0);
        noTags->setAnchorPoint({0, 0});
        noTags->setScale(0.3);
        noTags->setColor({255, 255, 255});
        noTags->setOpacity(255);
        tags->addChild(noTags);
    }
};