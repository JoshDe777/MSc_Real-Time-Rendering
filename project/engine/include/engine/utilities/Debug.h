#pragma once

#include <iostream>
#include <fstream>
#include <windows.h>
#include <array>
#include <string>
#include <sstream>
#include <OpenGL/OpenGlInclude.h>

namespace EisEngine {
    using namespace std;

    enum LogPriority{ DebugP, InfoP, WarnP, ErrorP, FatalP};

    /**
     * @def DEBUG_INFO(message)
     *
     * Logs an informational message indicating the file and line number where the macro is called.
     *
     * @param message - std::string: A string to be displayed in the log message.
     */
    #ifndef DEBUG_INFO
    #define DEBUG_INFO(message) Debug::Info(__FILE__, __LINE__, message);
    #endif

    /**
     * @def DEBUG_LOG(message)
     *
     * Logs a message indicating the file and line number where the macro is called.
     *
     * @param message - std::string: A string to be displayed in the log message.
     */
    #ifndef DEBUG_LOG
    #define DEBUG_LOG(message) Debug::Log(__FILE__, __LINE__, message);
    #endif

    /**
     * @def DEBUG_WARN(message)
     *
     * Logs a warning message indicating the file and line number where the macro is called.
     *
     * @param message - std::string: A string to be displayed in the warning message.
     */
    #ifndef DEBUG_WARN
    #define DEBUG_WARN(message) Debug::Warn(__FILE__, __LINE__, message);
    #endif

    /**
     * @def DEBUG_ERROR(message)
     *
     * Logs an error message indicating the file and line number where the macro is called.
     * \n This **does not** interrupt the game, and should be used e.g. in error catching.
     *
     * @param message - std::string: A string to be displayed in the error message.
     */
    #ifndef DEBUG_ERROR
    #define DEBUG_ERROR(message) Debug::Error(__FILE__, __LINE__, message);
    #endif


    /**
     * @def DEBUG_RUNTIME_ERROR(message)
     *
     * Logs an error message indicating the file and line number where the macro is called.
     * \n This **does** interrupt the game when called.
     *
     * @param message - std::string: A string to be displayed in the error message.
     */
    #ifndef DEBUG_RUNTIME_ERROR
    #define DEBUG_RUNTIME_ERROR(message) Debug::RuntimeError(__FILE__, __LINE__, message);
    #endif

    /// \n {DEPRECATED} A utility class used for debugging purposes -
    /// provides more practical console inputs than the standard library's console interactions.\n
    /// \n Deprecated - use provided macros instead (DEBUG_LOG(message) instead of
    /// Debug::Log(__FILE__, __LINE__, message))
    class Debug {
    public:
        /// \n sends a specific line of text to the console.
        /// @param file - [use __FILE__] -> gives file information to the file the statement is called from.
        /// @param line - [use __LINE__] -> gives the line the statement was called from.
        /// @param text - a string to be sent to the console.
        static void Log(const char* file, int line, const std::string &text){
            CreateLog(DebugP, file, line, text);
        }

        /// \n sends an information message to the console.
        /// @param file - [use __FILE__] -> gives file information to the file the statement is called from.
        /// @param line - [use __LINE__] -> gives the line the statement was called from.
        /// @param text - a string to be displayed as information.
        static void Info(const char* file, int line, const std::string &text){
            CreateLog(InfoP, file, line, text);
        }

        /// \n sends a warning message to the console
        /// @param file - [use __FILE__] -> gives file information to the file the statement is called from.
        /// @param line - [use __LINE__] -> gives the line the statement was called from.
        /// @param warning - a string to be displayed as a warning message.
        static void Warn(const char* file, int line, const std::string &warning){
            CreateLog(WarnP, file, line, warning);
        }

        /// \n sends an error message to the console.
        /// @param file - [use __FILE__] -> gives file information to the file the statement is called from.
        /// @param line - [use __LINE__] -> gives the line the statement was called from.
        /// @param errorText - a string to be displayed as the explanation behind the error message.
        static void Error(const char* file, int line, const std::string &errorText){
            CreateLog(ErrorP, file, line, errorText);
        }

        /// \n throws a runtime error, crashing the game and logging the error message in the process.
        /// @param file - [use __FILE__] -> gives file information to the file the statement is called from.
        /// @param line - [use __LINE__] -> gives the line the statement was called from.
        /// @param err - the error message to be displayed.
        static void RuntimeError(const char* file, int line, const std::string &err){
            CreateLog(FatalP, file, line, err);
        }
    private:
        /// \n Compiles the log info to a message in the console.
        static void CreateLog(LogPriority priority,
                              const char* file,
                              int line,
                              const std::string &message){
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

            std::string time = GetTime();

            std::stringstream logStream;
            logStream << "["<< file <<"("<< line << ")]:\n";
            logStream << "[" << PriorityToString(priority) << "] - (" << time << "): ";
            logStream << message;

            switch(priority){
                case DebugP:
                case InfoP:
                    std::cout << logStream.str() << std::endl;
                    break;
                case WarnP:
                    SetConsoleTextAttribute(hConsole, 6);
                    std::cout << logStream.str() << std::endl;
                    break;
                case ErrorP:
                    SetConsoleTextAttribute(hConsole, 12);
                    std::cout << logStream.str() << std::endl;
                    break;
                case FatalP:
                    SetConsoleTextAttribute(hConsole, 12);
                    std::cout << logStream.str() << std::endl;
                    throw std::runtime_error(logStream.str());
            }

            SetConsoleTextAttribute(hConsole, 7);
        }

        /// \n Maps every log priority to a string.
        static std::string PriorityToString(LogPriority p){
            switch (p) {
                case DebugP:
                    return "Debug";
                case InfoP:
                    return "Info";
                case WarnP:
                    return "Warning";
                case ErrorP:
                    return "Error";
                case FatalP:
                    return "Fatal";
            }
            return "Unexpected log priority ";
        }

        /// \n Get current system time.
        static std::string GetTime(){
            time_t currentTime;
            struct tm* localTime;

            time(&currentTime);
            localTime = localtime(&currentTime);

            const std::array<int, 3> time{localTime->tm_hour, localTime->tm_min, localTime->tm_sec};

            return CompileTimeToString(time);
        }

        /// \n Converts an array of time values to a string displaying hh:mm:ss.
        static std::string CompileTimeToString(const std::array<int, 3> &time)
        { return to_string(time[0]) + ":" + to_string(time[1]) + ":" + to_string(time[2]);}
    };

    inline void GLCheckError(const char* context, const std::string& entityName) {
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            DEBUG_ERROR("OpenGL Error [" + entityName + "::" + std::string(context) + "]: " + std::to_string(error))
        }
    }
}
