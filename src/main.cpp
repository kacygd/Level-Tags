#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/modify/InfoLayer.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <Geode/cocos/cocoa/CCObject.h>
#include <Geode/modify/FLAlertLayer.hpp>
#include <Geode/binding/DailyLevelPage.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>
#include <fmt/core.h>
#include <fmt/format.h>
#include <Geode/loader/SettingEvent.hpp>
#include <Geode/binding/CCMenuItemToggler.hpp>
#include <vector>
#include <functional>
#include <Geode/cocos/base_nodes/Layout.hpp>
#include <matjson.hpp>
#include <string>
#include <unordered_map>
#include <map>
#include <array>
#include <Geode/ui/ScrollLayer.hpp>
#include <Geode/ui/LoadingSpinner.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>

#include "levelCell.hpp"
#include "infoLayer.hpp"
#include "levelInfoLayer.hpp"

// I'll add tag search soon here