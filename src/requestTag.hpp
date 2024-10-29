class submit : public geode::Popup<bool> {
protected:
    bool setup(bool success) {
        auto winSize = CCDirector::sharedDirector()->getWinSize();
    
        m_bgSprite->setContentSize({200,100});
        m_closeBtn->setPosition({-100,40});

        auto successIcon = CCSprite::createWithSpriteFrameName(success ? "GJ_completesIcon_001.png" : "GJ_deleteIcon_001.png");
        successIcon->setScale(1.5);
        successIcon->setPosition({m_mainLayer->getContentWidth() / 2, m_mainLayer->getContentHeight() / 1.9f});
        m_mainLayer->addChild(successIcon);

        auto successLabel = CCLabelBMFont::create(success ? "Tags Request submitted!" : "Tags Request failed", "chatFont.fnt");
        successLabel->setScale(0.75);
        successLabel->setPosition({m_mainLayer->getContentWidth() / 2, m_mainLayer->getContentHeight() / 2.3f});
        m_mainLayer->addChild(successLabel);

        return true;
    }
public:
    static submit* create(bool success) {
        auto ret = new submit();
        if (ret->init(240.f, 160.f, success, "GJ_square02.png")) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    };
};

class requestTag : public geode::Popup<int> {
protected:
    InputNode* inputNode;
    CCLabelBMFont* message;
    EventListener<web::WebTask> m_listener;
    int m_level;

    void submit(cocos2d::CCObject* sender) {
        if (inputNode->getString().empty()) {
            message->setString("Please provide level tags");
            message->setOpacity(255);
            message->setScale(0.75);
            message->setColor({150,0,0});
            return;
        }

        m_listener.bind([this](web::WebTask::Event* e) {
            if (auto res = e->getValue()) {
                    this->onClose(this);
                    submit::create(res->ok() || res->code() == 405)->show();
            }
        });

        auto req = web::WebRequest();
        req.bodyString(fmt::format("id={}&tags={}", m_level, inputNode->getString()));
        m_listener.setFilter(
            req.post(fmt::format("https://script.google.com/macros/s/AKfycbyDbwwXM6vDzR9832XUHbqJ1quZR6O87aAxrMjHdf0THPg3SCp6faVVKy_EANQ1_TmG/exec"))
        );

        auto loading = LoadingSpinner::create(40);
        loading->setPosition({m_mainLayer->getContentWidth() / 2, m_mainLayer->getContentHeight() / 2.0f});
        m_mainLayer->addChild(loading);

        m_mainLayer->removeChild(m_title);
        m_mainLayer->removeChild(m_closeBtn);
        m_mainLayer->removeChild(m_mainLayer->getChildByID("input-menu"));
        m_mainLayer->removeChild(message);
    };

    bool setup(int levelID) {
        CCSize winSize = CCDirector::get()->getWinSize();
        m_level = levelID;

        this->setTitle("Request Tags");
        m_title->setScale(1.25);
        m_title->setPositionY(m_mainLayer->getContentHeight() / 1.55);

        m_bgSprite->setContentSize({300,150});
        m_closeBtn->setPosition({-150,75});

        auto inputMenu = CCMenu::create();
        inputMenu->setPosition({0,0});
        inputMenu->setID("input-menu");
        inputMenu->setContentSize({285,40});
        m_mainLayer->addChild(inputMenu);

        inputNode = InputNode::create(250, "tag1, tag2, tag3");
        inputNode->setPosition({winSize.width / 2,m_mainLayer->getContentHeight() / 1.9f});
        inputNode->setID("tag-input");
        inputMenu->addChild(inputNode);

        auto submit = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Submit"), this, menu_selector(requestTag::submit)
        );
        submit->setPosition({winSize.width / 2, m_mainLayer->getContentHeight() / 2.4f});
        inputMenu->addChild(submit);

        message = CCLabelBMFont::create("Tags are given by level style, deco and gameplay style \n Example tags: Glow, Effect, NONG, Flow, Fast Paced", "chatFont.fnt");
        message->setScale(0.5);
        message->setColor({0,0,0});
        message->setOpacity(100);
        message->setID("message");
        message->setPosition({winSize.width / 2, m_mainLayer->getContentHeight() / 3.0f});
        m_mainLayer->addChild(message);

        return true;
    };
public:
    static requestTag* create(int levelID) {
        auto ret = new requestTag();
        if (ret->init(240.f, 160.f, levelID)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    };
};





// class requestTag : public geode::Popup<int> {
// protected:
//     InputNode* inputNode;
//     CCMenu* menuStyle;
//     CCMenu* menuMeta;
//     CCMenu* menuGameplay;
//     CCLabelBMFont* message;
//     std::vector<std::string> tags;
//     EventListener<web::WebTask> m_listener;
//     int m_level;

//     void submit(cocos2d::CCObject* sender) {
//         if (inputNode->getString().empty()) {
//             message->setString("Please provide level tags");
//             message->setOpacity(255);
//             message->setScale(0.75);
//             message->setColor({150,0,0});
//             return;
//         }

