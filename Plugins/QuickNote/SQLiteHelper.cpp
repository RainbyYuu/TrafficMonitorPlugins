#include "pch.h"
#include "SQLiteHelper.h"
#include <sstream>
#include <ctime>
#include <codecvt>
#include <locale>

SQLiteHelper::SQLiteHelper() {}

SQLiteHelper::~SQLiteHelper() {
    if (m_db) sqlite3_close(m_db);
}

bool SQLiteHelper::Init(const std::wstring& dbPath) {
    if (sqlite3_open16(dbPath.c_str(), &m_db) != SQLITE_OK)
        return false;
    std::string createTable = R"(
    CREATE TABLE IF NOT EXISTS notes (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        note_text TEXT NOT NULL,
        summary TEXT,
        create_time TEXT,
        update_time TEXT,
        category_id INTEGER
    );
    CREATE TABLE IF NOT EXISTS category (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT UNIQUE
    );
    CREATE TABLE IF NOT EXISTS tag (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT UNIQUE
    );
    CREATE TABLE IF NOT EXISTS notetag (
        note_id INTEGER,
        tag_id INTEGER,
        weight INTEGER DEFAULT 1,
        FOREIGN KEY(note_id) REFERENCES notes(id),
        FOREIGN KEY(tag_id) REFERENCES tag(id)
    );
)";
    Execute(createTable);

    return checkCategory();
}

bool SQLiteHelper::Open(const std::wstring& dbPath)
{
    Close(); // 保证不重复打开
    std::string utf8Path = WStringToUTF8(dbPath);
    return sqlite3_open(utf8Path.c_str(), &m_db) == SQLITE_OK;
}

bool SQLiteHelper::Execute(const std::string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool SQLiteHelper::InsertNoteWithSummary(const std::wstring& text, const std::wstring& summary, const int& category_id)
{
    CString now = CTime::GetCurrentTime().Format(L"%Y-%m-%d %H:%M:%S");
    std::wostringstream oss;
    oss << category_id;
    std::wstring wstr = oss.str();


    std::wstring sql = L"INSERT INTO notes (note_text, summary, category_id, create_time, update_time) VALUES (?, ?, ?, ?, ?)";
    return Execute(sql, { text, summary, wstr, (LPCWSTR)now, (LPCWSTR)now });
}

bool SQLiteHelper::InsertCategory(const std::wstring& category)
{
    std::wstring sql = L"INSERT INTO category (name) VALUES (?)";
    return Execute(sql, {category});
}

std::vector<std::pair<int, std::wstring>> SQLiteHelper::GetAllCategories()
{
    std::vector<std::pair<int, std::wstring>> categories;
    CString sql = _T("SELECT id, name FROM Category");

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare16_v2(m_db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            int id = sqlite3_column_int(stmt, 0);
            const wchar_t* name = reinterpret_cast<const wchar_t*>(sqlite3_column_text16(stmt, 1));
            categories.emplace_back(id, name ? name : L"");
        }
    }
    sqlite3_finalize(stmt);
    return categories;
}

int SQLiteHelper::searchByCategory(int categoryId)
{
    CString sql = _T("SELECT COUNT(*) FROM notes WHERE category_id = ?");

    sqlite3_stmt* stmt = nullptr;
    int count = 0;
    if (sqlite3_prepare16_v2(m_db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, categoryId);
        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            count = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return count;
}

bool SQLiteHelper::checkCategory()
{
    std::string sql = "SELECT COUNT(*) FROM category";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        return false;
    }

    bool isEmpty = false;

    // 执行查询并获取结果
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        int count = sqlite3_column_int(stmt, 0);
        isEmpty = (count == 0);
    }
    if (isEmpty) {
        std::wstring sql = L"INSERT INTO category (name) VALUES (?)";
        return Execute(sql, { L"默认" });
    }

    // 完成后清理资源
    sqlite3_finalize(stmt);

    return true;
}

std::vector<NoteData> SQLiteHelper::SearchNotes(const std::wstring& keyword)
{
    std::vector<NoteData> notes;

    if (!m_db || keyword.empty()) {
        return notes;
    }

    // 使用 LIKE 进行模糊搜索，搜索笔记内容和摘要
    std::wstring sql = L"SELECT id, note_text, summary, create_time, update_time, category_id "
        L"FROM notes "
        L"WHERE note_text LIKE ? OR summary LIKE ? "
        L"ORDER BY update_time DESC";

    sqlite3_stmt* stmt = nullptr;
    std::string utf8Sql = WStringToUTF8(sql);

    if (sqlite3_prepare_v2(m_db, utf8Sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        // 准备搜索关键词（添加通配符）
        std::wstring searchPattern = L"%" + keyword + L"%";
        std::string utf8Pattern = WStringToUTF8(searchPattern);

        // 绑定参数
        sqlite3_bind_text(stmt, 1, utf8Pattern.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, utf8Pattern.c_str(), -1, SQLITE_TRANSIENT);

        // 执行查询
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            NoteData note;
            note.id = sqlite3_column_int(stmt, 0);
            note.text = UTF8ToWString((const char*)sqlite3_column_text(stmt, 1));
            const char* summaryText = (const char*)sqlite3_column_text(stmt, 2);
            note.summary = summaryText ? UTF8ToWString(summaryText) : note.text;
            note.createTime = UTF8ToWString((const char*)sqlite3_column_text(stmt, 3));
            note.updateTime = UTF8ToWString((const char*)sqlite3_column_text(stmt, 4));
            note.categoryId = sqlite3_column_int(stmt, 5);
            notes.push_back(note);
        }
    }

    sqlite3_finalize(stmt);
    return notes;
}

