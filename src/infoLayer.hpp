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
        CCSize winSize = CCDirector::get()->getWinSize();

        auto tagMenu = CCMenu::create();
        tagMenu->setContentWidth(winSize.width / 1.5);
        tagMenu->setLayout(RowLayout::create()->setAutoScale(true)->setDefaultScaleLimits(0.2,0.35)->setGap(5)->setAxisAlignment(AxisAlignment::Center));
        if (auto desc = m_mainLayer->getChildByID("description-area")) tagMenu->setPosition({winSize.width / 2, desc->getPositionY() + 33});
        tagMenu->setScale(0.8);
        tagMenu->setID("level-tags");

        return tagMenu;
    };

    void updateTags() {
        if (auto descArea = m_mainLayer->getChildByID("description-area")) descArea->setPositionY(descArea->getPositionY() - 4);

        auto tagMenu = createTagContainer();
        m_mainLayer->addChild(tagMenu);

        if (auto descBg = m_mainLayer->getChildByID("desc-background")) {
            descBg->setPositionY(descBg->getPositionY() - 4);
        } else {
            m_mainLayer->removeChild(tagMenu);
            return;
        }

        int currentLevelID = m_level->m_levelID.value();
        if (m_fields->jsonResponse.find(currentLevelID) != m_fields->jsonResponse.end()) {
            auto texts = m_fields->jsonResponse[currentLevelID];

            for (const auto& tag : texts) {
                std::string displayText = tag;

                auto tagNode = IconButtonSprite::create("tagSquare.png"_spr, CCSprite::createWithSpriteFrameName("GJ_noteIcon_001.png"), displayText.c_str(), "bigFont.fnt");
                tagNode->setAnchorPoint({0.5, 0.5});
                tagNode->setScale(0.3);
                tagNode->setColor(color(tag));
                tagNode->setOpacity(255);

                tagMenu->addChild(tagNode);
            }
            tagMenu->updateLayout();
        }
    };
};