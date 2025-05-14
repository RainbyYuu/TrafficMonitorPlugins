#include "pch.h"
#include "QuickNoteItem.h"
#include "DataManager.h"
#include "QuickNote.h"

CQuickNoteItem::CQuickNoteItem()
{
}

CQuickNoteItem::~CQuickNoteItem()
{
}

const wchar_t* CQuickNoteItem::GetItemName() const
{
    return L"快捷笔记";
}

const wchar_t* CQuickNoteItem::GetItemId() const
{
    return L"quick_note";
}

const wchar_t* CQuickNoteItem::GetItemLableText() const
{
    return L"笔记:";
}

const wchar_t* CQuickNoteItem::GetItemValueText() const
{
    if (m_is_command_mode)
        return m_command_buffer.c_str();

    if (m_notes.empty())
        return L"无笔记";

    return m_current_note.c_str();
}

const wchar_t* CQuickNoteItem::GetItemValueSampleText() const
{
    return L"请输入一条笔记...";
}

bool CQuickNoteItem::IsCustomDraw() const
{
    return true;
}

int CQuickNoteItem::GetItemWidth() const
{
    return 12;
}

void CQuickNoteItem::DrawItem(void* hDC, int x, int y, int w, int h, bool dark_mode)
{
    //绘图句柄
    CDC* pDC = CDC::FromHandle((HDC)hDC);
    //矩形区域
    CRect rect(CPoint(x, y), CSize(w, h));
    //TODO: 在此添加绘图代码

    HICON hIcon = g_data.GetIcon(IDI_ICON6); // 或 LoadIcon 等

    const int icon_size{ g_data.DPI(16) };
    CPoint icon_point{ rect.TopLeft() };
    icon_point.x = rect.left + g_data.DPI(2);
    icon_point.y = rect.top + (rect.Height() - icon_size) / 2;
    ::DrawIconEx(pDC->GetSafeHdc(), icon_point.x, icon_point.y, hIcon, icon_size, icon_size, 0, NULL, DI_NORMAL);

}

int CQuickNoteItem::OnMouseEvent(MouseEventType type, int x, int y, void* hWnd, int flag)
{
    CWnd* pWnd = CWnd::FromHandle((HWND)hWnd);
    if (type == IPluginItem::MT_DBCLICKED)
    {
        auto& data_manager = CDataManager::Instance();

        CQuickNote::Instance().ShowOptionsDialog(hWnd);

        return 1;
    }

    return 0;
}

int CQuickNoteItem::OnKeboardEvent(int key, bool ctrl, bool shift, bool alt, void* hWnd, int flag)
{
    return 0;
}

//void CQuickNoteItem::AddNote(const std::wstring& note)
//{
//    m_notes.push_back(note);
//    m_current_note = note;
//}
//
//void CQuickNoteItem::ClearNotes()
//{
//    m_notes.clear();
//    m_current_note.clear();
//}
//
//const std::vector<std::wstring>& CQuickNoteItem::GetNotes() const
//{
//    return m_notes;
//}

bool CQuickNoteItem::ExecuteCommand(const std::wstring& command)
{
    if (command.empty())
        return false;

    /*if (command == L"/new")
    {
        m_current_note.clear();
        m_is_command_mode = false;
        return true;
    }
    else if (command == L"/save")
    {
        if (!m_current_note.empty())
        {
            m_notes.push_back(m_current_note);
            m_is_command_mode = false;
            return true;
        }
    }
    else if (command == L"/clear")
    {
        m_notes.clear();
        m_current_note.clear();
        m_is_command_mode = false;
        return true;
    }*/
    if (command == L"/cmd")
    {
        m_is_command_mode = true;
        m_command_buffer.clear();
        return true;
    }

    return false;
}

void CQuickNoteItem::ProcessCommand(const std::wstring& command)
{
    if (ExecuteCommand(command))
        return;

    if (m_is_command_mode)
    {
        m_command_buffer = command;
    }
    else
    {
        m_current_note = command;
    }
}