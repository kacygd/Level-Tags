#include "tagDesc.hpp"

bool TagDesc::setup(std::string tag) {
    auto iconBg = CCScale9Sprite::create("square02b_001.png");
    iconBg->setContentSize({70, 70});
    iconBg->setPosition({50, m_mainLayer->getContentHeight() / 2});
    iconBg->setColor({0, 0, 0});
    iconBg->setOpacity(60);
    m_mainLayer->addChild(iconBg);

    auto descMenu = CCScale9Sprite::create("square02b_001.png");
    descMenu->setContentSize({200, 70});
    descMenu->setPosition({190, m_mainLayer->getContentHeight() / 2});
    descMenu->setColor({0, 0, 0});
    descMenu->setOpacity(60);
    m_mainLayer->addChild(descMenu);

    auto tagLabel = CCLabelBMFont::create(tag.c_str(), "bigFont.fnt");
    tagLabel->limitLabelWidth(50, 0.5, 0.2);
    tagLabel->setPosition({50, 35});
    tagLabel->setColor(TagsManager::color(tag));
    m_mainLayer->addChild(tagLabel);

    auto tagIcon = CCSprite::createWithSpriteFrameName(fmt::format("{}"_spr, tag + ".png").c_str());
    if (!tagIcon || tagIcon->getUserObject("geode.texture-loader/fallback")) tagIcon = CCSprite::create("404.png"_spr);
    tagIcon->setColor(TagsManager::color(tag));
    tagIcon->setPosition({50, 58});
    if (tagIcon) tagIcon->setScale(0.5);
    m_mainLayer->addChild(tagIcon);

    if (TagsManager::sharedState()->cachedDescs.size() != 0) {
        createDesc(tag, TagsManager::sharedState()->cachedDescs);
        log::info("{}", TagsManager::sharedState()->cachedDescs.dump());
        return true;
    }

    auto loading = LoadingSpinner::create(20);
    loading->setPosition({190, m_mainLayer->getContentHeight() / 2});
    m_mainLayer->addChild(loading);

    m_listener.bind([this, loading, tag](web::WebTask::Event* e) {
        if (auto res = e->getValue(); res && res->ok()) {
            m_mainLayer->removeChild(loading);
            auto jsonStr = res->string().unwrapOr("{}");
            auto json = matjson::parse(jsonStr).unwrapOr("{}");
            TagsManager::sharedState()->cachedDescs = json;
            createDesc(tag, json);
        }
    });
    auto req = web::WebRequest();
    m_listener.setFilter(req.get("https://cps.ps.fhgdps.com/v1/tagDesc.json"));

    return true;
}

void TagDesc::createDesc(std::string tag, matjson::Value descs) {
    auto result = descs[tag].as<std::string>().unwrapOr("");
    if (result == "") result = "no description provided";

    auto desc = SimpleTextArea::create(result.c_str(), "bigFont.fnt", 0.25, 180);
    desc->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
    desc->setPosition({190, m_mainLayer->getContentHeight() / 2});
    desc->setColor({TagsManager::color(tag).r, TagsManager::color(tag).g, TagsManager::color(tag).b, 255});
    m_mainLayer->addChild(desc);
}

TagDesc* TagDesc::create(std::string tag) {
    auto ret = new TagDesc();
    if (ret->initAnchored(300.f, 100.f, tag, "square.png"_spr)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void TagDesc::open(CCObject* sender) {
    CCMenuItemSpriteExtra* btn = static_cast<CCMenuItemSpriteExtra*>(sender);
    create(btn->getID().c_str())->show();
}
