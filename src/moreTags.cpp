#include "moreTags.hpp"

bool MoreTags::setup(matjson::Value tags) {
    auto descMenu = CCScale9Sprite::create("square02b_001.png");
    descMenu->setContentSize({240, 105});
    descMenu->setPosition({m_mainLayer->getContentWidth() / 2, 95});
    descMenu->setAnchorPoint({0.5, 1});
    descMenu->setColor({117, 65, 39});
    m_mainLayer->addChild(descMenu);

    auto tagMenu = CCMenu::create();
    tagMenu->setContentWidth(235);
    tagMenu->setPosition({m_mainLayer->getContentWidth() / 2, 95});
    tagMenu->setAnchorPoint({0.5, 1});
    tagMenu->setID("level-tags");
    tagMenu->setLayout(RowLayout::create()->setGrowCrossAxis(true)->setAutoScale(false)->setGap(2));
    m_mainLayer->addChild(tagMenu);

    if (!tags.isNull() && tags.isArray()) {
        for (const auto& tag : tags) {
            auto tagNode = CCMenuItemSpriteExtra::create(TagsManager::addTag(tag.asString().unwrapOr(""), 0.35), this, menu_selector(TagDesc::open));
            tagNode->setID(tag.asString().unwrapOr(""));
            tagMenu->addChild(tagNode);
            tagMenu->updateLayout();
        }
        m_mainLayer->setContentHeight(30.f + tagMenu->getContentHeight());
        m_mainLayer->updateLayout();
        tagMenu->setPositionY(m_mainLayer->getContentHeight() - 15);
        descMenu->setPositionY(m_mainLayer->getContentHeight() - 10);
        descMenu->setContentHeight(tagMenu->getContentHeight() + 10);
    }

    return true;
}

MoreTags* MoreTags::create(matjson::Value tags) {
    auto ret = new MoreTags();
    if (ret->initAnchored(280.f, 20.f, tags, "square.png"_spr)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}