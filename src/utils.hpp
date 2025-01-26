#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class TagUtils {
public:
    static ccColor3B color(const std::string& tag);
    static IconButtonSprite* addTag(std::string tag, float scale);
};