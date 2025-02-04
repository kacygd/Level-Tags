#pragma once

#include <Geode/utils/web.hpp>

using namespace geode::prelude;

class TagInfo : public geode::Popup<std::string> {
private:
    EventListener<web::WebTask> m_listener;
    CCMenu* menuStyle;
    CCMenu* menuTheme;
    CCMenu* menuMeta;
    CCMenu* menuGameplay;
    CCMenuItemSpriteExtra* includeBG;
    CCMenuItemSpriteExtra* excludeBG;
    bool uncompletedSelected = false;
    std::vector<std::string> include;
    std::vector<std::string> exclude;
    int tagZ = 0;

    CCMenu* createMenu(std::string name);
    void tagMenuChange(CCObject* sender);
    void menuChange(CCObject* sender);
    void category(CCObject* sender);
    void btn(CCObject* sender);
    void addTagsList(int tag, std::vector<std::string> tags);

protected:
    bool setup(std::string tag);

public:
    static TagInfo* create(std::string tag);
};