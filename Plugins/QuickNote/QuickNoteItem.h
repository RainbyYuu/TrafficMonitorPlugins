#pragma once
#include "PluginInterface.h"
#include <string>
#include <vector>

class CQuickNoteItem : public IPluginItem
{
public:
    CQuickNoteItem();
    virtual ~CQuickNoteItem();

    virtual const wchar_t* GetItemName() const override;
    virtual const wchar_t* GetItemId() const override;
    virtual const wchar_t* GetItemLableText() const override;
    virtual const wchar_t* GetItemValueText() const override;
    virtual const wchar_t* GetItemValueSampleText() const override;
    virtual bool IsCustomDraw() const override;
    virtual int GetItemWidth() const override;
    virtual void DrawItem(void* hDC, int x, int y, int w, int h, bool dark_mode) override;
    virtual int OnMouseEvent(MouseEventType type, int x, int y, void* hWnd, int flag) override;
    virtual int OnKeboardEvent(int key, bool ctrl, bool shift, bool alt, void* hWnd, int flag) override;

    void ProcessCommand(const std::wstring& command);

private:
    std::vector<std::wstring> m_notes;
    std::wstring m_current_note;
    int m_width{};
    bool m_is_command_mode{ false };
    std::wstring m_command_buffer;

    bool ExecuteCommand(const std::wstring& command);
};