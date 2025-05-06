#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>
#include <unordered_map>
#include <string>

class AssetsManager {
private:
    enum class Type: unsigned char { FONT, SOUND };

    std::unordered_map<std::string, sf::Font> m_fonts;
    std::unordered_map<std::string, sf::SoundBuffer> m_soundBuffers;
    std::unordered_map<std::string, sf::Sound> m_sounds;

    void loadFont(const std::string& name, const std::string& path);
    void loadSoundBuffer(const std::string& name, const std::string& path);
    void loadSound(const std::string& name, const sf::SoundBuffer& buffer);

public:
    AssetsManager() = default;

    void loadFromFile(const std::string& filepath);

    const sf::Font& getFont(const std::string& name) const  { return m_fonts.at(name);  }
    sf::Sound& getSound(const std::string& name)            { return m_sounds.at(name); }
};