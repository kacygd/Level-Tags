using namespace geode::prelude;

class tagUtils {
public:
    static ccColor3B color(const std::string& tag) {
        std::hash<std::string> hasher;
        size_t hashValue = hasher(tag);

        GLubyte r = (hashValue >> 16) & 0xFF;
        GLubyte g = (hashValue >> 8) & 0xFF;
        GLubyte b = hashValue & 0xFF;

        if (r < 100) r = 100;
        if (g < 100) g = 100;
        if (b < 100) b = 100;

        return {r,g,b};
    };

    static IconButtonSprite* addTag(std::string tag, float scale) {
        auto tagNode = IconButtonSprite::create("tagSquare.png"_spr, CCSprite::createWithSpriteFrameName(fmt::format("{}"_spr, tag + ".png").c_str()), tag.c_str(), "bigFont.fnt");
        if (!tagNode->getIcon() || tagNode->getIcon()->getScale() == 0.625) tagNode->setIcon(CCSprite::create("404.png"_spr));

        if (std::unordered_set<std::string>{
            "generic", "tech", "effect", "design", "layout", "modern", "art", "ambience", "animation", "cartoon",
            "realism", "core", "minigame", "simplism", "atmospheric", "experimental", "standard", "recreation", "sunset"
        }.count(tag))  tagNode->setIcon(CCSprite::createWithSpriteFrameName("style.png"_spr));

        tagNode->setAnchorPoint({1, 0.5});
        tagNode->setScale(scale);
        tagNode->setColor(tagUtils::color(tag));
        tagNode->setOpacity(255);
        
        if(tagNode->getIcon()) tagNode->getIcon()->setScale(0.5);

        return tagNode;
    }
};

class tagDesc : public geode::Popup<std::string> {
    EventListener<web::WebTask> m_listener;
protected:
    bool setup(std::string tag) {
        m_closeBtn->setPosition({m_mainLayer->getContentWidth()/2,25});
        m_closeBtn->setSprite(ButtonSprite::create("Ok"));

        auto tagSpr = tagUtils::addTag(tag, 0.75);
        tagSpr->setAnchorPoint({0.5, 0.5});
        tagSpr->setPosition({m_mainLayer->getContentWidth() / 2,135});
        m_mainLayer->addChild(tagSpr);

        auto descMenu = CCScale9Sprite::create("square02b_001.png");
        descMenu->setContentSize({200,70});
        descMenu->setPosition({m_mainLayer->getContentWidth() / 2, 80});
        descMenu->setColor({0, 0, 0});
        descMenu->setOpacity(60);
        m_mainLayer->addChild(descMenu);

        auto loading = LoadingSpinner::create(20);
        loading->setPosition({m_mainLayer->getContentWidth() / 2, 80});
        m_mainLayer->addChild(loading);

        m_listener.bind([this, loading, tag](web::WebTask::Event* e) {
            if (auto res = e->getValue(); res && res->ok()) {
                m_mainLayer->removeChild(loading);
                auto jsonStr = res->string().unwrapOr("{}");
                auto json = matjson::parse(jsonStr).unwrapOr("{}");
                auto result = json[tag].as<std::string>().unwrapOr("");
                if (result == "") result = "no description provided";
                
                auto desc = SimpleTextArea::create(result.c_str(), "bigFont.fnt", 0.25, 180);
                desc->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
                desc->setPosition({m_mainLayer->getContentWidth() / 2, 80});
                m_mainLayer->addChild(desc);
            }
        });
        auto req = web::WebRequest();
        m_listener.setFilter(req.get("https://raw.githubusercontent.com/KampWskiR/test3/main/tagDesc.json"));

        return true;
    }
public:
    static tagDesc* create(std::string tag) {
        auto ret = new tagDesc();
        if (ret->initAnchored(240.f, 160.f, tag, "GJ_square01.png")) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    };
};