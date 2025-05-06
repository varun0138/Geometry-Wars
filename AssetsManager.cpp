#include <iostream>
#include <fstream>
#include <stdexcept>

#include "AssetsManager.hpp"

AssetsManager::AssetsManager() {}

void AssetsManager::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);

    if(!file) { throw std::runtime_error("File Not Loaded: " + filepath); }

    unsigned int type;
    std::string name, path;
    while(!file.eof()) {
        file >> type >> name >> path;

        switch(Type(type)) {
            case Type::FONT: {
                loadFont(name, path);
                std::cout << "Font Loaded: " << path << std::endl;
                break;
            }
                
            case Type::SOUND: {
                loadSoundBuffer(name, path);
                loadSound(name, m_soundBuffers[name]);
                std::cout << "Sound Loaded: " << path << std::endl;
                break;
            }

            default:
                break;
        }
    }

    file.close();
}

void AssetsManager::loadFont(const std::string& name, const std::string& path) {
    sf::Font font;
    if(!font.loadFromFile(path)) {
        throw std::runtime_error("Font Not Loaded: "+ path);
    }
    m_fonts[name] = font;
}

void AssetsManager::loadSoundBuffer(const std::string& name, const std::string& path) {
    sf::SoundBuffer buffer;
    if(!buffer.loadFromFile(path)) {
        throw std::runtime_error("Sound Not Loaded: "+ path);
    }
    m_soundBuffers[name] = buffer;
}

void AssetsManager::loadSound(const std::string& name, const sf::SoundBuffer& buffer) {
    sf::Sound sound;
    sound.setBuffer(buffer);
    sound.setVolume(30.0f);
    m_sounds[name] = sound;
}

const sf::Font& AssetsManager::getFont(const std::string& name) const {
    return m_fonts.at(name);
}

sf::Sound& AssetsManager::getSound(const std::string& name) {
    return m_sounds.at(name);
}