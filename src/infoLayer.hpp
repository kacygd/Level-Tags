using namespace geode::prelude;

class $modify(ltInfoLayer, InfoLayer) {
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

    $override
    bool init(GJGameLevel* level, GJUserScore* score, GJLevelList* list) {
        if (!InfoLayer::init(level, score, list)) return false;
        loadCustomInfoLayer();
        return true;
    }

    void loadCustomInfoLayer() {
        if (!Mod::get()->getSettingValue<bool>("infoShow") || !m_level) return;

        auto containerMenu = createTagContainer();
        m_mainLayer->addChild(containerMenu);

        if (auto descBg = m_mainLayer->getChildByID("desc-background")) {
            descBg->setPositionY(208);
        } else {
            m_mainLayer->removeChildByID("level-tags");
            return;
        }
        if (auto descArea = m_mainLayer->getChildByID("description-area")) descArea->setPositionY(208);
        if (auto creatorButton = m_mainLayer->getChildByID("main-menu")->getChildByID("creator-button")) creatorButton->setPositionY(103);

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
                    }
                } else {
                    showNoTagsMessage();
                }
            }
        });

        auto req = web::WebRequest();
        auto levels = req.get(fmt::format("https://raw.githubusercontent.com/KampWskiR/test3/main/{}.json", m_level->m_levelID.value()));
        m_fields->m_listener.setFilter(levels);
    }

    CCMenu* createTagContainer() {
        auto tagMenu = CCMenu::create();
        tagMenu->setContentSize({210, 12});
        tagMenu->setPosition({190, 238.5});
        tagMenu->setID("level-tags");
        tagMenu->setAnchorPoint({1, 1});

        auto spr = CCSprite::create("GJ_progressBar_001.png");
        spr->setScale(0.625);
        auto container = CCMenuItemSpriteExtra::create(spr,this,menu_selector(ltInfoLayer::moreTags));
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
            m_mainLayer->removeChildByID("level-tags");
            m_mainLayer->getChildByID("desc-background")->setPositionY(212);
            m_mainLayer->getChildByID("description-area")->setPositionY(213);
            return;
        }
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