#pragma once
#include <functional>
#include <vector>
#include <string>
#include <sstream>
#include "Imgui/imgui.h"
#include "Imgui/misc/cpp/imgui_stdlib.h"
#include "Misc/KeyboardState.h"

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

    void update()
    {
        const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        
        ImGui::Begin("TurboConsole", 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize);
        ImGui::SetWindowSize(sf::Vector2f(500, 600));
        ImGui::BeginChild("log", sf::Vector2i(0, -footer_height_to_reserve), false);
        for (std::string& str : log)
            ImGui::Text("%s\n", str.c_str());

        ImGui::EndChild();
        ImGui::Separator();
        ImGuiTextEditCallback callback = [](ImGuiTextEditCallbackData* data)->int //What even IS this?
        {
            ConsoleWindow* wadn = (ConsoleWindow*)data->UserData;
            return wadn->textEditCallback(data);
        };

        static std::string input = ""; 
        if (ImGui::InputText("command", &input, 
            ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue | 
            ImGuiInputTextFlags_::ImGuiInputTextFlags_CallbackHistory |
            ImGuiInputTextFlags_::ImGuiInputTextFlags_CallbackCompletion
            , callback, (void*)this))
        {
            if (input[0])
                log.push_back(runCommand(input));

            input = "";
            historyPos = -1;
            ImGui::SetKeyboardFocusHere(-1);
        }
        ImGui::End();

    }

    int textEditCallback(ImGuiInputTextCallbackData* data)
    {
        switch (data->EventFlag)
        {
        case ImGuiInputTextFlags_::ImGuiInputTextFlags_CallbackHistory:

            if (data->EventKey == ImGuiKey_::ImGuiKey_UpArrow)
            {

            }
            break;

        case ImGuiInputTextFlags_::ImGuiInputTextFlags_CallbackCompletion:
            break;
        }


        return 0;
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
    std::vector<Command> commands;
    std::vector<std::string> log;
    std::vector<std::string> history;
    int historyPos;

    ConsoleWindow() 
    {
        historyPos = -1;
    }

    std::string runCommand(std::string command)
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
            return commands[found](args);
        }

        else
        {
            return "Command \"" + actualCommand +"\" not found." ;
        }
    }

    
};