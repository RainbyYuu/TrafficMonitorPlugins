#include "pch.h"
#include "DataManager.h"
#include <fstream>
#include <ShlObj.h>
#include <vector>
#include <sstream>
#include "../utilities/bass64/base64.h"
#include <locale>
#include <codecvt>
#include <ctime>
#include "DeepSeekHelper.h"
#define MAXSUMMARYCHAR 10


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
    
}

CDataManager& CDataManager::Instance()
{
    return m_instance;
}

void CDataManager::LoadConfig()
{
    m_setting_data.m_notes = m_db.GetAllNotes();
    m_setting_data.category = m_db.GetAllCategories();
}


void CDataManager::SaveConfig() const {
    // 不再需要手动保存，每个操作即写入数据库
}

void CDataManager::AddNote(const std::wstring& note, const int& category_id) {
    std::wstring summary;
    if (note.size() > MAXSUMMARYCHAR) {
        summary = GetNoteSummary(note);  // 调用摘要函数
    }
    else {
        summary = note;
    }

    m_db.InsertNoteWithSummary(note, summary, category_id);
    LoadConfig(); // 刷新界面
}

void CDataManager::AddCategory(const std::wstring& category)
{
    m_db.InsertCategory(category);
    LoadConfig(); // 刷新界面
}

std::wstring CDataManager::GetNoteSummary(const std::wstring& content) {
    return DeepSeekHelper::GenerateSummary(content);
}

std::vector<NoteData> CDataManager::SearchNotes(const std::wstring& content)
{
    return m_db.SearchNotes(content);
}

std::vector<NoteData> CDataManager::getNotesWithCategoryId(int categoryId)
{
    return m_db.getNotesWithCategoryId(categoryId);
}

void CDataManager::DeleteByCategoryId(int categoryId)
{
    m_db.DeleteNotesByCategoryId(categoryId);
    m_db.DeleteCategory(categoryId);
}

void CDataManager::DeleteNote(size_t index) {
    if (index < m_setting_data.m_notes.size())
    {
        int id = m_setting_data.m_notes[index].id;
        m_db.DeleteNote(id);
        LoadConfig();
    }
}

void CDataManager::ClearNotes() {
    for (const auto& note : m_setting_data.m_notes)
        m_db.DeleteNote(note.id);
    LoadConfig();
}

const std::vector<NoteData>& CDataManager::GetNotes() const
{
    return m_setting_data.m_notes;
}

bool CDataManager::InitDatabase() {
    return m_db.Init(GetDBPath());
}

void CDataManager::UpdateNoteTextById(int noteId, const std::wstring& newText, int categoryId)
{
    CString updateTime = CTime::GetCurrentTime().Format(L"%Y-%m-%d %H:%M:%S");

    std::wstring summary = (newText.length() > 50)
        ? DeepSeekHelper::GenerateSummary(newText)
        : newText;

    m_db.UpdateNote(noteId, newText, summary, categoryId);

    LoadConfig(); // 重新加载内存数据
}

int CDataManager::searchByCategory(const int& categoryId)
{
    return m_db.searchByCategory(categoryId);
}

std::wstring CDataManager::GetDBPath() const
{
    wchar_t appData[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, appData)))
    {
        std::wstring path = appData;
        path += L"\\TrafficMonitor\\QuickNote";
        CreateDirectory(path.c_str(), NULL);
        return path + L"\\notes.db";
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
