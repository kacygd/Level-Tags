class submit : public geode::Popup<bool> {
protected:
    bool setup(bool success) {
        auto winSize = CCDirector::sharedDirector()->getWinSize();
    
        m_bgSprite->setContentSize({200,100});
        m_closeBtn->setPosition({-100,40});

        auto successIcon = CCSprite::createWithSpriteFrameName(success ? "GJ_completesIcon_001.png" : "GJ_deleteIcon_001.png");
        successIcon->setScale(1.5);
        successIcon->setPosition({285, 170});
        m_mainLayer->addChild(successIcon);

        auto successLabel = CCLabelBMFont::create(success ? "Tags Request submitted!" : "Tags Request failed", "chatFont.fnt");
        successLabel->setScale(0.75);
        successLabel->setPosition({285, 140});
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
        loading->setPosition({285,160});
        m_mainLayer->addChild(loading);

        m_mainLayer->removeChild(m_title);
        m_mainLayer->removeChild(m_closeBtn);
        m_mainLayer->removeChild(m_mainLayer->getChildByID("input-menu"));
        m_mainLayer->removeChild(message);
    };

    bool setup(int levelID) {
        m_level = levelID;
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        this->setTitle("Request Tags");
        m_title->setScale(1.25);
        m_title->setPositionY(210);

        m_bgSprite->setContentSize({300,150});
        m_closeBtn->setPosition({-150,75});

        auto inputMenu = CCMenu::create();
        inputMenu->setPosition({145,150});
        inputMenu->setID("input-menu");
        inputMenu->setContentSize({285,40});
        m_mainLayer->addChild(inputMenu);

        inputNode = InputNode::create(250, "tag1, tag2, tag3");
        inputNode->setPosition({135,20});
        inputNode->setID("tag-input");
        inputMenu->addChild(inputNode);

        auto submit = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Submit"), this, menu_selector(requestTag::submit)
        );
        submit->setPosition({140,-15});
        inputMenu->addChild(submit);

        message = CCLabelBMFont::create("Tags are given by level style, deco and gameplay style \n Example tags: Glow, Effect, NONG, Flow, Fast Paced", "chatFont.fnt");
        message->setScale(0.5);
        message->setColor({0,0,0});
        message->setOpacity(100);
        message->setID("message");
        message->setPosition({285,105});
        m_mainLayer->addChild(message);

        // auto s = CCScale9Sprite::create("square02_001.png");
        // s->setContentSize({350,30});
        // s->setPosition({285,225});
        // s->setOpacity(100);
        // m_mainLayer->addChild(s);

        // auto d = CCMenu::create();
        // d->setContentSize({350,30});
        // d->setPosition({285,225});
        // d->setID("menu"); 
        // d->setLayout(AxisLayout::create()->setAutoScale(true)->setGrowCrossAxis(true));
        // d->updateLayout();
        // m_mainLayer->addChild(d);

        // auto b = CCScale9Sprite::create("square02_001.png");
        // b->setContentSize({350,150});
        // b->setPosition({285,125});
        // b->setOpacity(100);
        // m_mainLayer->addChild(b);

        // auto scroll = ScrollLayer::create({352,150});
        // scroll->setPosition({109,50});
        // m_mainLayer->addChild(scroll);
        // scroll->m_contentLayer->setLayout(
        //     ColumnLayout::create()
        //         ->setAutoGrowAxis(195.0f)
        //         ->setGap(5)
        //         ->setAxis(Axis::Row)
        // );
        
        // auto a = CCMenu::create();
        // a->setContentSize({350,100});
        // a->setPosition({176,175});
        // a->setID("menu2");
        // a->setZOrder(100);
        // a->setTouchEnabled(true);
        // a->setLayout(AxisLayout::create()->setAutoScale(false)->setGrowCrossAxis(true));
        // a->updateLayout();

        // const char* objectt[] = {"Generic","Glow","Tech","Space","Hell","Modern","Art","1.0","Ambience","Animation","Cartoon","Pixel","Realism","Core","Minigame","Simplism","Atmospheric"};
        // for (int i = 0; i < (sizeof(objectt) / sizeof(objectt[0])); i++) {
        //     auto t = CCMenuItemSpriteExtra::create(
        //         ButtonSprite::create(objectt[i]), this, menu_selector(requestTag::btn) 
        //     );
        //     t->setID(objectt[i]);
        //     a->addChild(t);
        // };

        // scroll->getChildByID("content-layer")->addChild(a);
        // a->updateLayout();
        // scroll->m_contentLayer->updateLayout();

        return true;
    };
    // void btn(CCObject* sender) {
    //     CCMenuItemSpriteExtra* clickedButton = static_cast<CCMenuItemSpriteExtra*>(sender);
    //     std::string buttonTag = std::string(clickedButton->getID());
    //     if (clickedButton->getParent()->getID() == "menu") {
    //         clickedButton->getParent()->removeChild(clickedButton);
    //         if (auto layer = m_mainLayer->getChildByID("ScrollLayer")->getChildByID("content-layer")->getChildByID("menu2")) layer->addChild(clickedButton);
    //         m_mainLayer->getChildByID("ScrollLayer")->getChildByID("content-layer")->getChildByID("menu2")->updateLayout();
    //     } else {
    //         clickedButton->getParent()->removeChild(clickedButton);
    //         m_mainLayer->getChildByID("menu")->addChild(clickedButton);
    //         m_mainLayer->getChildByID("menu")->updateLayout();
    //     };
    // };
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