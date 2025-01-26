#include "utils.hpp"

ccColor3B TagUtils::color(const std::string& tag) {
    std::hash<std::string> hasher;
    size_t hashValue = hasher(tag);

    GLubyte r = (hashValue >> 16) & 0xFF;
    GLubyte g = (hashValue >> 8) & 0xFF;
    GLubyte b = hashValue & 0xFF;

    if (r < 100) r = 100;
    if (g < 100) g = 100;
    if (b < 100) b = 100;

    return { r, g, b };
}

IconButtonSprite* TagUtils::addTag(std::string tag, float scale) {
    auto tagNode = IconButtonSprite::create(
        "tagSquare.png"_spr, 
        CCSprite::createWithSpriteFrameName(fmt::format("{}"_spr, tag + ".png").c_str()), 
        tag.c_str(), "bigFont.fnt"
    );
    if (!tagNode->getIcon() || tagNode->getIcon()->getScale() == 0.625) tagNode->setIcon(CCSprite::create("404.png"_spr));
    if (std::unordered_set<std::string>{
        "generic", "tech", "effect", "design", "layout", "modern", "art", "ambience", "animation", 
        "cartoon", "realism", "core", "minigame", "simplism", "atmospheric", "experimental", 
        "standard", "recreation", "sunset"
    }.count(tag)) {
        tagNode->setIcon(CCSprite::createWithSpriteFrameName("style.png"_spr));
    }
    tagNode->setAnchorPoint({ 1, 0.5 });
    tagNode->setScale(scale);
    tagNode->setColor(TagUtils::color(tag));
    tagNode->setOpacity(255);
    if (tagNode->getIcon()) tagNode->getIcon()->setScale(0.5);

    return tagNode;
}
