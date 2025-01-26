#include <Geode/utils/web.hpp>
#include <Geode/ui/LoadingSpinner.hpp>
#include "utils.hpp"
#include "betaSearch.hpp"

bool BetaSearch::setup(std::string tag) {
    setTitle("beta search");
    m_title->setScale(1);

    auto includeBG = CCScale9Sprite::create("square02b_001.png");
    includeBG->setContentSize({380, 30});
    includeBG->setPosition({m_mainLayer->getContentWidth() / 2, 210});
    includeBG->setID("include-bg");
    includeBG->setColor({107, 59, 25});
    m_mainLayer->addChild(includeBG);

    auto excludeBG = CCScale9Sprite::create("square02b_001.png");
    excludeBG->setContentSize({380, 30});
    excludeBG->setPosition({m_mainLayer->getContentWidth() / 2, 160});
    excludeBG->setID("exclude-bg");
    excludeBG->setColor({107, 59, 25});
    m_mainLayer->addChild(excludeBG);

    auto includeLabel = CCLabelBMFont::create("Include", "bigFont.fnt");
    includeLabel->setPosition({50, 235});
    includeLabel->setScale(0.5);
    m_mainLayer->addChild(includeLabel);

    auto excludeLabel = CCLabelBMFont::create("Exclude", "bigFont.fnt");
    excludeLabel->setPosition({50, 185});
    excludeLabel->setScale(0.5);
    m_mainLayer->addChild(excludeLabel);

    auto menu = CCMenu::create();
    menu->setPosition({m_mainLayer->getContentWidth() / 2, 0});
    menu->setID("menu");
    menu->setContentSize({400, 270});
    m_mainLayer->addChild(menu);

    auto search = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("GJ_longBtn06_001.png"), this, menu_selector(BetaSearch::search));
    search->setPosition({170, 245});
    search->setID("search");
    menu->addChild(search);

    auto categoryMenu = CCMenu::create();
    categoryMenu->setContentSize({355, 22});
    categoryMenu->setPosition({m_mainLayer->getContentWidth() / 2, 125});
    categoryMenu->setLayout(AxisLayout::create()->setGap(5)->setDefaultScaleLimits(0.3f, 1.f));
    categoryMenu->setID("category-menu");
    m_mainLayer->addChild(categoryMenu);

    auto menuInclude = CCMenu::create();
    menuInclude->setContentSize({380, 30});
    menuInclude->setPosition({m_mainLayer->getContentWidth() / 2, 210});
    menuInclude->setID("menu-include");
    menuInclude->setLayout(AxisLayout::create()->setGap(5)->setDefaultScaleLimits(0.3f, 1.f));
    menuInclude->updateLayout();
    m_mainLayer->addChild(menuInclude);

    auto menuExclude = CCMenu::create();
    menuExclude->setContentSize({380, 30});
    menuExclude->setPosition({m_mainLayer->getContentWidth() / 2, 160});
    menuExclude->setID("menu-exclude");
    menuExclude->setLayout(AxisLayout::create()->setGap(5)->setDefaultScaleLimits(0.3f, 1.f));
    menuExclude->updateLayout();
    m_mainLayer->addChild(menuExclude);

    auto selectBtn = CCMenuItemSpriteExtra::create(ButtonSprite::create(">"), this, menu_selector(BetaSearch::menuChange));
    selectBtn->setPosition({-220, 190});
    menu->addChild(selectBtn);

    auto select = CCSprite::create("select.png"_spr);
    select->setScale(1.185);
    select->setID("select");
    select->setPosition({m_mainLayer->getContentWidth() / 2, 210});
    m_mainLayer->addChild(select);

    auto categoryStyle = CCMenuItemSpriteExtra::create(ButtonSprite::create("Style"), this, menu_selector(BetaSearch::category));
    categoryStyle->setTag(2);
    categoryMenu->addChild(categoryStyle);

    auto categoryTheme = CCMenuItemSpriteExtra::create(ButtonSprite::create("Theme"), this, menu_selector(BetaSearch::category));
    categoryTheme->setTag(3);
    categoryMenu->addChild(categoryTheme);

    auto categoryMeta = CCMenuItemSpriteExtra::create(ButtonSprite::create("Meta"), this, menu_selector(BetaSearch::category));
    categoryMeta->setTag(4);
    categoryMenu->addChild(categoryMeta);

    auto categoryGameplay = CCMenuItemSpriteExtra::create(ButtonSprite::create("Gameplay"), this, menu_selector(BetaSearch::category));
    categoryGameplay->setTag(5);
    categoryMenu->addChild(categoryGameplay);

    categoryMenu->updateLayout();

    menuStyle = createMenu("style");
    m_mainLayer->addChild(menuStyle);
    menuStyle->setVisible(true);

    menuTheme = createMenu("theme");
    m_mainLayer->addChild(menuTheme);

    menuMeta = createMenu("meta");
    m_mainLayer->addChild(menuMeta);

    menuGameplay = createMenu("gameplay");
    m_mainLayer->addChild(menuGameplay);

    auto loadingTags = LoadingSpinner::create(30);
    loadingTags->setPosition({m_mainLayer->getContentWidth() / 2, 95});
    m_mainLayer->addChild(loadingTags);

    m_listener.bind([this, loadingTags](web::WebTask::Event* e) {
        if (auto res = e->getValue(); res && res->ok()) {
            m_mainLayer->removeChild(loadingTags);
            auto jsonStr = res->string().unwrapOr("{}");
            auto json = matjson::parse(jsonStr).unwrapOr("{}");

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
            menuGameplay->setContentWidth(480);
            menuGameplay->setScale(0.85);
            menuGameplay->updateLayout();
        }
    });
    auto req = web::WebRequest();
    m_listener.setFilter(req.get("https://raw.githubusercontent.com/KampWskiR/test3/main/tags.json"));

    return true;
}

