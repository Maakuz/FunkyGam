#include "ConsoleWindow.h"
#include <stdarg.h>
#include <algorithm>
#include <sstream>
#include "Imgui/misc/cpp/imgui_stdlib.h"

ConsoleWindow::ConsoleWindow()
{
    historyPos = -1;

    addCommand("clearLog", [&](Arguments args)->std::string {
        clearLog();
        return "Log cleared!"; });

    addCommand("listAll", [&](Arguments args)->std::string {
        listAllCommands();
        return "That's all folks!"; });


    colors.commandColor = sf::Color(70, 255, 70);
    colors.errorColor = sf::Color(255, 50, 50);
    colors.infoColor = sf::Color::White;
    colors.fromOutsideColor = sf::Color(255, 0, 255);
    colors.windowColor = sf::Color(0, 0, 0, 200);
    addLog("Use command \"listAll\" for a list of all commands.", colors.infoColor);
}

inline void ConsoleWindow::updateInputBox()
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

inline int ConsoleWindow::textEditCallback(ImGuiInputTextCallbackData* data)
{
    switch (data->EventFlag)
    {
    case ImGuiInputTextFlags_::ImGuiInputTextFlags_CallbackHistory:
        textEditCallbackHistory(data);

        break;

    case ImGuiInputTextFlags_::ImGuiInputTextFlags_CallbackCompletion:
        textEditCallbackComplete(data);
        break;
    }


    return 0;
}

inline int ConsoleWindow::textEditCallbackHistory(ImGuiInputTextCallbackData* data)
{
    int prevPos = historyPos;

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

    return 0;
}

inline int ConsoleWindow::textEditCallbackComplete(ImGuiInputTextCallbackData* data)
{
    std::string comp = data->Buf;

    if (comp.empty())
    {
        this->addLog("The possibilities are endless!");
        this->addLog("Type 'listAll' for a list of all commands.");
        return 0;
    }

    std::vector<std::string> possibilities;

    for (const Command& command : commands)
    {
        if (lowercase(command.string).compare(0, comp.size(), lowercase(comp)) == 0)
            possibilities.push_back(command.string);
    }

    if (possibilities.empty())
    {
        this->addLog("No matches found :(");
        return 0;
    }

    std::sort(possibilities.begin(), possibilities.end(), sortByLength);

    this->addLog("Possibilities: ");
    for (std::string& possibility : possibilities)
        this->addLog("- " + possibility);

    bool death = false;
    std::string completion = "";
    for (size_t i = 0; i < possibilities[0].size() && !death; i++)
    {
        completion += possibilities[0][i];
        for (size_t j = 1; j < possibilities.size() && !death; j++)
        {
            if (lowercase(possibilities[j]).compare(0, completion.size(), lowercase(completion)) != 0)
                death = true;
        }
    }

    if (death)
        completion.erase(completion.end() - 1);

    int length = snprintf(data->Buf, completion.capacity() + 1, "%s", completion.c_str());

    data->BufTextLen = length;
    data->SelectionEnd = length;
    data->SelectionStart = length;
    data->CursorPos = length;
    data->BufDirty = true;

    return 0;
}

inline void ConsoleWindow::runCommand(std::string command)
{
    std::string actualCommand;
    std::vector<std::string> args;
    std::stringstream stream(command);

    stream >> actualCommand;

    //Find command
    int found = -1;
    for (size_t i = 0; i < commands.size() && found == -1; i++)
    {
        if (actualCommand == commands[i].string)
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

        auto it = history.begin();
        while (it != history.end())
        {
            if (it->compare(command) == 0)
                it = history.erase(it);

            else
                ++it;
        }

        //Todo: limit maybe
        history.insert(history.begin(), command);


        addLog(command + " - " + commands[found](args), colors.commandColor);
    }

    else
    {
        addLog("Command \"" + actualCommand + "\" not found.", colors.errorColor);
    }
}

inline void ConsoleWindow::addLog(std::string string, sf::Color color)
{
    if (log.size() >= LOG_MAX)
    {
        log.erase(log.begin());
    }

    this->log.push_back(LogEntry(color, string));
}

inline void ConsoleWindow::clearLog()
{
    this->log.clear();
}

inline void ConsoleWindow::listAllCommands()
{
    addLog("Listing all commands:", colors.infoColor);
    for (const Command& command : commands)
        addLog("- " + command.string, colors.infoColor);
}


void ConsoleWindow::addCommand(std::string command, std::function<std::string(Arguments args)> func)
{
    //Todo: assert dupes
    commands.push_back(Command(command, func));
}

void ConsoleWindow::printText(std::string text)
{
    addLog(text, colors.fromOutsideColor);
}

void ConsoleWindow::printf(const char* format, ...)
{
    va_list listPtr;
    va_start(listPtr, format);

    std::string str(format);
    std::string finalStr;
    for (int i = 0; i < str.size(); i++)
    {
        if (str[i] == '%')
        {
            char type = str[++i];

            if (type == 'f')
            {
                finalStr += std::to_string(va_arg(listPtr, double));
            }
            else if (type == 'd')
                finalStr += std::to_string(va_arg(listPtr, int));

            else if (type == 'b')
                finalStr += std::to_string(va_arg(listPtr, bool));

            else if (type == 's')
                finalStr += va_arg(listPtr, const char*);
        }

        else
            finalStr += str[i];
    }

    va_end(listPtr);

    printText(finalStr);
}

void ConsoleWindow::update(bool setFocusOnTextbox)
{
    const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();

    ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_WindowBg, colors.windowColor);
    ImGui::Begin("Turbosole", 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize);
    ImGui::SetWindowSize(sf::Vector2f(500, 600));
    sf::Vector2i windowPos = ImGui::GetWindowPos();

    if (ImGui::BeginTabBar("TurboConsole"))
    {
        if (ImGui::BeginTabItem("The good stuff"))
        {


            ImGui::BeginChild("log", sf::Vector2i(0, -footer_height_to_reserve), false);
            for (int i = 0; i < log.size(); i++)
            {
                ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Text, log[i].color);
                ImGui::TextWrapped("%d: %s\n", i, log[i].text.c_str());
                ImGui::PopStyleColor();
            }

            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                ImGui::SetScrollHereY(1.f);

            ImGui::EndChild();
            ImGui::Separator();

            if (setFocusOnTextbox)
                ImGui::SetKeyboardFocusHere();

            this->updateInputBox();

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Color picker"))
        {
            static float col[4] = { 0, 0, 0, 200 };
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

