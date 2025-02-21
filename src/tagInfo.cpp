#include "tagInfo.hpp"

bool TagInfo::setup(std::string jsonStr) {
    m_closeBtn->setPosition({m_mainLayer->getContentWidth() / 2, 23});
    m_closeBtn->setSprite(ButtonSprite::create("Close"));

    auto tagsBG = CCScale9Sprite::create("square02b_001.png");
    tagsBG->setContentSize({330, 180});
    tagsBG->setPosition({m_mainLayer->getContentWidth() / 2, 130});
    tagsBG->setColor({ 0, 0, 0 });
    tagsBG->setOpacity(100);
    m_mainLayer->addChild(tagsBG);

    auto categoryMenu = CCMenu::create();
    categoryMenu->setContentSize({330, 22});
    categoryMenu->setPosition({m_mainLayer->getContentWidth() / 2, 205});
    categoryMenu->setLayout(AxisLayout::create()->setGap(5));
    categoryMenu->setID("category-menu");
    m_mainLayer->addChild(categoryMenu);

    auto styleBtn = CCSprite::create("styleBtn.png"_spr);
    styleBtn->setScale(0.65);
    auto categoryStyle = CCMenuItemSpriteExtra::create(styleBtn, this, menu_selector(TagInfo::category));
    categoryStyle->setTag(2);
    categoryMenu->addChild(categoryStyle);

    auto themeBtn = CCSprite::create("themeBtn.png"_spr);
    themeBtn->setScale(0.65);
    auto categoryTheme = CCMenuItemSpriteExtra::create(themeBtn, this, menu_selector(TagInfo::category));
    categoryTheme->setTag(3);
    categoryMenu->addChild(categoryTheme);

    auto metaBtn = CCSprite::create("metaBtn.png"_spr);
    metaBtn->setScale(0.65);
    auto categoryMeta = CCMenuItemSpriteExtra::create(metaBtn, this, menu_selector(TagInfo::category));
    categoryMeta->setTag(4);
    categoryMenu->addChild(categoryMeta);

    auto gameplayBtn = CCSprite::create("gameplayBtn.png"_spr);
    gameplayBtn->setScale(0.65);
    auto categoryGameplay = CCMenuItemSpriteExtra::create(gameplayBtn, this, menu_selector(TagInfo::category));
    categoryGameplay->setTag(5);
    categoryMenu->addChild(categoryGameplay);

    categoryMenu->updateLayout();

    menuStyle = createMenu("style");
    m_mainLayer->addChild(menuStyle);
    menuStyle->setVisible(true);

    menuTheme = createMenu("theme");
    menuTheme->setContentWidth(380);
    menuTheme->setScale(0.85);
    m_mainLayer->addChild(menuTheme);

    menuMeta = createMenu("meta");
    m_mainLayer->addChild(menuMeta);

    menuGameplay = createMenu("gameplay");
    menuGameplay->setContentWidth(380);
    menuGameplay->setScale(0.85);
    m_mainLayer->addChild(menuGameplay);

    auto json = matjson::parse(jsonStr).unwrapOr("{}");

    if (!json.contains("style") || !json.contains("theme") || !json.contains("classic-meta") || !json.contains("plat-meta") || !json.contains("classic-gp") || !json.contains("plat-gp")) {
        Notification::create("Failed to retrieve tags from the server.", NotificationIcon::Error, 2)->show();
        return true;
    }

    std::vector<std::string> meta = json["classic-meta"].as<std::vector<std::string>>().unwrap();
    std::unordered_set<std::string> metab(meta.begin(), meta.end());

    for (const auto& value2 : json["plat-meta"]) {
        std::string val = value2.asString().unwrapOr("");
        if (metab.insert(val).second) meta.push_back(val);
    }

    std::vector<std::string> gameplay = json["classic-gp"].as<std::vector<std::string>>().unwrap();
    std::unordered_set<std::string> gameplayb(gameplay.begin(), gameplay.end());

    for (const auto& value2 : json["plat-gp"]) {
        std::string val = value2.asString().unwrapOr("");
        if (gameplayb.insert(val).second) {
            gameplay.push_back(val);
        }
    }

    addTagsList(2, json["style"].as<std::vector<std::string>>().unwrap());
    addTagsList(3, json["theme"].as<std::vector<std::string>>().unwrap());
    addTagsList(4, meta);
    addTagsList(5, gameplay);
    menuGameplay->updateLayout();

    return true;
}

CCMenu* TagInfo::createMenu(std::string name) {
    CCMenu* menu = CCMenu::create();
    menu->setContentSize({325, 100});
    menu->setAnchorPoint({0.5, 1});
    menu->setPosition({m_mainLayer->getContentWidth() / 2, 190});
    menu->setVisible(false);
    menu->setID(fmt::format("menu-{}", name));
    menu->setLayout(AxisLayout::create()->setAutoScale(false)->setGrowCrossAxis(true)->setGap(2));
    menu->updateLayout();
    return menu;
}

void TagInfo::category(CCObject* sender) {
    CCMenuItemSpriteExtra* category = static_cast<CCMenuItemSpriteExtra*>(sender);
    menuStyle->setVisible(false);
    menuTheme->setVisible(false);
    menuMeta->setVisible(false);
    menuGameplay->setVisible(false);

    switch (category->getTag()) {
        case 2: menuStyle->setVisible(true); break;
        case 3: menuTheme->setVisible(true); break;
        case 4: menuMeta->setVisible(true); break;
        case 5: menuGameplay->setVisible(true); break;
    }
}

void TagInfo::btn(CCObject* sender) {
    CCMenuItemSpriteExtra* clickedButton = static_cast<CCMenuItemSpriteExtra*>(sender);

    TagDesc::create(clickedButton->getID())->show();
}

void TagInfo::addTagsList(int tag, std::vector<std::string> tags) {
    CCMenu* menus[] = {menuStyle, menuTheme, menuMeta, menuGameplay};
    for (int i = 0; i < tags.size(); i++) {
        auto spr = TagsManager::addTag(tags[i].c_str(), 0.5);
        auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(TagInfo::btn));
        btn->setID(tags[i]);
        btn->setZOrder(tagZ);
        btn->setTag(tag);
        menus[tag-2]->addChild(btn);
        tagZ++;
    }
    menus[tag-2]->updateLayout();
}

TagInfo* TagInfo::create(std::string jsonStr) {
    auto ret = new TagInfo();
    if (ret->initAnchored(350.f, 230.f, jsonStr, "square.png"_spr)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}
