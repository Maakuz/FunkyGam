#pragma once
#include <functional>
#include <vector>
#include <string>
#include <sstream>
#include "Imgui/imgui.h"
#include "Imgui/misc/cpp/imgui_stdlib.h"

class ConsoleWindow
{
public:
    ~ConsoleWindow() {};
    static ConsoleWindow& get()
    {
        static ConsoleWindow wino;
        return wino;
    }


    void addCommand(std::string command, std::function<std::string(std::vector<std::string> args)> func)
    {
        //Todo: assert dupes
        commands.push_back(Command(command, func));
    }

    void addLog(std::string string, sf::Color color = sf::Color::White)
    {
        this->log.push_back(LogEntry(color, string));
    }

    void update()
    {
        const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        
        ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_WindowBg, colors.windowColor);
        ImGui::Begin("TurboConsole", 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize);
        ImGui::SetWindowSize(sf::Vector2f(500, 600));
        sf::Vector2i windowPos = ImGui::GetWindowPos();

        if (ImGui::BeginTabBar("TurboConsole"))
        {
            if (ImGui::BeginTabItem("The good stuff"))
            {


                ImGui::BeginChild("log", sf::Vector2i(0, -footer_height_to_reserve), false);
                for (const LogEntry& entry : log)
                {
                    ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Text, entry.color);
                    ImGui::Text("%s\n", entry.text.c_str());
                    ImGui::PopStyleColor();
                }

                ImGui::EndChild();
                ImGui::Separator();

                this->updateInputBox();

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Color picker"))
            {
                static float col[4] = {0, 0, 0, 200};
                if (ImGui::ColorEdit4("Window", col))
                {
                    colors.windowColor = sf::Color(col[0] * 255, col[1] * 255, col[2] * 255, col[3] * 255);
                }

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
        ImGui::PopStyleColor();
    }
private:
    struct Command
    {
        std::string command;
        std::function<std::string(std::vector<std::string> args)> func;

        Command(std::string s, std::function<std::string(std::vector<std::string> args)> f)
        {
            this->command = s;
            this->func = f;
        }

        std::string operator()(std::vector<std::string> args)
        {
            return func(args);
        }
    };

    struct LogEntry
    {
        sf::Color color;
        std::string text;

        LogEntry(sf::Color col, std::string string)
        {
            color = col;
            text = string;
        }
    };

    struct Colors 
    {
        sf::Color errorColor;
        sf::Color commandColor;
        sf::Color infoColor;
        sf::Color windowColor;
    };

    std::vector<Command> commands;
    std::vector<LogEntry> log;
    std::vector<std::string> history;
    int historyPos;

    Colors colors;

    ConsoleWindow() 
    {
        historyPos = -1;

        addCommand("clearLog", [&](std::vector<std::string> args)->std::string {
            clearLog();
            return "Log cleared!"; });


        colors.commandColor = sf::Color(70, 255, 70);
        colors.errorColor = sf::Color(255, 50, 50);
        colors.infoColor = sf::Color::White;
        colors.windowColor = sf::Color(0, 0, 0, 200);
    }

    void updateInputBox()
    {
        ImGuiTextEditCallback callback = [](ImGuiTextEditCallbackData* data)->int //What even IS this?
        {
            ConsoleWindow* wadn = (ConsoleWindow*)data->UserData;
            return wadn->textEditCallback(data);
        };

        static std::string input = "";
        if (ImGui::InputText("command", &input,
            ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue |
            ImGuiInputTextFlags_::ImGuiInputTextFlags_CallbackHistory |
            ImGuiInputTextFlags_::ImGuiInputTextFlags_CallbackCompletion |
            ImGuiInputTextFlags_::ImGuiInputTextFlags_CallbackAlways
            , callback, (void*)this))
        {
            if (input[0])
                runCommand(input);

            input = "";
            historyPos = -1;
            ImGui::SetKeyboardFocusHere(-1);
        }
    }

    int textEditCallback(ImGuiInputTextCallbackData* data)
    {
        int prevPos = historyPos;
        switch (data->EventFlag)
        {
        case ImGuiInputTextFlags_::ImGuiInputTextFlags_CallbackHistory:

            if (data->EventKey == ImGuiKey_::ImGuiKey_UpArrow)
            {
                historyPos++;
                if (historyPos >= history.size())
                    historyPos--;
            }

            else
            {
                historyPos--;
                if (historyPos < 0)
                    historyPos = -1;
            }

            if (prevPos != historyPos)
            {
                int length = 0;
                if (historyPos == -1)
                    length = snprintf(data->Buf, size_t(data->BufSize), "");

                else
                    length = snprintf(data->Buf, size_t(data->BufSize), "%s", history[historyPos].c_str());
                
                data->BufTextLen = length;
                data->SelectionEnd = length;
                data->SelectionStart = length;
                data->CursorPos = length;
                data->BufDirty = true;
            }
            break;

        case ImGuiInputTextFlags_::ImGuiInputTextFlags_CallbackCompletion:
            std::string comp = data->Buf;

            if (comp.empty())
            {
                this->addLog("The possibilities are endless!");
                break;
            }

            std::vector<std::string> possibilities;

            for (const Command& command : commands)
            {
                if (command.command.compare(0, comp.size(), comp) == 0)
                    possibilities.push_back(command.command);
            }

            if (possibilities.empty())
            {
                this->addLog("No matches found :(");
                break;
            }

            std::sort(possibilities.begin(), possibilities.end(), sortByLength);

            this->addLog("Possibilities: ");
            for (std::string& possibility : possibilities)
                this->addLog("- " + possibility);

            

            int length = snprintf(data->Buf, size_t(data->BufSize), "%s", possibilities[0].c_str());

            data->BufTextLen = length;
            data->SelectionEnd = length;
            data->SelectionStart = length;
            data->CursorPos = length;
            data->BufDirty = true;
            break;
        }


        return 0;
    }

    void runCommand(std::string command)
    {
        std::string actualCommand;
        std::vector<std::string> args;
        std::stringstream stream(command);

        stream >> actualCommand;

        //Find command
        int found = -1;
        for (size_t i = 0; i < commands.size() && found == -1; i++)
        {
            if (actualCommand == commands[i].command)
                found = i;
        }

        if (found != -1)
        {
            while (!stream.eof())
            {
                std::string temp = "";
                stream >> temp;
                args.push_back(temp);
            }

            //Todo: limit maybe
            history.push_back(command);
            addLog(commands[found](args), colors.commandColor);
        }

        else
        {
            addLog( "Command \"" + actualCommand +"\" not found.", colors.errorColor);
        }
    }

    void clearLog()
    {
        this->log.clear();
    }

    static bool sortByLength(const std::string& a, const std::string& b)
    {
        return a.size() < b.size();
    }
};