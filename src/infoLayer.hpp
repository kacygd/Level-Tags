using namespace geode::prelude;

class $modify(ltInfoLayer, InfoLayer) {
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
        }
    };

    $override
    bool init(GJGameLevel* level, GJUserScore* score, GJLevelList* list) {
        if (!InfoLayer::init(level, score, list)) return false;
        loadCustomInfoLayer();
        return true;
    }

    void loadCustomInfoLayer() {
        if (!m_level) return;

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
        m_fields->m_listener.setFilter(req.get(fmt::format("https://raw.githubusercontent.com/KampWskiR/test3/main/{}.json", m_level->m_levelID.value())));
    }

    CCMenu* createTagContainer() {
        auto tagMenu = CCMenu::create();
        tagMenu->setContentSize({210, 12});
        tagMenu->setPosition({190, 238.5});
        tagMenu->setID("level-tags");

        auto spr = CCSprite::create("GJ_progressBar_001.png");
        spr->setScale(0.625);

        auto container = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ltInfoLayer::moreTags));
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
        if (auto descArea = m_mainLayer->getChildByID("description-area")) descArea->setPositionY(208);
        if (auto creatorButton = m_mainLayer->getChildByID("main-menu")->getChildByID("creator-button")) creatorButton->setPositionY(103);

        auto tagMenu = createTagContainer();
        m_mainLayer->addChild(tagMenu);

        if (auto descBg = m_mainLayer->getChildByID("desc-background")) {
            descBg->setPositionY(208);
        } else {
            m_mainLayer->removeChild(tagMenu);
            return;
        }

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