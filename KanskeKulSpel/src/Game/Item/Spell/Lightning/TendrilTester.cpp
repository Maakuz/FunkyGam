#include "TendrilTester.h"
#include "Imgui/imgui.h"
#include "Game/Misc/VectorFunctions.h"

TendrilTester::TendrilTester()
{
    open = false;
}

void TendrilTester::update(float dt)
{
    if (open)
    {
        ImGui::Begin("Tendril Test Facility", &open);

        static float start[2] = { 400, 400 };
        static float end[2] = { 600, 400 };
        float col[4] = { tendril.data.color.r / 255.f, tendril.data.color.g / 255.f ,tendril.data.color.b / 255.f, tendril.data.color.a / 255.f };
        static bool spamUpdate = false;
        static bool spamFork = false;
        static float rotation = 0;

        ImGui::DragFloat2("start", start);
        ImGui::DragFloat2("end", end);
        if (ImGui::ColorEdit4("Color", col))
            tendril.data.color = sf::Color(col[0] * 255.f, col[1] * 255.f, col[2] * 255.f, col[3] * 255.f);

        ImGui::DragInt("Splits", &tendril.data.splits);
        if (tendril.data.splits < 0)
            tendril.data.splits = 0;

        ImGui::DragInt("sway", &tendril.data.sway);
        ImGui::DragInt("min", &tendril.data.min);
        ImGui::DragInt("max", &tendril.data.max);
        ImGui::DragInt("posMin", &tendril.data.forkMin);
        ImGui::DragInt("posMax", &tendril.data.forkMax);
        ImGui::DragFloat("Height", &tendril.data.peakHeight, 0.001f);
        ImGui::DragFloat("rotation", &rotation, 1.f);
        ImGui::DragInt("fork degrees", &tendril.data.angle, 1.f);
        ImGui::DragInt("Visible time", &tendril.data.visibleTime, 1.f);
        ImGui::DragFloat("Fadespeed", &tendril.data.fadeSpeed, 0.01f);
        ImGui::DragFloat("thickness", &tendril.data.thickness, 1.f);
        ImGui::InputInt("Algorithm", &tendril.data.timeAlgorithm);

        ImGui::Checkbox("Update", &spamUpdate);

        if (ImGui::Button("Generatre!"))
        {
            sf::Vector2f startVec(start[0], start[1]);
            sf::Vector2f stopVec(end[0], end[1]);

            stopVec = rotateBy(rotation, stopVec, startVec);

            tendril.generate(startVec, stopVec);
        }

        ImGui::End();

        if (spamUpdate)
            tendril.update(dt);
    }
}

void TendrilTester::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (open)
        target.draw(tendril, states);
}
