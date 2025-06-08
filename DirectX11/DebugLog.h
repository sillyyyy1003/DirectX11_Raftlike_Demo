#pragma once
#include <fstream>
#include <string>
#include <format>

namespace Log
{
    //メッセージのタイプ
    enum class Type :int
    {
        LOG_INFO = 0,
        LOG_WARNING = 1,
        LOG_ERROR = 2,
    };
}

/// @brief 情報出力
class DebugLog
{
private:
    DebugLog();
    ~DebugLog() = default;

private:
    /// @brief シングルトンインスタンス取得
    static DebugLog& GetInstance();

    /// @brief メッセージ出力
    /// @param message メッセージ 
    /// @param type メッセージの種類
    void LogOutput(const std::string& message, Log::Type type);


    std::ofstream ofs;

public:
    /// @brief 初期化
    /// @param _fileName 出力ファイル名 
    /// @return 初期化成功か否か
    bool Init(const char* _fileName);

    template<class... Args>
    static void Log(std::string_view rt_fmt_str, Args && ... args)
    {

        std::string str = std::vformat(rt_fmt_str, std::make_format_args(args...));
        DebugLog::GetInstance().LogOutput(str, Log::Type::LOG_INFO);


    };

    template<class... Args>
    static void LogWarning(std::string_view rt_fmt_str, Args && ... args)
    {

        std::string str = std::vformat(rt_fmt_str, std::make_format_args(args...));
        DebugLog::GetInstance().LogOutput(str, Log::Type::LOG_WARNING);

    };

    template<class... Args>
    static void LogError(std::string_view rt_fmt_str, Args && ... args)
    {

        std::string str = std::vformat(rt_fmt_str, std::make_format_args(args...));
        DebugLog::GetInstance().LogOutput(str, Log::Type::LOG_ERROR);

    };
};