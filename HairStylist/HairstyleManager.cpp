#include "HairstyleManager.h"
#include <fstream>
#include "file.h"
#include "Framebuffer.h"

HairstyleManager::HairstyleManager(const std::string& hairstyleName, const std::string& basePath, const std::string& infoFilename)
    :m_hairstyleName(hairstyleName), m_basePath(basePath), m_infoFilename(infoFilename)
{
    std::ifstream info(m_basePath + "/" + m_infoFilename);

    // Load hairstyle counter
    info >> m_hairstyleCounter;

    // Load hairstyle information
    HairstyleInfo hairstyleInfo;
    while (info >> hairstyleInfo)
        if (file::exists(m_basePath + "/" + hairstyleInfo.filename))
            m_hairstyles.push_back(hairstyleInfo);
}

void HairstyleManager::loadNext(Framebuffer& framebuffer, Hairstyle& outHairstyle)
{
    if (m_hairstyles.size() == 0)
        return;

    m_curStyleIndex = (m_curStyleIndex + 1) % m_hairstyles.size();
    load(m_curStyleIndex, framebuffer, outHairstyle);
}

void HairstyleManager::loadPrev(Framebuffer& framebuffer, Hairstyle& outHairstyle)
{
    if (m_hairstyles.size() == 0)
        return;

    m_curStyleIndex = (m_curStyleIndex - 1 + m_hairstyles.size()) % m_hairstyles.size();
    load(m_curStyleIndex, framebuffer, outHairstyle);
}

void HairstyleManager::load(size_t idx, Framebuffer& framebuffer, Hairstyle& outHairstyle)
{
    assert(idx < m_hairstyles.size());
    framebuffer.loadRenderTexture(m_basePath + "/" + m_hairstyles[idx].filename);
    outHairstyle = m_hairstyles[idx].hairstyle;
}

void HairstyleManager::loadRecent(Framebuffer& framebuffer, Hairstyle& outHairstyle)
{
    if (m_hairstyles.size() == 0)
        return;

    load(m_hairstyles.size() - 1, framebuffer, outHairstyle);
}

void HairstyleManager::save(const Hairstyle& hairstyle, Framebuffer& framebuffer)
{
    std::string filename = m_hairstyleName + std::to_string(m_hairstyleCounter++) + ".style";
    m_hairstyles.push_back(HairstyleInfo(filename, hairstyle));
    framebuffer.saveRenderTexture(m_basePath + "/" + filename);
    saveInfo();
}

void HairstyleManager::saveInfo()
{
    std::ofstream info(m_basePath + "/" + m_infoFilename);

    // Save hairstyle counter
    info << m_hairstyleCounter << "\n";

    // Save hairstyle information
    for (auto& hs : m_hairstyles)
        info << hs << "\n";
}
