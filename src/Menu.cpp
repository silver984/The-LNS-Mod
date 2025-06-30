#include <Geode/modify/FMODAudioEngine.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "util.hpp"

using namespace geode::prelude;

class $modify(MenuLayer) {
	virtual bool init() {
		if (!MenuLayer::init())
			return false;

		int index = 0;
		for (auto child : CCArrayExt<CCNode*>(this->getChildren())) {
			if (auto sprite = typeinfo_cast<CCSprite*>(child)) {
				if (index == 0) {
					sprite->setVisible(false);
					auto winSize = CCDirector::get()->getWinSize();
					auto newLogo = CCSprite::create("LNSLogo.png"_spr);
					newLogo->setPosition(CCPoint(winSize.width / 2, sprite->getPosition().y - 8));
					this->addChild(newLogo, sprite->getZOrder());
					break;
				}
				index++;
			}
		}

		return true;
	}
};

std::vector<std::string> audio = {
	"XO",
	"how2p2willruinGD",
	"heyguys",
	"omgibeatit",
	"restaurant",
	"channeltrailer",
	"racism",
};

std::string fileName;

class $modify(FMODAudioEngine) {
	void playMusic(gd::string path, bool shouldLoop, float fadeInTime, int channel) {
		if (path == "menuLoop.mp3") {
			fileName = fmt::format(
				"{}/{}.mp3",
				getResourcePath(),
				audio[getRandomInt(0, audio.size() - 1)]);

			FMODAudioEngine::playMusic(fileName, shouldLoop, fadeInTime, channel);
		}
		else
			FMODAudioEngine::playMusic(path, shouldLoop, fadeInTime, channel);
	}
};