std::vector<NoteData> SQLiteHelper::getNotesWithCategoryId(int categoryId)
{
    std::vector<NoteData> notes;
    std::string sql = "SELECT * FROM notes WHERE category_id = ?";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, categoryId);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            NoteData note;
            note.id = sqlite3_column_int(stmt, 0);
            note.text = UTF8ToWString((const char*)sqlite3_column_text(stmt, 1));
            const char* text = (const char*)sqlite3_column_text(stmt, 2);
            note.summary = text ? UTF8ToWString(text) : note.text;
            note.createTime = UTF8ToWString((const char*)sqlite3_column_text(stmt, 3));
            note.updateTime = UTF8ToWString((const char*)sqlite3_column_text(stmt, 4));
            note.categoryId = sqlite3_column_int(stmt, 5);
            notes.push_back(note);
        }
    }
    sqlite3_finalize(stmt);
    return notes;
}

bool SQLiteHelper::DeleteNotesByCategoryId(int categoryId)
{
    std::string sql = "DELETE FROM notes WHERE category_id = ?";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, categoryId);
    bool result = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return result;
}

bool SQLiteHelper::DeleteCategory(int categoryId)
{
    std::string sql = "DELETE FROM category WHERE id = ?";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, categoryId);
    bool result = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return result;
}

bool SQLiteHelper::DeleteNote(int id) {
    std::string sql = "DELETE FROM notes WHERE id = ?";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, id);
    bool result = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return result;
}

bool SQLiteHelper::UpdateNote(int id, const std::wstring& newText) {
    std::string sql = "UPDATE notes SET note_text = ?, update_time = datetime('now','localtime') WHERE id = ?";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text16(stmt, 1, newText.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, id);
    bool result = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return result;
}

std::vector<NoteData> SQLiteHelper::GetAllNotes() {
    std::vector<NoteData> notes;
    std::string sql = "SELECT id, note_text, summary, create_time, update_time, category_id FROM notes ORDER BY id ASC";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            NoteData note;
            note.id = sqlite3_column_int(stmt, 0);
            note.text = UTF8ToWString((const char*)sqlite3_column_text(stmt, 1));
            const char* text = (const char*)sqlite3_column_text(stmt, 2);
            note.summary = text ? UTF8ToWString(text) : note.text;
            note.createTime = UTF8ToWString((const char*)sqlite3_column_text(stmt, 3));
            note.updateTime = UTF8ToWString((const char*)sqlite3_column_text(stmt, 4));
            note.categoryId = sqlite3_column_int(stmt, 5);
            notes.push_back(note);
        }
    }
    sqlite3_finalize(stmt);
    return notes;
}

void SQLiteHelper::Close()
{
    if (m_db)
    {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

bool SQLiteHelper::Execute(const std::wstring& sql, const std::vector<std::wstring>& params)
{
    sqlite3_stmt* stmt = nullptr;
    std::string utf8Sql = WStringToUTF8(sql);
    if (sqlite3_prepare_v2(m_db, utf8Sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        CString errorMsg;
        errorMsg.Format(_T("SQL error: %hs"), sqlite3_errmsg(m_db));
        OutputDebugString(errorMsg);
        return false;
    }

    for (size_t i = 0; i < params.size(); ++i)
    {
        sqlite3_bind_text(stmt, static_cast<int>(i + 1), WStringToUTF8(params[i]).c_str(), -1, SQLITE_TRANSIENT);
    }

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}


std::vector<std::map<std::wstring, std::wstring>> SQLiteHelper::Query(const std::wstring& sql, const std::vector<std::wstring>& params)
{
    std::vector<std::map<std::wstring, std::wstring>> results;
    sqlite3_stmt* stmt = nullptr;
    std::string utf8Sql = WStringToUTF8(sql);
    if (sqlite3_prepare_v2(m_db, utf8Sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        return results;

    for (size_t i = 0; i < params.size(); ++i)
    {
        sqlite3_bind_text(stmt, static_cast<int>(i + 1), WStringToUTF8(params[i]).c_str(), -1, SQLITE_TRANSIENT);
    }

    int cols = sqlite3_column_count(stmt);
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        std::map<std::wstring, std::wstring> row;
        for (int i = 0; i < cols; ++i)
        {
            std::string colName = sqlite3_column_name(stmt, i);
            const unsigned char* val = sqlite3_column_text(stmt, i);
            row[UTF8ToWString(colName)] = val ? UTF8ToWString(reinterpret_cast<const char*>(val)) : L"";
        }
        results.push_back(row);
    }

    sqlite3_finalize(stmt);
    return results;
}

std::string SQLiteHelper::WStringToUTF8(const std::wstring& str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.to_bytes(str);
}

std::wstring SQLiteHelper::UTF8ToWString(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.from_bytes(str);
}
