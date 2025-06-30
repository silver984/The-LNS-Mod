#include <Geode/Geode.hpp>
#include <random>
#include "util.hpp"

using namespace geode::prelude;

std::string getResourcePath() {
	return fmt::format("{}/resources/{}", Mod::get()->getID(), Mod::get()->getID());
}

int getRandomInt(int min, int max) {
	if (min == max)
		return min;

	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(min, max);
	return dist(gen);
}