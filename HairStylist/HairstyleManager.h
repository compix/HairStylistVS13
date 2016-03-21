#pragma once
#include <string>
#include "Hairstyle.h"
#include <vector>

class Framebuffer;

class HairstyleManager
{
    struct HairstyleInfo
    {
        HairstyleInfo() {}
        HairstyleInfo(const std::string& filename, const Hairstyle& hairstyle)
            : filename(filename), hairstyle(hairstyle) {}

        std::string filename;
        Hairstyle hairstyle;
    };

    friend std::ostream& operator <<(std::ostream &output, const HairstyleInfo& info)
    {
        auto& hs = info.hairstyle;
        return output << info.filename << " "
            << hs.color.r << " " << hs.color.g << " " << hs.color.b << " "
            << hs.width << " "
            << hs.length;
    }

    friend std::istream& operator >>(std::istream &input, HairstyleInfo& info)
    {
        auto& hs = info.hairstyle;
        return input >> info.filename
            >> hs.color.r >> hs.color.g >> hs.color.b
            >> hs.width
            >> hs.length;
    }

public:
    HairstyleManager(const std::string& hairstyleName, const std::string& basePath, const std::string& infoFilename);

    void loadNext(Framebuffer& framebuffer, Hairstyle& outHairstyle);
    void loadPrev(Framebuffer& framebuffer, Hairstyle& outHairstyle);
    void load(size_t idx, Framebuffer& framebuffer, Hairstyle& outHairstyle);
    void loadRecent(Framebuffer& framebuffer, Hairstyle& outHairstyle);
    void save(const Hairstyle& hairstyle, Framebuffer& framebuffer);

private:
    void saveInfo();

private:
    std::vector<HairstyleInfo> m_hairstyles;

    size_t m_curStyleIndex{ 0 };
    size_t m_hairstyleCounter{ 0 };

    std::string m_hairstyleName;
    std::string m_basePath;
    std::string m_infoFilename;
};
