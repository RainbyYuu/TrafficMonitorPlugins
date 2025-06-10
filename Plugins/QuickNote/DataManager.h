#pragma once
#include <string>
#include <map>
#include <vector>
#include "resource.h"
#include "SQLiteHelper.h"

#define g_data CDataManager::Instance()

struct SettingData {
    std::vector<NoteData> m_notes; // SQLite方式存储
    std::vector<std::pair<int, std::wstring>>category;
};

class CDataManager
{
public:
    CDataManager();
    ~CDataManager();

    static CDataManager& Instance();
    void LoadConfig();
    void SaveConfig() const;
    const CString& StringRes(UINT id);      //根据资源id获取一个字符串资源
    void DPIFromWindow(CWnd* pWnd);
    int DPI(int pixel);
    float DPIF(float pixel);
    int RDPI(int pixel);
    HICON GetIcon(UINT id);
    void AddNote(const std::wstring& note, const int& category_id);
    void AddCategory(const std::wstring& category);
    void DeleteNote(size_t index);
    void ClearNotes();
    const std::vector<NoteData>& GetNotes() const;
    bool InitDatabase();
    void UpdateNoteTextById(int noteId, const std::wstring& newText, int categoryId);
    int searchByCategory(const int& categoryId);
    std::wstring GetNoteSummary(const std::wstring& content);
    std::vector<NoteData> SearchNotes(const std::wstring& content);
    std::vector<NoteData> getNotesWithCategoryId(int categoryId);
    void DeleteByCategoryId(int categoryId);

    SettingData m_setting_data;

private:
    std::wstring GetDBPath() const;
    std::map<UINT, HICON> m_icons;
    std::map<UINT, CString> m_string_table;
    static CDataManager m_instance;
    int m_dpi{ 96 };
    SQLiteHelper m_db;
};