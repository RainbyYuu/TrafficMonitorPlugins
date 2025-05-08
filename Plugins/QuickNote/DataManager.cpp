#include "pch.h"
#include "DataManager.h"
#include <fstream>
#include <ShlObj.h>
#include <vector>
#include <sstream>
#include "../utilities/bass64/base64.h"
#include <locale>
#include <codecvt>

CDataManager CDataManager::m_instance;

CDataManager::CDataManager()
{
    //初始化DPI
    HDC hDC = ::GetDC(HWND_DESKTOP);
    m_dpi = GetDeviceCaps(hDC, LOGPIXELSY);
    ::ReleaseDC(HWND_DESKTOP, hDC);
}

CDataManager::~CDataManager()
{
    SaveConfig();
}

CDataManager& CDataManager::Instance()
{
    return m_instance;
}

void CDataManager::AddNote(const std::wstring& note)
{
    m_setting_data.m_notes.push_back(note);
    SaveConfig();
}

void CDataManager::DeleteNote(size_t index)
{
    if (index < m_setting_data.m_notes.size())
    {
        m_setting_data.m_notes.erase(m_setting_data.m_notes.begin() + index);
        SaveConfig();
    }
}

void CDataManager::ClearNotes()
{
    m_setting_data.m_notes.clear();
    SaveConfig();
}

const std::vector<std::wstring>& CDataManager::GetNotes() const
{
    return m_setting_data.m_notes;
}
/*
void CDataManager::LoadConfig()
{
    std::wifstream file(GetConfigPath());
    if (!file)
        return;

    std::wstring line;
    while (std::getline(file, line))
    {
        if (!line.empty())
            m_setting_data.m_notes.push_back(line);
    }
    file.close();  // 显式关闭
}

void CDataManager::SaveConfig() const
{
    std::wofstream file(GetConfigPath());
    if (!file)
        return;

    for (const auto& note : m_setting_data.m_notes)
    {
        file << note << std::endl;
    }

    file.flush();  // 刷新缓冲区
    file.close();  // 显式关闭
}
*/

void CDataManager::LoadConfig() {
    // 打开文件时指定UTF-8编码模式
    std::wifstream file(GetConfigPath());
    if (!file)
        return;

    // 设置本地化和UTF-8编码
    file.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));

    std::wstring line;
    while (std::getline(file, line))
    {
        if (!line.empty())
            m_setting_data.m_notes.push_back(line);
    }
    file.close();
}

void CDataManager::SaveConfig() const {
    // 打开文件时指定UTF-8编码模式
    std::wofstream file(GetConfigPath());
    if (!file)
        return;

    // 设置本地化和UTF-8编码
    file.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));

    // 可选：添加BOM标记使某些Windows应用更好地识别UTF-8
    // const wchar_t BOM = 0xFEFF;
    // file << BOM;

    for (const auto& note : m_setting_data.m_notes)
    {
        file << note << std::endl;
    }
    file.flush();
    file.close();
}

std::wstring CDataManager::GetConfigPath() const
{
    wchar_t appData[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, appData)))
    {
        std::wstring path = appData;
        path += L"\\TrafficMonitor\\QuickNote";
        CreateDirectory(path.c_str(), NULL);
        return path + L"\\notes.txt";
    }
    return L"";
}

const CString& CDataManager::StringRes(UINT id)
{
    auto iter = m_string_table.find(id);
    if (iter != m_string_table.end())
    {
        return iter->second;
    }
    else
    {
        AFX_MANAGE_STATE(AfxGetStaticModuleState());
        m_string_table[id].LoadString(id);
        return m_string_table[id];
    }
}

void CDataManager::DPIFromWindow(CWnd* pWnd)
{
    CWindowDC dc(pWnd);
    HDC hDC = dc.GetSafeHdc();
    m_dpi = GetDeviceCaps(hDC, LOGPIXELSY);
}

int CDataManager::DPI(int pixel)
{
    return m_dpi * pixel / 96;
}

float CDataManager::DPIF(float pixel)
{
    return m_dpi * pixel / 96;
}

int CDataManager::RDPI(int pixel)
{
    return pixel * 96 / m_dpi;
}

HICON CDataManager::GetIcon(UINT id)
{
    auto iter = m_icons.find(id);
    if (iter != m_icons.end())
    {
        return iter->second;
    }
    else
    {
        AFX_MANAGE_STATE(AfxGetStaticModuleState());
        HICON hIcon = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(id), IMAGE_ICON, DPI(16), DPI(16), 0);
        m_icons[id] = hIcon;
        return hIcon;
    }
}