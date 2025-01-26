#pragma once

#include <Geode/utils/web.hpp>

using namespace geode::prelude;

class BetaSearch : public geode::Popup<std::string> {
private:
    EventListener<web::WebTask> m_listener;
    bool includeSelected = true;
    CCMenu* menuStyle;
    CCMenu* menuTheme;
    CCMenu* menuMeta;
    CCMenu* menuGameplay;
    std::vector<std::string> include;
    std::vector<std::string> exclude;
    int tagZ = 0;

    CCMenu* createMenu(std::string name);
    void menuChange(CCObject* sender);
    void search(CCObject* sender);
    void category(CCObject* sender);
    void btn(CCObject* sender);
    void addTagsList(int tag, std::vector<std::string> tags);

protected:
    bool setup(std::string tag);

public:
    static BetaSearch* create(std::string tag);
};