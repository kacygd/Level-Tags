#include "tagDesc.hpp"

bool TagDesc::setup(std::string tag) {
    m_closeBtn->setPosition({m_mainLayer->getContentWidth() / 2, 25});
    m_closeBtn->setSprite(ButtonSprite::create("Ok"));

    auto tagSpr = TagUtils::addTag(tag, 0.75);
    tagSpr->setAnchorPoint({0.5, 0.5});
    tagSpr->setPosition({m_mainLayer->getContentWidth() / 2, 135});
    m_mainLayer->addChild(tagSpr);

    auto descMenu = CCScale9Sprite::create("square02b_001.png");
    descMenu->setContentSize({200, 70});
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

TagDesc* TagDesc::create(std::string tag) {
    auto ret = new TagDesc();
    if (ret->initAnchored(240.f, 160.f, tag, "GJ_square01.png")) {
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
