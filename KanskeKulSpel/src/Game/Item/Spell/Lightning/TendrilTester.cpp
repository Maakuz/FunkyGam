#include "TendrilTester.h"
#include "Imgui/imgui.h"
#include "Game/Misc/VectorFunctions.h"

TendrilTester::TendrilTester()
{
    open = false;
}

void TendrilTester::update()
{
    if (open)
    {
        ImGui::Begin("Tendril Test Facility", &open);

        static float start[2] = { 400, 400 };
        static float end[2] = { 600, 400 };
        float col[4] = { tendril.color.r / 255.f, tendril.color.g / 255.f ,tendril.color.b / 255.f, tendril.color.a / 255.f };
        static float thickness = 32;
        static bool spamUpdate = false;
        static int splits = 5;
        static float rotation = 0;

        ImGui::DragFloat2("start", start);
        ImGui::DragFloat2("end", end);
        if (ImGui::ColorEdit4("Color", col))
            tendril.color = sf::Color(col[0] * 255.f, col[1] * 255.f, col[2] * 255.f, col[3] * 255.f);

        ImGui::DragInt("Splits", &splits);
        if (splits < 0)
            splits = 0;

        ImGui::DragInt("sway", &tendril.sway);
        ImGui::DragFloat("Height", &tendril.peakHeight, 0.001f);
        ImGui::DragFloat("rotation test", &rotation, 1.f);

        ImGui::Checkbox("Update", &spamUpdate);

        ImGui::End();

        if (spamUpdate)
        {
            sf::Vector2f startVec(start[0], start[1]);
            sf::Vector2f stopVec(end[0], end[1]);

            stopVec = rotateBy(rotation, stopVec, startVec);

            tendril.generateLightning(startVec, stopVec, thickness, splits);
        }
    }
}

void TendrilTester::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (open)
        target.draw(tendril, states);
}
