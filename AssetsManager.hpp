#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>
#include <map>
#include <string>

class AssetsManager {
private:
    std::map<std::string, sf::Font> m_fonts;
    std::map<std::string, sf::SoundBuffer> m_soundBuffers;
    std::map<std::string, sf::Sound> m_sounds;

public:
    AssetsManager();

    void loadFromFile(const std::string& filePath);

    void addFont(const std::string& name, const std::string& path);
    void addSoundBuffer(const std::string& name, const std::string& path);
    void addSound(const std::string& name, const sf::SoundBuffer& buffer);

    const sf::Font& getFont(const std::string& name) const;
    const sf::SoundBuffer& getSoundBuffer(const std::string& name) const;
    sf::Sound& getSound(const std::string& name);
};