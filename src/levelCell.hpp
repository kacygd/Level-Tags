using namespace geode::prelude;

class $modify(ltLevelCell, LevelCell) {
    struct Fields {
        EventListener<web::WebTask> m_listener;
        std::map<int, std::vector<std::string>> jsonResponse;
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
                if (auto res = e->getValue(); res && res->ok()) {
                    auto json = matjson::parse(res->string().unwrapOr("[]")).unwrapOr("[]");
                    m_fields->jsonResponse[m_level->m_levelID.value()] = json.as<std::vector<std::string>>().unwrap();
                    updateTags(false);
                    adjustPositions();
                }
            });

        auto req = web::WebRequest();

        m_fields->m_listener.setFilter(
            req.get(fmt::format("https://raw.githubusercontent.com/KampWskiR/test3/main/{}.json", m_level ? m_level->m_levelID.value() : GameLevelManager::sharedState()->m_dailyID))
        );
    };

    CCMenu* createTagContainer(bool extended) {
        bool list = false;
        if (m_mainLayer->getChildByID("level-place") && m_mainLayer->getChildByID("level-place")->isVisible()) list = true;

        auto tagMenu = CCMenu::create();
        tagMenu->setContentSize({230, 12});
        tagMenu->setPosition({m_compactView ? 46.0f : 53.0f, m_compactView ? 21.0f : 31.0f});
        tagMenu->setAnchorPoint({0,1});
        tagMenu->setScale(m_compactView ? 0.75 : 1);
        tagMenu->setID("level-tags");
        
        if (list) tagMenu->setPositionX(66);
        if (extended) {
            tagMenu->setLayout(RowLayout::create()->setGrowCrossAxis(true)->setAutoScale(false)->setGap(3)->setAxisAlignment(AxisAlignment::Start));
        } else {
            tagMenu->setLayout(RowLayout::create()->setAutoScale(false)->setGap(3)->setAxisAlignment(AxisAlignment::Start));
        }

        return tagMenu;
    };

    void updateTags(bool extended) {
        if (m_mainLayer->getChildByID("level-tags")) m_mainLayer->removeChildByID("level-tags");

        auto tagMenu = createTagContainer(extended);
        m_mainLayer->addChild(tagMenu);

        int currentLevelID = m_level->m_levelID.value();
        if (m_fields->jsonResponse.find(currentLevelID) != m_fields->jsonResponse.end()) {
            auto tags = m_fields->jsonResponse[currentLevelID];

            for (const auto& tag : tags) {
                IconButtonSprite* tagNode = tagUtils::addTag(tag);
                tagMenu->addChild(tagNode);
                tagMenu->updateLayout();

                if (!extended && tagNode->getPositionX() > 150) {
                    tagNode->setAnchorPoint({0.5, 0.5});

                    if (tags.size() == tagMenu->getChildrenCount()) break;

                    auto expandSpr = IconButtonSprite::create("tagSquare.png"_spr, CCSprite::createWithSpriteFrameName("PBtn_Arrow_001.png"), "more", "bigFont.fnt");
                    expandSpr->setScale(0.35);

                    auto tagExpand = CCMenuItemSpriteExtra::create(expandSpr, this, menu_selector(ltLevelCell::expandTags));
                    tagExpand->setAnchorPoint({0.5, 0.5});
                    tagExpand->setColor({255,255,255});
                    tagExpand->setOpacity(255);
                    tagMenu->addChild(tagExpand);
                    tagMenu->updateLayout();
                    break;
                }
            };

            if (extended) {
                auto arrow = CCSprite::createWithSpriteFrameName("PBtn_Arrow_001.png");
                arrow->setRotation(180);
                auto expandSpr = IconButtonSprite::create("tagSquare.png"_spr, arrow, "less", "bigFont.fnt");
                expandSpr->setScale(0.35);

                auto tagExpand = CCMenuItemSpriteExtra::create(expandSpr, this, menu_selector(ltLevelCell::collapseTags));
                tagExpand->setAnchorPoint({0.5, 0.5});
                tagExpand->setColor({255,255,255});
                tagExpand->setOpacity(255);
                tagMenu->addChild(tagExpand);
                tagMenu->updateLayout();
            };

            if (tagMenu->getContentHeight() > 28) {
                tagMenu->setPositionY(m_compactView ? 35 : 45);
                m_mainLayer->getChildByID("song-name")->setVisible(false);
                for (const auto& id : {"coin-icon-1", "coin-icon-2", "coin-icon-3"}) {
                    if (auto icon = m_mainLayer->getChildByID(id)) icon->setVisible(false);
                }
            } else {
                m_mainLayer->getChildByID("song-name")->setVisible(true);
                for (const auto& id : {"coin-icon-1", "coin-icon-2", "coin-icon-3"}) {
                    if (auto icon = m_mainLayer->getChildByID(id)) icon->setVisible(true);
                }
            }

            tagMenu->updateLayout();
        }
    };

    void expandTags(CCObject* sender) {
        for (const auto& id : {"length-icon", "length-label", "downloads-icon", "downloads-label", "likes-icon", "likes-label", "orbs-icon", "orbs-label"}) {
            if (auto icon = m_mainLayer->getChildByID(id)) icon->setVisible(false);
        }
        updateTags(true);
    }

    void collapseTags(CCObject* sender) {
        for (const auto& id : {"length-icon", "length-label", "downloads-icon", "downloads-label", "likes-icon", "likes-label", "orbs-icon", "orbs-label"}) {
            if (auto icon = m_mainLayer->getChildByID(id)) icon->setVisible(true);
        }
        updateTags(false);
    }
};