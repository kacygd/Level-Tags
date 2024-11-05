using namespace geode::prelude;

class tagUtils {
public:
    static CCSprite* getIcon(const std::string& tag) {
        CCSprite* icon = nullptr;
icon = CCSprite::createWithSpriteFrameName("ship");
        return icon;
    };
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
    }
    static IconButtonSprite* addTag(std::string tag) {
        std::transform(tag.begin(), tag.end(), tag.begin(),
               [](unsigned char c){ return std::tolower(c); });

        auto tagNode = IconButtonSprite::create("tagSquare.png"_spr, CCSprite::create(fmt::format("{}"_spr, tag + ".png").c_str()), tag.c_str(), "bigFont.fnt");
        if (tagNode->getIcon()->getScale() == 0.625) tagNode->setIcon(CCSprite::create("404.png"_spr));

        if (std::unordered_set<std::string>{"generic", "tech", "effect", "design", "layout", "modern", "art", "ambience", "animation", "cartoon", "realism", "core", "minigame", "simplism", "atmospheric", "experimental", "standard", "recreation", "neon"}.count(tag))  tagNode->setIcon(CCSprite::create("style.png"_spr));

        tagNode->setAnchorPoint({1, 0.5});
        tagNode->setScale(0.35);
        tagNode->setColor(tagUtils::color(tag));
        tagNode->setOpacity(255);
        if(tagNode->getIcon()) tagNode->getIcon()->setScale(0.5);

        return tagNode;
    }
};