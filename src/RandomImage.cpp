#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>
#include "util.hpp"
using namespace geode::prelude;

bool appear = false;
bool imageCached = false;
std::vector<std::string> image = {
	"bucees.png"_spr,
	"cows.png"_spr,
	"ferriswheel.png"_spr,
	"icecream.png"_spr,
	"outforawalk.png"_spr,
	"windowseat.png"_spr,
	"theman.png"_spr
};
std::vector<CCTexture2D*> imageTexture;

class $modify(LuckyLayer, PlayLayer) {
	struct Fields {
		CCSprite* funny{};
		CCLayerColor* bg{};
	};

	bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
		if (!PlayLayer::init(level, useReplay, dontCreateObjects))
			return false;

		if (!imageCached) {
			for (auto& path : image) {
				auto texture = CCTextureCache::sharedTextureCache()->addImage(path.c_str(), false);
				imageTexture.push_back(texture);

			}
			imageCached = true;
		}

		m_fields->bg = CCLayerColor::create(ccc4(0, 0, 0, 255));
		m_fields->bg->setVisible(false);
		this->addChild(m_fields->bg, 9998);

		m_fields->funny = CCSprite::create(image[getRandomInt(0, image.size() - 1)].c_str());
		m_fields->funny->setVisible(false);
		this->addChild(m_fields->funny, 9999);

		appear = false;

		return true;
	}

	virtual void postUpdate(float p0) {
		auto winSize = CCDirector::get()->getWinSize();

		if (m_fields->funny && m_fields->funny->getParent()) {
			auto spriteSize = m_fields->funny->getContentSize();

			float scaleX = winSize.width / spriteSize.width;
			float scaleY = winSize.height / spriteSize.height;
			float scale = std::min(scaleX, scaleY);

			m_fields->funny->setScale(scale);
			m_fields->funny->setPosition(winSize / 2);
		}

		PlayLayer::postUpdate(p0);
	}

	// delay the respawn time if the picture appears lmao
	virtual void destroyPlayer(PlayerObject* player, GameObject* object) {
		PlayLayer::destroyPlayer(player, object);

		if (!appear)
			return;

		auto* oldTimer = this->getActionByTag(0x10);
		if (oldTimer) {
			this->stopAction(oldTimer);

			auto* newTimer = cocos2d::CCSequence::create(
				cocos2d::CCDelayTime::create(3.f),
				cocos2d::CCCallFunc::create(this, callfunc_selector(PlayLayer::delayedResetLevel)),
				nullptr
			);

			newTimer->setTag(0x10);
			this->runAction(newTimer);
		}
	}

	void pauseGame(bool p0) {
		if (!(m_fields->funny->isVisible() && m_fields->bg->isVisible()))
			PlayLayer::pauseGame(p0);
	}
};

LuckyLayer* getLuckyLayer() {
	return static_cast<LuckyLayer*>(CCDirector::sharedDirector()->getRunningScene()->getChildByID("PlayLayer"));
}

bool objectsVisible(LuckyLayer* lucky) {
	return (lucky->m_fields->funny && lucky->m_fields->funny->getParent()) && (lucky->m_fields->bg && lucky->m_fields->bg->getParent()); // long ass line of code lil bro
}

class $modify(PlayerObject) {
	void playDeathEffect() {
		appear = getRandomInt(1, 4) == 1;
		if (!appear)
			return PlayerObject::playDeathEffect();

		auto lucky = getLuckyLayer();
		if (lucky && objectsVisible(lucky)) {
			lucky->m_fields->bg->setVisible(true);
			lucky->m_fields->funny->setVisible(true);
		}

		PlayerObject::playDeathEffect();
	}

	virtual void resetObject() {
		auto lucky = getLuckyLayer();
		if (lucky && objectsVisible(lucky)) {
			lucky->m_fields->bg->setVisible(false);
			lucky->m_fields->funny->setVisible(false);

			int index = getRandomInt(0, imageTexture.size() - 1);
			if (imageTexture[index]) {
				lucky->m_fields->funny->setTexture(imageTexture[index]);

				auto width = imageTexture[index]->getContentSize().width;
				auto height = imageTexture[index]->getContentSize().height;
				lucky->m_fields->funny->setTextureRect(CCRect(0, 0, width, height));
			}
		}

		PlayerObject::resetObject();
	}
};

class $modify(FMODAudioEngine) {
	int playEffect(gd::string path, float speed, float p2, float volume) {
		auto lucky = getLuckyLayer();
		if (lucky) {
			if (path == "explode_11.ogg" && objectsVisible(lucky))
				return FMODAudioEngine::playEffect(path, speed, p2, 0.f);
		}

		return FMODAudioEngine::playEffect(path, speed, p2, volume);
	}
};
