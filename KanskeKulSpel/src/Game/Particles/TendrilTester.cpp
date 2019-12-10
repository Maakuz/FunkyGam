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

        Tendril::InitGenData* data = tendril.getDataPtr();

        static float start[2] = { 400, 400 };
        static float end[2] = { 600, 400 };
        float col[4] = { data->color.r / 255.f, data->color.g / 255.f ,data->color.b / 255.f, data->color.a / 255.f };
        static bool spamUpdate = false;
        static bool spamFork = false;
        static float rotation = 0;

        ImGui::DragFloat2("start", start);
        ImGui::DragFloat2("end", end);
        if (ImGui::ColorEdit4("Color", col))
            data->color = sf::Color(col[0] * 255.f, col[1] * 255.f, col[2] * 255.f, col[3] * 255.f);

        ImGui::DragInt("Splits", &data->splits);
        if (data->splits < 0)
            data->splits = 0;

        ImGui::DragInt("sway", &data->sway);
        ImGui::DragInt("min", &data->min);
        ImGui::DragInt("max", &data->max);
        ImGui::DragInt("posMin", &data->forkMin);
        ImGui::DragInt("posMax", &data->forkMax);
        ImGui::DragFloat("Height", &data->peakHeight, 0.001f);
        ImGui::DragFloat("rotation", &rotation, 1.f);
        ImGui::DragInt("fork degrees", &data->angle, 1.f);
        ImGui::DragInt("Visible time", &data->visibleTime, 1.f);
        ImGui::DragFloat("Fadespeed", &data->fadeSpeed, 0.01f);
        ImGui::DragFloat("thickness", &data->thickness, 1.f);
        ImGui::InputInt("Algorithm", &data->timeAlgorithm);

        ImGui::Checkbox("repeating", &data->repeating);
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
