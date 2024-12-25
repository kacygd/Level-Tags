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

    static IconButtonSprite* addTag(std::string tag) {
        if (tag.empty()) return nullptr;
        std::transform(tag.begin(), tag.end(), tag.begin(), [](unsigned char c){ return std::tolower(c); });
               
        auto tagNode = IconButtonSprite::create("tagSquare.png"_spr, CCSprite::createWithSpriteFrameName(fmt::format("{}"_spr, tag + ".png").c_str()), tag.c_str(), "bigFont.fnt");
        if (!tagNode->getIcon() || tagNode->getIcon()->getScale() == 0.625) tagNode->setIcon(CCSprite::create("404.png"_spr));

        if (std::unordered_set<std::string>{
            "generic", "tech", "effect", "design", "layout", "modern", "art", "ambience", "animation", "cartoon",
            "realism", "core", "minigame", "simplism", "atmospheric", "experimental", "standard", "recreation", "sunset"
        }.count(tag))  tagNode->setIcon(CCSprite::createWithSpriteFrameName("style.png"_spr));

        tagNode->setAnchorPoint({1, 0.5});
        tagNode->setScale(0.35);
        tagNode->setColor(tagUtils::color(tag));
        tagNode->setOpacity(255);
        
        if(tagNode->getIcon()) tagNode->getIcon()->setScale(0.5);

        return tagNode;
    }

    static std::string getTagDesc(std::string tag) {
        std::unordered_map<std::string, std::string> desc = {
            {"standard", "A style that uses 1.0-1.9 decorations, often resembling official levels due to its limitations."},
            {"layout", "A style that prioritizes gameplay as the main focus, often with little to no emphasis on decoration."},
            {"generic", "Popular at the turn of versions 2.0 and 2.1, this style is characterized by a predominantly dark color palette, accentuated by strikingly bright outlines."},
            {"design", "Similar to the generic style, however block designs and air decorations are much simpler. Secondary colors also do not pop out as much as in other styles."},
            {"glow", "Style similar to generic design, but with a heavy emphasis on glowing objects and blending effects. Levels in this style often use glow to create dynamic effects like camera flashes or glowing lasers."},
            {"effect", "A style featuring flashy, colorful designs with visual effects, showcasing the level editor's capabilities, often set against a solid black background."},
            {"modern", "A simplistic and uniform style with well-defined shapes, angles, and structures. It uses solid, contrasting colors for a pleasant look and is often combined with art elements."},
            {"simplism", "Very simplistic style, typically using only two colors for the background and blocks, with minimal decoration."},
            {"cartoon", "A style that uses bright, vivid colors and exaggerated shapes to create a cartoonish look."},
            {"tech", "A blend of Art and Modern styles, creating a futuristic look that often features bossfights, usually with heavy use of glow."},
            {"art", "A style that uses art as the main source of decoration, often combined with modern elements."},
            {"realism", "A style similar to art, but with a focus on achieving realistic visuals."},
            {"animation", "A style that heavily uses animations, often designed to tell a story or convey a narrative through dynamic visuals and movement."},
            {"pixel", "A style that mainly uses default pixel blocks."},
            {"core", "A style focused on structure, symmetry, and intricate details, often resulting in a mechanical or geometric look."},
            {"atmospheric", "A style designed to evoke atmosphere, often visually impressive and featuring a distinct climate or mood that enhances the gameplay experience."},
            {"experimental", "A style that lacks a set design or executes existing styles in a unique way, ranging from unconventional levels to slightly modified versions of current styles."},
            {"circles", "A style that uses pulsing objects during fast wave parts, creating flashy effects with long segments, often combined with effect style."},
            {"recreation", "A style designed to recreate the functions or gameplay of a separate game, or heavily inspired by it"},
            {"sunset", "A style featuring sunset-colored backgrounds with objects that are purely black."},
            {"minigame", "A style that features a minigame or contains a minigame within the level."},
            {"1.0","This level is themed around version 1.0, and uses mostly objects from this era."},
            {"1.9","This level is themed around version 1.9, and uses mostly objects from this era."},
            {"hell","This level is themed around hell, featuring a dominant red palette, demonic and satanic imagery, as well as elements like blood and fire"},
            {"heaven","This level is themed around heaven, featuring white and gold palette, angelic elements and clouds."},
            {"rainbow","This level is themed around rainbow, heavily features rainbow blocks prominently throughout its decoration."},
            {"retro","This level is themed around retro, mostly features black backgrounds with green outlines, designed to evoke a classic retro aesthetic."},
            {"space","This level is themed around space, featuring a cosmic design with elements such as starry backgrounds, planets, meteors and other elements commonly associated with space."},
            {"robtop","This level aims to replicate the style of RobTop's main levels and spinoff games."},
            {"apocalyptic","This level is themed around a post-apocalyptic world, featuring ruined landscapes, dark skies, and crumbling buildings."},
            {"futuristic","This level features decoration arranged to evoke a futuristic, tech-inspired aesthetic."},
            {"pixel art","This level heavily uses pixel art for custom objects and backgrounds (not pixel blocks from 2.2)."},
            {"monochromatic","This level have one particular color palette in them. The colors don’t change at all throughout the level."},
            {"aquatic","This level is themed around the ocean, featuring an underwater atmosphere with aquatic elements."},
            {"nature","This level is themed around nature, featuring elements such as landscapes, mountains and other natural scenery."},
            {"cold","This level have a cold environment, typically featuring snow, ice, and frosty landscapes."},
            {"hot","This level have a hot environment, typically featuring deserts, volcanoes, and other hot landscapes."},
            {"desert","This level is themed around desert landscape, featuring sand, pyramids and oases."},
            {"magic","This level is themed around magic, featuring enchanting forests, glowing runes and other elements commonly associated with magic."},
            {"impossible","This level is to be humanly impossible, or humanly possible but too difficult to be ever beaten."},
            {"challenge","This level is typically shorter, meant to be a quick burst of difficulty."},
            {"flashy","This level contains flashy colors that may trigger epilepsy. If you are affected by this condition, it is strongly advised not to play levels with this tag."},
            {"checkpointless", "This level does not have any checkpoints, requiring to complete the entire level in one run."},
            {"nong","This level contains a song that must be downloaded from a source other than the official ones."},
            {"2p","This level contains a 2p option turned on."},
            {"nerf","This level is an easier version of an existing level."},
            {"buff","This level is harder version of an existing level."},
            {"remake","This level is remake of another game."},
            {"story","This level is meant to tell a story."},
            {"fixed hitboxes","This level contains objects with more accurate hitboxes"},
            {"joke","This level contains humorous elements."},
            {"cube","This level's difficulty mainly comes from its cube sections."},
            {"ship","This level's difficulty mainly comes from its ship sections."},
            {"jetpack","This level's difficulty mainly comes from its jetpack sections."},
            {"ball","This level's difficulty mainly comes from its ball sections."},
            {"ufo","This level's difficulty mainly comes from its ufo sections."},
            {"wave","This level's difficulty mainly comes from its wave sections."},
            {"robot","This level's difficulty mainly comes from its robot sections."},
            {"spider","This level's difficulty mainly comes from its spider sections."},
            {"swing","This level's difficulty mainly comes from its swing sections."},
            {"bossfight","This level features bossfights and attack system in platformer mode."},
            {"flow","This level have flowy gameplay, characterized by fast paced gameplay and frequent gamemode changes."},
            {"fast paced","This level has quick and intense gameplay (usually using x3 or x4 player speed)."},
            {"slow paced","This level has slow and relaxing gameplay (usually using x0.5 and x1 player speed)."},
            {"sync","This level's gameplay is synchronized with the music."},
            {"blinds","This level contains off-screen obstacles."},
            {"memory","This level requires to memorize paths or patterns"},
            {"duals","This level's difficulty mainly comes from its duals sections."},
            {"timing","This level requires precision and accuracy."},
            {"sideways","This level contains sections where you move sideways."},
            {"random","This level contains randomized sections or elements"},
            {"high cps","This level requires high click speed."},
            {"nerve control","This level requires ability to handle pressure near the end of the level."},
            {"learny","This level may take some time to learn its gameplay."},
            {"chokepoints","This level contains sections with significantly higher difficulty compared to the rest of the level."},
            {"gimmicky","This level differs from standard gameplay by featuring custom mechanics."},
            {"spam","This level requires fast inputs."},
            {"maze","This level requires to navigate a labyrints-like layout to find the correct path."},
            {"needle","This level requires hitbox abusing."},
            {"timed","This level has short sections that need to be completed within a set time limit."},
            {"puzzle","This level has sections that require solving puzzles."},
            {"momentum","This level requires using slopes to gain speed and maintain momentum."},
            {"wall jump","This level features wall jumping mechanics."},
            {"cycle","This level contains obstacles that follow a repeating pattern."},
            {"slope boost","This level requires slope boost skills to move up slopes."},
            {"slippery","This level features slippery or icy surfaces."},
            {"zippers","This level contains platforms that boost the player, a mechanic commonly seen in Celeste."},
            {"wavedash","This level contains modified sped up dash orbs."},
            {"force blocks","This level contains force blocks gameplay or uses them as wind"},
            {"blinkers","This level features platforms or obstacles that appear and disappear at regular intervals."},
            {"avoidance","This level focuses on avoiding obstacles."},
            {"foddian","This level usually lacks hazards but punishes mistakes by falling to the bottom of the level."},
            {"autoscroller","This level has sections where the screen automatically scrolls."},
            {"rooms","This level is divided into rooms."},
            {"double jump","This level features double jump mechanics."},
            {"speedrun","This level includes sections, designed to be completed as fast as possible."},
            {"gravity","This level contains gravity changes."},
            {"classic","This level features an autoscroll section, similar to classic mode."},
            {"metroidvania","This level is heavily inspired by Metroidvania games, featuring open-world areas that can be explored non-linearly, with progressive upgrades unlocking new paths and abilities."},
        };

        if (auto a = desc.find(tag); a != desc.end()) {
            return a->second.c_str();
        } else {
            return "no description provided";
        }
    };
};