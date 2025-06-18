#pragma once
#include <string>
#include <vector>
#include <map>
#include <sqlite3.h>

struct NoteData {
    int id;
    std::wstring text;
    std::wstring createTime;
    std::wstring updateTime;
    std::wstring summary; // ÐÂÔö£ºÕªÒª
    int categoryId;
};

class SQLiteHelper {
public:
    SQLiteHelper();
    ~SQLiteHelper();

    bool Init(const std::wstring& dbPath);
    bool DeleteNote(int id);
    bool UpdateNote(int id, const std::wstring& newText, const std::wstring& summary, int categoryId);
    std::vector<NoteData> GetAllNotes();
    bool Open(const std::wstring& dbPath);
    bool Execute(const std::wstring& sql, const std::vector<std::wstring>& params = {});
    std::vector<std::map<std::wstring, std::wstring>> Query(const std::wstring& sql, const std::vector<std::wstring>& params = {});
    void Close();
    bool InsertNoteWithSummary(const std::wstring& text, const std::wstring& summary, const int& category_id);
    bool InsertCategory(const std::wstring& category);
    std::vector<std::pair<int, std::wstring>> GetAllCategories();
    int searchByCategory(int categoryId);
    bool checkCategory();
    std::vector<NoteData> SearchNotes(const std::wstring& keyword);
    std::vector<NoteData> getNotesWithCategoryId(int categoryId);
    bool DeleteNotesByCategoryId(int categoryId);
    bool DeleteCategory(int categoryId);

private:
    sqlite3* m_db = nullptr;
    bool Execute(const std::string& sql);
    static std::string WStringToUTF8(const std::wstring& str);
    static std::wstring UTF8ToWString(const std::string& str);
};