//         m_listener.bind([this](web::WebTask::Event* e) {
//             if (auto res = e->getValue()) {
//                     this->onClose(this);
//                     submit::create(res->ok() || res->code() == 405)->show();
//             }
//         });

//         auto req = web::WebRequest();
//         req.bodyString(fmt::format("id={}&tags={}", m_level, inputNode->getString()));
//         m_listener.setFilter(
//             req.post(fmt::format("https://script.google.com/macros/s/AKfycbyDbwwXM6vDzR9832XUHbqJ1quZR6O87aAxrMjHdf0THPg3SCp6faVVKy_EANQ1_TmG/exec"))
//         );

//         auto loading = LoadingSpinner::create(40);
//         loading->setPosition({285,160});
//         m_mainLayer->addChild(loading);

//         m_mainLayer->removeChild(m_title);
//         m_mainLayer->removeChild(m_closeBtn);
//         m_mainLayer->removeChild(m_mainLayer->getChildByID("input-menu"));
//         m_mainLayer->removeChild(message);
//     };

//     bool setup(int levelID) {
//         m_level = levelID;
//         auto winSize = CCDirector::sharedDirector()->getWinSize();
//         this->setTitle("Request Tags");
//         m_title->setScale(1.25);
//         m_title->setPositionY(260);

//         m_bgSprite->setContentSize({400,250});
//         m_closeBtn->setPosition({-200,125});

//         auto s = CCScale9Sprite::create("square02_001.png");
//         s->setContentSize({350,30});
//         s->setPosition({285,225});
//         s->setOpacity(100);
//         m_mainLayer->addChild(s);

//         auto menu = CCMenu::create();
//         menu->setContentSize({250,30});
//         menu->setPosition({238,225});
//         menu->setID("menu"); 
//         menu->setLayout(AxisLayout::create()->setGap(5));
//         menu->updateLayout();
//         m_mainLayer->addChild(menu);

//         auto b = CCScale9Sprite::create("square02_001.png");
//         b->setContentSize({350,150});
//         b->setPosition({285,125});
//         b->setOpacity(100);
//         m_mainLayer->addChild(b);
        
//         menuStyle = CCMenu::create();
//         menuStyle->setContentSize({340,100});
//         menuStyle->setPosition({285,125});
//         menuStyle->setID("menu-style");
//         menuStyle->setLayout(AxisLayout::create()->setAutoScale(false)->setGrowCrossAxis(true));
//         menuStyle->updateLayout();

//         menuMeta = CCMenu::create();
//         menuMeta->setContentSize({340,100});
//         menuMeta->setPosition({285,125});
//         menuMeta->setID("menu-meta");
//         menuMeta->setLayout(AxisLayout::create()->setAutoScale(false)->setGrowCrossAxis(true));
//         menuMeta->updateLayout();

//         menuGameplay = CCMenu::create();
//         menuGameplay->setContentSize({340,100});
//         menuGameplay->setPosition({285,125});
//         menuGameplay->setID("menu-gameplay");
//         menuGameplay->setLayout(AxisLayout::create()->setAutoScale(false)->setGrowCrossAxis(true));
//         menuGameplay->updateLayout();

        

//         auto submitMenu = CCMenu::create();
//         submitMenu->setContentSize({340,100});
//         submitMenu->setPosition({285,120});
//         submitMenu->setID("submit-menu");
//         m_mainLayer->addChild(submitMenu);

//         auto submitSpr = ButtonSprite::create("Submit");
//         submitSpr->setScale(0.85);
//         auto submit = CCMenuItemSpriteExtra::create(
//             submitSpr, this, menu_selector(requestTag::submitTags) 
//         );
//         submit->setPosition({126,105});
//         submitMenu->addChild(submit);

//         auto categoryCustom = CCMenuItemSpriteExtra::create(
//             ButtonSprite::create("  Custom"), this, menu_selector(requestTag::category) 
//         );
//         categoryCustom->m_baseScale = 0.75;
//         categoryCustom->setScale(0.75);
//         categoryCustom->setPosition({-122,66});
//         categoryCustom->setTag(1);
//         submitMenu->addChild(categoryCustom);

//         auto customSpr = CCSprite::createWithSpriteFrameName("edit_addCBtn_001.png");
//         customSpr->setPosition({17,15});
//         customSpr->setScale(0.6);
//         categoryCustom->addChild(customSpr);

//         auto categoryStyle = CCMenuItemSpriteExtra::create(
//             ButtonSprite::create("Style"), this, menu_selector(requestTag::category) 
//         );
//         categoryStyle->m_baseScale = 0.75;
//         categoryStyle->setScale(0.75);
//         categoryStyle->setPosition({-33,66});
//         categoryStyle->setTag(2);
//         submitMenu->addChild(categoryStyle);

