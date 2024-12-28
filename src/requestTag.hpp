class submit : public geode::Popup<bool> {
protected:
    bool setup(bool success) {
        auto successIcon = CCSprite::createWithSpriteFrameName(success ? "GJ_completesIcon_001.png" : "GJ_deleteIcon_001.png");
        successIcon->setScale(1.5);
        successIcon->setPosition({m_mainLayer->getContentWidth() / 2, 60});
        m_mainLayer->addChild(successIcon);

        auto successLabel = CCLabelBMFont::create(success ? "Tags Request submitted!" : "Tags Request failed", "chatFont.fnt");
        successLabel->setScale(0.75);
        successLabel->setPosition({m_mainLayer->getContentWidth() / 2, 30});
        m_mainLayer->addChild(successLabel);

        m_mainLayer->updateLayout();

        return true;
    }
public:
    static submit* create(bool success) {
        auto ret = new submit();
        if (ret->initAnchored(180.f, 100.f, success, "GJ_square02.png")) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    };
};

class requestTag : public geode::Popup<std::vector<std::variant<std::string, bool>>> {
protected:
    TextInput* customInput;
    CCMenu* menuCustom;
    CCMenu* menuStyle;
    CCMenu* menuTheme;
    CCMenu* menuMeta;
    CCMenu* menuGameplay;
    CCLabelBMFont* limit;
    std::vector<std::string> tags;
    EventListener<web::WebTask> m_listener;
    std::vector<std::variant<std::string, bool>> m_level;
    int tagZ = 0;

    CCMenu* createMenu(std::string name) {
        CCMenu* menu = CCMenu::create();
        menu->setContentSize({340,100});
        menu->setAnchorPoint({0.5,1});
        menu->setPosition({m_mainLayer->getContentWidth() / 2,150});
        menu->setVisible(false);
        menu->setID(fmt::format("menu-{}", name));
        menu->setLayout(AxisLayout::create()->setAutoScale(false)->setGrowCrossAxis(true)->setGap(2));
        menu->updateLayout();

        return menu;
    }

    void createCorners() {
        auto c1 = CCSprite::createWithSpriteFrameName("dailyLevelCorner_001.png");
        c1->setPosition({24.6f,24.6f});
        m_mainLayer->addChild(c1);

        auto c2 = CCSprite::createWithSpriteFrameName("dailyLevelCorner_001.png");
        c2->setRotation(90);
        c2->setPosition({24.6f,225.4f});
        m_mainLayer->addChild(c2);

        auto c3 = CCSprite::createWithSpriteFrameName("dailyLevelCorner_001.png");
        c3->setRotation(180);
        c3->setPosition({375.f,225.4f});
        m_mainLayer->addChild(c3);

        auto c4 = CCSprite::createWithSpriteFrameName("dailyLevelCorner_001.png");
        c4->setRotation(270);
        c4->setPosition({375.f,24.6f});
        m_mainLayer->addChild(c4);
    }

    void addCustomMenu() {
        auto tagNode = IconButtonSprite::create("tagSquare.png"_spr, CCSprite::createWithSpriteFrameName("gimmicky.png"_spr), "          ", "bigFont.fnt");
        tagNode->setPosition({menuCustom->getContentWidth() / 2, 60});
        tagNode->setScale(1);
        tagNode->setColor({255,255,255});
        tagNode->setOpacity(255);
        if (tagNode->getIcon()) tagNode->getIcon()->setScale(0.5);
        menuCustom->addChild(tagNode);

        customInput = TextInput::create(110, "tag", "bigFont.fnt");
        customInput->setPosition({(menuCustom->getContentWidth() / 2) + 12, 60});
        customInput->setScale(0.9);
        customInput->hideBG();
        customInput->setMaxCharCount(20);
        menuCustom->addChild(customInput);
        
        auto add = CCMenuItemSpriteExtra::create(ButtonSprite::create("Add"), this, menu_selector(requestTag::customAdd));
        add->setPosition({menuCustom->getContentWidth() / 2,20});
        add->setTag(1);
        menuCustom->addChild(add);
        m_mainLayer->addChild(menuCustom);
    };

