#pragma once
#include <string>
#include <map>
#include <vector>
#include "resource.h"

#define g_data CDataManager::Instance()

struct SettingData
{
    vector<std::wstring> m_notes; // �ʼ�
};

class CDataManager
{
public:
    CDataManager();
    ~CDataManager();

    static CDataManager& Instance();
    void LoadConfig();
    void SaveConfig() const;
    const CString& StringRes(UINT id);      //������Դid��ȡһ���ַ�����Դ
    void DPIFromWindow(CWnd* pWnd);
    int DPI(int pixel);
    float DPIF(float pixel);
    int RDPI(int pixel);
    HICON GetIcon(UINT id);
    void AddNote(const std::wstring& note);
    void DeleteNote(size_t index);
    void ClearNotes();
    const std::vector<std::wstring>& GetNotes() const;

    SettingData m_setting_data;

private:
    std::wstring GetConfigPath() const;
    std::map<UINT, HICON> m_icons;
    std::map<UINT, CString> m_string_table;
    static CDataManager m_instance;
    int m_dpi{ 96 };
};