//         auto categoryMeta = CCMenuItemSpriteExtra::create(
//             ButtonSprite::create("Meta"), this, menu_selector(requestTag::category) 
//         );
//         categoryMeta->m_baseScale = 0.75;
//         categoryMeta->setScale(0.75);
//         categoryMeta->setPosition({35,66});
//         categoryMeta->setTag(3);
//         submitMenu->addChild(categoryMeta);

//         auto categoryGameplay = CCMenuItemSpriteExtra::create(
//             ButtonSprite::create("Gameplay"), this, menu_selector(requestTag::category) 
//         );
//         categoryGameplay->m_baseScale = 0.75;
//         categoryGameplay->setScale(0.75);
//         categoryGameplay->setPosition({121,66});
//         categoryGameplay->setTag(4);
//         submitMenu->addChild(categoryGameplay);

//         const char* styleTags[] = {"Generic","Glow","Tech","Space","Hell","Modern","Art","1.0","Ambience","Animation","Cartoon","Pixel","Realism","Core","Minigame","Simplism","Atmospheric"};
//         for (int i = 0; i < (sizeof(styleTags) / sizeof(styleTags[0])); i++) {
//             auto spr = ButtonSprite::create(styleTags[i], "bigFont.fnt", "GJ_button_05.png");
//             spr->setScale(0.5);
//             auto t = CCMenuItemSpriteExtra::create(
//                 spr, this, menu_selector(requestTag::btn) 
//             );
//             t->setID(styleTags[i]);
//             menuStyle->addChild(t);
//         };

//         m_mainLayer->addChild(menuStyle);
//         menuStyle->updateLayout();

//         const char* metaTags[] = {"cube","ship","jetpack","robot"};
//         for (int i = 0; i < (sizeof(metaTags) / sizeof(metaTags[0])); i++) {
//             auto spr = ButtonSprite::create(metaTags[i], "bigFont.fnt", "GJ_button_05.png");
//             spr->setScale(0.5);
//             auto t = CCMenuItemSpriteExtra::create(
//                 spr, this, menu_selector(requestTag::btn) 
//             );
//             t->setID(metaTags[i]);
//             menuMeta->addChild(t);
//         };

//         m_mainLayer->addChild(menuMeta);
//         menuMeta->updateLayout();

//         return true;
//     };
//     void category(CCObject* sender) {
//         CCMenuItemSpriteExtra* category = static_cast<CCMenuItemSpriteExtra*>(sender);
        
//         switch (category->getTag()) {
//             case 1:
//                 log::info("a");
//             break;
//             case 2:
//                 log::info("b");
//             break;
//             case 3:
//                 log::info("c");
//             break;
//             case 4:
//                 log::info("d");
//             break;
//         };
//     };

//     void btn(CCObject* sender) {
//         CCMenuItemSpriteExtra* clickedButton = static_cast<CCMenuItemSpriteExtra*>(sender);

//         if (clickedButton->getParent()->getID() == "menu") {
//             tags.erase(std::remove(tags.begin(), tags.end(), clickedButton->getID()), tags.end());
//             clickedButton->getParent()->removeChild(clickedButton);
//             clickedButton->m_baseScale = 1;
//             clickedButton->setScale(1);
//             switch (clickedButton->getTag()) {
//                 case 1:
//                 break;
//                 case 2:
//                 break;
//                 case 3:
//                 break;
//                 case 4:
//                 break;
//             }
//             if (auto layer = m_mainLayer->getChildByID("menu-style")) layer->addChild(clickedButton);
//             m_mainLayer->getChildByID("menu-style")->updateLayout();
//         } else {
//             tags.push_back(clickedButton->getID());
//             clickedButton->getParent()->removeChild(clickedButton);
//             m_mainLayer->getChildByID("menu")->addChild(clickedButton);
//             m_mainLayer->getChildByID("menu")->updateLayout();
//         };
//         menuStyle->updateLayout();
//         m_mainLayer->getChildByID("menu")->updateLayout();
//     };

// void submitTags(CCObject* sender) {
//     std::string tagsRequest;
//     for (const auto& tag : tags) tagsRequest += fmt::format("{}, ", tag);
//     m_listener.bind([this](web::WebTask::Event* e) {
//         if (auto res = e->getValue()) {
//                 this->onClose(this);
//                 submit::create(res->ok() || res->code() == 405)->show();
//         }
//     });

//     auto req = web::WebRequest();
//     req.bodyString(fmt::format("id={}&tags={}", m_level, tagsRequest));
//     m_listener.setFilter(
//         req.post(fmt::format("https://script.google.com/macros/s/AKfycbyDbwwXM6vDzR9832XUHbqJ1quZR6O87aAxrMjHdf0THPg3SCp6faVVKy_EANQ1_TmG/exec"))
//     );
// };

// public:
//     static requestTag* create(int levelID) {
//         auto ret = new requestTag();
//         if (ret->init(240.f, 160.f, levelID)) {
//             ret->autorelease();
//             return ret;
//         }
//         delete ret;
//         return nullptr;
//     };
// };