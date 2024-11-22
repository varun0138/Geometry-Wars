#include <iostream>
#include <fstream>

#include "AssetsManager.hpp"

AssetsManager::AssetsManager() {}

void AssetsManager::loadFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if(!file) {
        std::cerr << "File not loaded: " << filePath << std::endl;
        return;
    }


    std::string type, name, path;
    while(!file.eof()) {
        file >> type >> name >> path;

        if(type == "Font") {
            addFont(name, path);
        }
        else if(type == "Sound") {
            addSoundBuffer(name, path);
            addSound(name, getSoundBuffer(name));
        }
        else {
            std::cerr << "Type not found: " << type << std::endl;
        }
    }

    file.close();
}

void AssetsManager::addFont(const std::string& name, const std::string& path) {
    sf::Font font;
    if(!font.loadFromFile(path)) {
        std::cerr << "Font not loaded: " << path << std::endl;
        return;
    }
    m_fonts[name] = font;
    std::cout << "Font loaded: " << path << std::endl;
}

void AssetsManager::addSoundBuffer(const std::string& name, const std::string& path) {
    sf::SoundBuffer buffer;
    if(!buffer.loadFromFile(path)) {
        std::cerr << "Sound Buffer not loaded: " << path << std::endl;
        return;
    }
    m_soundBuffers[name] = buffer;
    std::cout << "Sound Buffer loaded: " << path << std::endl;
}

void AssetsManager::addSound(const std::string& name, const sf::SoundBuffer& buffer) {
    sf::Sound sound;
    sound.setBuffer(buffer);
    sound.setVolume(30.0f);
    m_sounds[name] = sound;
}

const sf::Font& AssetsManager::getFont(const std::string& name) const {
    return m_fonts.at(name);
}

const sf::SoundBuffer& AssetsManager::getSoundBuffer(const std::string& name) const {
    return m_soundBuffers.at(name);
}

sf::Sound& AssetsManager::getSound(const std::string& name) {
    return m_sounds.at(name);
}