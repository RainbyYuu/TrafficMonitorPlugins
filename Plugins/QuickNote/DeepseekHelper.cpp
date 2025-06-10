#include "pch.h"
#include "DeepSeekHelper.h"
#include <windows.h>
#include <winhttp.h>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <json.hpp>
#include <codecvt>

#pragma comment(lib, "winhttp.lib")

using json = nlohmann::json;

// 你的 API Key
const std::wstring API_KEY = L"sk-cd1ffbfafa20447890925c18875045a9";
const std::wstring HOST = L"api.deepseek.com";
const std::wstring ENDPOINT = L"/chat/completions";

std::wstring Utf8ToWide(const std::string& str)
{
    wstring result;
    int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), NULL, 0);
    if (len < 0)return result;
    wchar_t* buffer = new wchar_t[len + 1];
    if (buffer == NULL)return result;
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), buffer, len);
    buffer[len] = '\0';
    result.append(buffer);
    delete[] buffer;
    return result;
}

std::string WideToUtf8(const std::wstring& wstr) {
    string result;
    int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
    if (len <= 0)return result;
    char* buffer = new char[len + 1];
    if (buffer == NULL)return result;
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
    buffer[len] = '\0';
    result.append(buffer);
    delete[] buffer;
    return result;
}


std::string to_utf8(std::wstring& wide_string)
{
    static std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
    return utf8_conv.to_bytes(wide_string);
}

std::wstring DeepSeekHelper::GenerateSummary(const std::wstring& content)
{
    //std::string utf8_content = WideToUtf8(content);
    std::wstring tmp = L" 请为以下笔记生成一段简洁的中文摘要（5字左右）:\n" + content;
    //std::string prompt = WideToUtf8(tmp);
    std::string prompt = to_utf8(tmp);

    json request_body = {
        {"model", "deepseek-chat"},
        {"messages", json::array({
            {
                {"role", "user"},
                {"content", prompt}
            }
        })},
        {"temperature", 0.5}
    };

    std::string body_str = request_body.dump();
    // 不需要转换回宽字符，直接使用UTF-8字符串

    HINTERNET hSession = WinHttpOpen(L"QuickNote/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession)
        return L"[错误] 无法创建会话";

    HINTERNET hConnect = WinHttpConnect(hSession, HOST.c_str(),
        INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect)
    {
        WinHttpCloseHandle(hSession);
        return L"[错误] 无法连接服务器";
    }

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", ENDPOINT.c_str(),
        nullptr, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE);
    if (!hRequest)
    {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return L"[错误] 创建请求失败";
    }

    std::wstring headers =
        L"Content-Type: application/json\r\n"
        L"Authorization: Bearer " + API_KEY + L"\r\n";

    // 这里是修复的关键 - 正确处理UTF-8字符串
    BOOL result = WinHttpSendRequest(hRequest,
        headers.c_str(), (DWORD)-1L,
        (LPVOID)body_str.c_str(),    // 使用UTF-8字符串，不是宽字符串
        (DWORD)body_str.length(),    // 不乘以sizeof(wchar_t)
        (DWORD)body_str.length(), 0); // 同上

    if (!result)
    {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return L"[错误] 请求发送失败";
    }

    WinHttpReceiveResponse(hRequest, nullptr);

    DWORD dwSize = 0;
    std::string response_str;

    do
    {
        DWORD dwDownloaded = 0;
        WinHttpQueryDataAvailable(hRequest, &dwSize);
        if (dwSize == 0)
            break;

        std::vector<char> buffer(dwSize + 1);
        ZeroMemory(buffer.data(), dwSize + 1);

        WinHttpReadData(hRequest, buffer.data(), dwSize, &dwDownloaded);
        response_str.append(buffer.data(), dwDownloaded);
    } while (dwSize > 0);

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    try
    {
        auto j = json::parse(response_str);
        std::string summary = j["choices"][0]["message"]["content"];
        return Utf8ToWide(summary);
    }
    catch (...)
    {
        return L"[错误] 响应解析失败: " + Utf8ToWide(response_str);
    }
}