    void addTagsList(int tag, std::vector<std::string> tags) {
        CCMenu* menus[] = { menuStyle, menuTheme, menuMeta, menuGameplay };
        if (tag < 0 || tag > 5) return;

        for (int i = 0; i < tags.size(); i++) {
            auto spr = tagUtils::addTag(tags[i].c_str(), 0.5);
            auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(requestTag::btn));
            btn->setID(tags[i]);
            btn->setZOrder(tagZ);
            btn->setTag(tag);
            menus[tag-2]->addChild(btn);
            tagZ++;
        }
        m_mainLayer->addChild(menus[tag-2]);
        menus[tag-2]->updateLayout();
    };

    bool setup(std::vector<std::variant<std::string, bool>> level) {
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        m_level = level;

        this->setTitle("Request Tags");
        m_title->setScale(1);

        createCorners();

        auto bgInput = CCScale9Sprite::create("square02b_001.png");
        bgInput->setContentSize({360,30});
        bgInput->setPosition({m_mainLayer->getContentWidth() / 2,195});
        bgInput->setColor({107, 59, 25});
        m_mainLayer->addChild(bgInput);

        auto menu = CCMenu::create();
        menu->setContentSize({265,30});
        menu->setPosition({m_mainLayer->getContentWidth() / 2.6f,195});
        menu->setID("menu"); 
        menu->setLayout(AxisLayout::create()->setGap(5)->setDefaultScaleLimits(0.3f,1.f));
        menu->updateLayout();
        m_mainLayer->addChild(menu);

        auto bgMenu = CCScale9Sprite::create("square02b_001.png");
        bgMenu->setContentSize({360,150});
        bgMenu->setPosition({m_mainLayer->getContentWidth() / 2,100});
        bgMenu->setColor({107, 59, 25});
        m_mainLayer->addChild(bgMenu);

        auto discordMenu = CCMenu::create();
        discordMenu->setContentSize({0,0});
        discordMenu->setPosition({-20,15});
        m_mainLayer->addChild(discordMenu);
        discordMenu->addChild(CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("gj_discordIcon_001.png"), this, menu_selector(requestTag::discord)));

        limit = CCLabelBMFont::create("0/10", "chatFont.fnt");
        limit->setPosition({333,215});
        limit->setScale(0.5);
        limit->setColor({0,0,0});
        limit->setID("tag-limit");
        m_mainLayer->addChild(limit);

        auto submitMenu = CCMenu::create();
        submitMenu->setContentSize({90,25.5});
        submitMenu->setPosition({288,182.5});
        submitMenu->setID("submit-menu");
        m_mainLayer->addChild(submitMenu);

        auto categoryMenu = CCMenu::create();
        categoryMenu->setContentSize({355,22});
        categoryMenu->setPosition({m_mainLayer->getContentWidth() / 2 ,162});
        categoryMenu->setLayout(AxisLayout::create()->setGap(5)->setDefaultScaleLimits(0.3f,1.f));
        categoryMenu->setID("category-menu");
        m_mainLayer->addChild(categoryMenu);

        auto submitSpr = ButtonSprite::create("Submit");
        submitSpr->setScale(0.85);
        auto submit = CCMenuItemSpriteExtra::create(submitSpr, this, menu_selector(requestTag::submitTags) );
        submit->setPosition({submitMenu->getContentWidth() / 2,submitMenu->getContentHeight() / 2});
        submitMenu->addChild(submit);

        auto categoryCustom = CCMenuItemSpriteExtra::create(ButtonSprite::create("Custom"), this, menu_selector(requestTag::category));
        categoryCustom->setTag(1);
        categoryMenu->addChild(categoryCustom);

        auto categoryStyle = CCMenuItemSpriteExtra::create(ButtonSprite::create("Style"), this, menu_selector(requestTag::category));
        categoryStyle->setTag(2);
        categoryMenu->addChild(categoryStyle);

        auto categoryTheme = CCMenuItemSpriteExtra::create(ButtonSprite::create("Theme"), this, menu_selector(requestTag::category));
        categoryTheme->setTag(3);
        categoryMenu->addChild(categoryTheme);

        auto categoryMeta = CCMenuItemSpriteExtra::create(ButtonSprite::create("Meta"), this, menu_selector(requestTag::category));
        categoryMeta->setTag(4);
        categoryMenu->addChild(categoryMeta);

        auto categoryGameplay = CCMenuItemSpriteExtra::create(ButtonSprite::create("Gameplay"), this, menu_selector(requestTag::category));
        categoryGameplay->setTag(5);
        categoryMenu->addChild(categoryGameplay);

        categoryMenu->updateLayout();

        menuCustom = createMenu("custom");
        m_mainLayer->addChild(menuCustom);

        menuStyle = createMenu("style");
        menuStyle->setVisible(true);
        m_mainLayer->addChild(menuStyle);

        menuTheme = createMenu("theme");
        m_mainLayer->addChild(menuTheme);

        menuMeta = createMenu("meta");
        m_mainLayer->addChild(menuMeta);

        menuGameplay = createMenu("gameplay");
        m_mainLayer->addChild(menuGameplay);

        addCustomMenu();

        auto loadingTags = LoadingSpinner::create(30);
        loadingTags->setPosition({m_mainLayer->getContentWidth() / 2, 95});
        m_mainLayer->addChild(loadingTags);

        m_listener.bind([this, loadingTags](web::WebTask::Event* e) {
            if (auto res = e->getValue(); res && res->ok()) {
                m_mainLayer->removeChild(loadingTags);
                auto jsonStr = res->string().unwrapOr("{}");
                auto json = matjson::parse(jsonStr).unwrapOr("{}");
                addTagsList(2,json["style"].as<std::vector<std::string>>().unwrap());
                addTagsList(3,json["theme"].as<std::vector<std::string>>().unwrap());
                if (std::get<bool>(m_level[1])) { menuGameplay->setScale(0.9);
                    addTagsList(4,json["plat-meta"].as<std::vector<std::string>>().unwrap());
                    addTagsList(5,json["plat-gp"].as<std::vector<std::string>>().unwrap());
                } else {
                    addTagsList(4,json["classic-meta"].as<std::vector<std::string>>().unwrap());
                    addTagsList(5,json["classic-gp"].as<std::vector<std::string>>().unwrap());
                }
            }
        });
        auto req = web::WebRequest();
        m_listener.setFilter(req.get("https://raw.githubusercontent.com/KampWskiR/test3/main/tags.json"));

        return true;
    };

    void discord(CCObject* sender) {web::openLinkInBrowser("https://discord.gg/6GXYHf9WTB");}

    void category(CCObject* sender) {
        CCMenuItemSpriteExtra* category = static_cast<CCMenuItemSpriteExtra*>(sender);
        menuCustom->setVisible(false);
        menuStyle->setVisible(false);
        menuTheme->setVisible(false);
        menuMeta->setVisible(false);
        menuGameplay->setVisible(false);

        switch (category->getTag()) {
            case 1: menuCustom->setVisible(true); break;
            case 2: menuStyle->setVisible(true); break;
            case 3: menuTheme->setVisible(true); break;
            case 4: menuMeta->setVisible(true); break;
            case 5: menuGameplay->setVisible(true); break;
        }
    };

    void customAdd(CCObject* sender) {
        if (tags.size() >= 10 || customInput->getString().size() < 2) return;
        std::string tagString = customInput->getString();
        if (std::string tagString = customInput->getString(); tagString.size() > 20)  tagString = tagString.substr(0, 20) + "...";
        if (std::find(tags.begin(), tags.end(), tagString) != tags.end()) return;
        tags.push_back(tagString);

        auto tagNode = IconButtonSprite::create("tagSquare.png"_spr, CCSprite::createWithSpriteFrameName("gimmicky.png"_spr), tagString.c_str(), "bigFont.fnt");
        tagNode->setScale(0.5);
        tagNode->setID(tagString.c_str());
        tagNode->setTag(1);
        tagNode->setColor({255,255,255});
        tagNode->setOpacity(255);
        if (tagNode->getIcon()) tagNode->getIcon()->setScale(0.5);

        auto btn = CCMenuItemSpriteExtra::create(tagNode, this, menu_selector(requestTag::btn));
        btn->setID(tagString);
        btn->setTag(1);

        if (tags.size() <= 5) m_mainLayer->getChildByID("menu")->setLayout(AxisLayout::create()->setGap(5)->setDefaultScaleLimits(0.3f,1.f));
        if (tags.size() > 5) m_mainLayer->getChildByID("menu")->setLayout(AxisLayout::create()->setGap(2)->setDefaultScaleLimits(0.3f,0.6f)->setGrowCrossAxis(true));

        m_mainLayer->getChildByID("menu")->addChild(btn);
        m_mainLayer->getChildByID("menu")->updateLayout();

        limit->setString(fmt::format("{}/10", tags.size()).c_str());
        limit->setColor({static_cast<GLubyte>(std::clamp<int>((tags.size() / 10.0) * 255, 0, 255)), 0, 0});
        customInput->setString("");
    };

    void btn(CCObject* sender) {
        CCMenuItemSpriteExtra* clickedButton = static_cast<CCMenuItemSpriteExtra*>(sender);

        if (tags.size() <= 5) m_mainLayer->getChildByID("menu")->setLayout(AxisLayout::create()->setGap(5)->setDefaultScaleLimits(0.3f,1.f));
        if (tags.size() > 5) m_mainLayer->getChildByID("menu")->setLayout(AxisLayout::create()->setGap(2)->setDefaultScaleLimits(0.3f,0.6f)->setGrowCrossAxis(true));

        if (clickedButton->getParent()->getID() == "menu") {
            tags.erase(std::remove(tags.begin(), tags.end(), clickedButton->getID()), tags.end());
            clickedButton->getParent()->removeChild(clickedButton);
            clickedButton->m_baseScale = 1;
            clickedButton->setScale(1);
            switch (clickedButton->getTag()) {
                case 2: menuStyle->addChild(clickedButton); break;
                case 3: menuTheme->addChild(clickedButton); break;
                case 4: menuMeta->addChild(clickedButton); break;
                case 5: menuGameplay->addChild(clickedButton); break;
            }
            menuStyle->updateLayout();
            menuTheme->updateLayout();
            menuMeta->updateLayout();
            menuGameplay->updateLayout();
        } else {
            if (tags.size() >= 10) return;
            tags.push_back(clickedButton->getID());
            clickedButton->getParent()->removeChild(clickedButton);
            m_mainLayer->getChildByID("menu")->addChild(clickedButton);
            m_mainLayer->getChildByID("menu")->updateLayout();
        };
        menuStyle->updateLayout();
        menuTheme->updateLayout();
        menuMeta->updateLayout();
        menuGameplay->updateLayout();
        m_mainLayer->getChildByID("menu")->updateLayout();

        limit->setString(fmt::format("{}/10",tags.size()).c_str());
        limit->setColor({static_cast<GLubyte>(std::clamp<int>((tags.size() / 10.0) * 255, 0, 255)), 0, 0});
    };

    void submitTags(CCObject* sender) {
        if (tags.size() < 2) {
            limit->setString("Add at least 2 tags");
            limit->setColor({255,0,0});
            return;
        }
        CCMenuItemSpriteExtra* submitBtn = static_cast<CCMenuItemSpriteExtra*>(sender);
        std::string tagsRequest;
        for (const auto& tag : tags) tagsRequest += fmt::format("{}, ", tag);
        tagsRequest.pop_back();
        tagsRequest.pop_back();
        m_listener.bind([this](web::WebTask::Event* e) {
            if (auto res = e->getValue()) {
                this->onClose(this);
                submit::create(res->ok() || res->code() == 405)->show();
            }
        });

        auto req = web::WebRequest();
        req.bodyString(fmt::format("id={}&tags={}", std::get<std::string>(m_level[0]), tagsRequest));
        m_listener.setFilter(req.post(fmt::format("https://script.google.com/macros/s/AKfycbyDbwwXM6vDzR9832XUHbqJ1quZR6O87aAxrMjHdf0THPg3SCp6faVVKy_EANQ1_TmG/exec")));

        m_mainLayer->getChildByID("submit-menu")->removeChild(submitBtn);
        auto loading = LoadingSpinner::create(20);
        loading->setPosition(m_mainLayer->getChildByID("submit-menu")->getContentWidth() / 2, m_mainLayer->getChildByID("submit-menu")->getContentHeight() / 2);
        m_mainLayer->getChildByID("submit-menu")->addChild(loading);
    };

public:
    static requestTag* create(std::vector<std::variant<std::string, bool>> level) {
        auto ret = new requestTag();
        if (ret->initAnchored(400.f, 250.f, level)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    };
};