CCMenu* BetaSearch::createMenu(std::string name) {
    CCMenu* menu = CCMenu::create();
    menu->setContentSize({380, 100});
    menu->setAnchorPoint({0.5, 1});
    menu->setPosition({m_mainLayer->getContentWidth() / 2, 109});
    menu->setVisible(false);
    menu->setID(fmt::format("menu-{}", name));
    menu->setLayout(AxisLayout::create()->setAutoScale(false)->setGrowCrossAxis(true)->setGap(2));
    menu->updateLayout();
    return menu;
}

void BetaSearch::menuChange(CCObject*) {
    includeSelected = !includeSelected;
    m_mainLayer->getChildByID("select")->setPositionY(includeSelected ? 210 : 160);
}

void BetaSearch::search(CCObject*) {
    if (include.empty()) {
        Notification::create("Add at least one tag in the Include tab.", NotificationIcon::Warning, 1)->show();
        return;
    }
    auto loading = LoadingSpinner::create(20);
    loading->setPosition({170, 245});
    m_mainLayer->getChildByID("menu")->addChild(loading);
    m_mainLayer->getChildByID("menu")->getChildByID("search")->setVisible(false);

    m_listener.bind([this, loading](web::WebTask::Event* e) {
        if (auto res = e->getValue(); res && res->ok()) {
            auto jsonStr = res->string().unwrapOr("{}");
            auto json = matjson::parse(jsonStr).unwrapOr("{}");
            std::string resultLevels;

            if (json.isObject()) {
                for (const auto& [key, value] : json) {
                    std::vector<std::string> tags;

                    if (value.isArray()) {
                        for (const auto& tag : value) {
                            if (tag.isString()) tags.push_back(tag.asString().unwrapOr(""));
                        }
                    }

                    bool includeMatch = include.empty() || std::all_of(include.begin(), include.end(), [&tags](const std::string& inc) {
                        return std::find(tags.begin(), tags.end(), inc) != tags.end();
                    });

                    bool excludeMatch = std::any_of(exclude.begin(), exclude.end(), [&tags](const std::string& exc) {
                        return std::find(tags.begin(), tags.end(), exc) != tags.end();
                    });

                    if (includeMatch && !excludeMatch) resultLevels += key + ",";
                }
                resultLevels = resultLevels.substr(0, resultLevels.size() - 1);

                m_mainLayer->getChildByID("menu")->removeChild(loading);
                m_mainLayer->getChildByID("menu")->getChildByID("search")->setVisible(true);

                if (resultLevels.empty()) {
                    Notification::create("No levels found", NotificationIcon::Error, 1)->show();
                } else {
                    CCDirector::sharedDirector()->pushScene(LevelBrowserLayer::scene(GJSearchObject::create(SearchType::Type19, resultLevels)));
                }
            }
        }
    });

    auto req = web::WebRequest();
    m_listener.setFilter(req.get("https://raw.githubusercontent.com/KampWskiR/test3/main/leveltags.json"));
}


void BetaSearch::category(CCObject* sender) {
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

void BetaSearch::btn(CCObject* sender) {
    CCMenuItemSpriteExtra* clickedButton = static_cast<CCMenuItemSpriteExtra*>(sender);

    if (clickedButton->getParent()->getID() == "menu-include" || clickedButton->getParent()->getID() == "menu-exclude") {
        if (clickedButton->getParent()->getID() == "menu-include") {
            include.erase(std::remove(include.begin(), include.end(), clickedButton->getID()), include.end());
        } else {
            exclude.erase(std::remove(exclude.begin(), exclude.end(), clickedButton->getID()), exclude.end());
        }
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
        if (includeSelected) {
            include.push_back(clickedButton->getID());
        } else {
            exclude.push_back(clickedButton->getID());
        }
        clickedButton->getParent()->removeChild(clickedButton);
        m_mainLayer->getChildByID(includeSelected ? "menu-include" : "menu-exclude")->addChild(clickedButton);
        m_mainLayer->getChildByID(includeSelected ? "menu-include" : "menu-exclude")->updateLayout();
    }
    menuStyle->updateLayout();
    menuTheme->updateLayout();
    menuMeta->updateLayout();
    menuGameplay->updateLayout();
    m_mainLayer->getChildByID("menu-include")->updateLayout();
}

void BetaSearch::addTagsList(int tag, std::vector<std::string> tags) {
    CCMenu* menus[] = {menuStyle, menuTheme, menuMeta, menuGameplay};
    for (int i = 0; i < tags.size(); i++) {
        auto spr = TagUtils::addTag(tags[i].c_str(), 0.5);
        auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(BetaSearch::btn));
        btn->setID(tags[i]);
        btn->setZOrder(tagZ);
        btn->setTag(tag);
        menus[tag-2]->addChild(btn);
        tagZ++;
    }
    menus[tag-2]->updateLayout();
}

BetaSearch* BetaSearch::create(std::string tag) {
    auto ret = new BetaSearch();
    if (ret->initAnchored(400.f, 270.f, tag, "GJ_square01.png")) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}
