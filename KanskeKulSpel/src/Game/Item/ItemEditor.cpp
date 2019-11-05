#include "ItemEditor.h"
#include "Game/Misc/Definitions.h"
#include "Misc/ConsoleWindow.h"
#include "Game/Handlers/TextureHandler.h"
#include "Game/Particles/ParticleHandler.h"
#include "Imgui/imgui.h"
#include "Imgui/misc/cpp/imgui_stdlib.h"

ItemEditor::ItemEditor()
{
    this->open = false;
    currentItem = -1;
}

ItemEditor::~ItemEditor()
{
    clearItems();
}

void ItemEditor::openWindow()
{
    open = true;

    readItems();
}

void ItemEditor::update()
{
    ImGui::Begin("Item Editor", &this->open);
    if (!items.empty())
    {
        if (ImGui::BeginCombo("Select item", this->items[this->currentItem]->getName().c_str()))
        {
            for (int i = 0; i < items.size(); i++)
            {
                if (ImGui::Selectable(items[i]->getName().c_str()))
                    this->currentItem = i;
            }


            ImGui::EndCombo();
        }

        Throwable* throwable = dynamic_cast<Throwable*>(items[currentItem]);
        if (throwable)
        {
            showThrowableData(throwable);
        }

        else
            showItemData(items[currentItem]);
    }

    if (ImGui::Button("save"))
        writeItems();

    if (ImGui::Button("Create Item"))
    {
        Item* item = new Item(sf::Vector2f(), items[currentItem]->getTexture());
        this->items.push_back(item);
        item->setID(this->items.size() - 1);
        this->currentItem = this->items.size() - 1;
    }

    ImGui::SameLine();

    if (ImGui::Button("Create Throwable"))
    {
        Throwable* item = new Throwable(sf::Vector2f(), sf::Vector2f(), items[currentItem]->getTexture());
        this->items.push_back(item);
        item->setID(this->items.size() - 1);
        item->setUseable(true);
        this->currentItem = this->items.size() - 1;
    }

    if (ImGui::Button("Delete"))
    {
        this->items.erase(items.begin() + currentItem);
        this->currentItem = this->items.size() - 1;
    }
    ImGui::End();
}

void ItemEditor::showItemData(Item* item)
{
    ImGui::Text("%s", ("ItemID: " + std::to_string(item->getID())).c_str());

    const std::vector<sf::Texture>* textures = TextureHandler::get().getTextureVec();
    int currentTexture = TextureHandler::get().getIDForTexture(item->getTexture());
    if (ImGui::BeginCombo("Select texture", TextureHandler::get().getTextureName(currentTexture).c_str()))
    {
        
        for (int i = 0; i < textures->size(); i++)
        {
            if (ImGui::Selectable(TextureHandler::get().getTextureName(i).c_str()))
                item->setTexture(&textures->at(i));
        }


        ImGui::EndCombo();
    }

    const std::vector<Emitter>* emitters = ParticleHandler::getEmitterTemplates();
    int currentEmitter = item->getEmitterID();
    if (ImGui::BeginCombo("Select emitter", ParticleHandler::getEmitterName(currentEmitter).c_str()))
    {

        for (int i = 0; i < emitters->size(); i++)
        {
            if (ImGui::Selectable(ParticleHandler::getEmitterName(i).c_str()))
                item->setEmitterID(i);
        }

        if (ImGui::Selectable("None"))
            item->setEmitterID(-1);

        ImGui::EndCombo();
    }


    std::string name = item->getName();
    ImGui::InputText("Name", &name);
    item->setName(name);

    int stackLimit = item->getStackLimit();
    ImGui::InputInt("Stack limit", &stackLimit);
    item->setStackLimit(stackLimit);
}

void ItemEditor::showThrowableData(Throwable* item)
{
    showItemData(item);
    ImGui::Separator();
    float mass = item->getMass();
    ImGui::DragFloat("Mass", &mass, 0.01, 0, 1000);
    item->setMass(mass);

    int armingTime = item->getArmingTime();
    ImGui::DragInt("Arming time, milliseconds", &armingTime, 0.01, 0, 100000);
    item->setArmingTime(armingTime);

    float bounce = item->getBounce();
    ImGui::DragFloat("Bounce", &bounce, 0.01, 0, 1);
    item->setBounce(bounce);

    bool detonateOnImpact = item->isDetonatingOnImpact();
    ImGui::Checkbox("DetonateOnImpact", &detonateOnImpact);
    item->setDetonatingOnImpact(detonateOnImpact);

    int damage = item->getDamage();
    ImGui::DragInt("Collision damage", &damage, 1, 0, 100000);
    item->setDamage(damage);

    const std::vector<Emitter>* emitters = ParticleHandler::getEmitterTemplates();
    int currentEmitter = item->getParticleEffectID();
    if (ImGui::BeginCombo("Select detonation emitter", ParticleHandler::getEmitterName(currentEmitter).c_str()))
    {

        for (int i = 0; i < emitters->size(); i++)
        {
            if (ImGui::Selectable(ParticleHandler::getEmitterName(i).c_str()))
                item->setParticleEffectID(i);
        }

        ImGui::EndCombo();
    }

    ImGui::Separator();
    const std::string types[2] = {"Basic", "Flash"};
    ExplosionType selected = item->getExplosion().type;
    if (ImGui::BeginCombo("Explosion type", types[selected].c_str()))
    {

        for (int i = 0; i < 2; i++)
        {
            if (ImGui::Selectable(types[i].c_str()))
                item->getExplosionPtr()->type = ExplosionType(i);
        }

        ImGui::EndCombo();
    }

    int radius = item->getExplosion().radius;
    ImGui::DragInt("Exposion radius", &radius, 1, 0, 100000);
    item->getExplosionPtr()->radius = radius;

    float falloff = item->getExplosion().falloff;
    ImGui::DragFloat("Exposion falloff", &falloff, 0.01, 0, 1);
    item->getExplosionPtr()->falloff = falloff;
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("How far in percentages the blast deal full damage");

    int bombdamage = item->getExplosion().damage;
    ImGui::DragInt("Exposion damage, negative damage heals", &bombdamage, 1, -100000, 100000);
    item->getExplosionPtr()->damage = bombdamage;
}

void ItemEditor::readItems()
{
    clearItems();
    std::ifstream file(DATA_PATH "Items.mop");

    if (!file.is_open())
    {
        printCon("File not found, editing unavailible");
        closeWindow();
    }

    else
    {
        while (!file.eof())
        {
            std::string itemType;
            std::string trash;
            int id;
            int texID;

            file >> itemType;
            file >> trash >> id;
            file >> trash >> texID;

            if (itemType == "[Throwable]")
            {
                Throwable* item = new Throwable(sf::Vector2f(), sf::Vector2f(), TextureHandler::get().getTexture(texID));
                item->setID(id);
                file >> *item;
                this->items.push_back(item);
            }

            if (itemType == "[Item]")
            {
                Item* item = new Item(sf::Vector2f(), TextureHandler::get().getTexture(texID));
                item->setID(id);
                file >> *item;
                this->items.push_back(item);
            }


        }



        file.close();
    }

    this->currentItem = this->items.size() - 1;
}

void ItemEditor::writeItems()
{
    std::ofstream file(DATA_PATH "Items.mop");

    if (!file.is_open())
    {
        printCon("File not found, editing unavailible");
        closeWindow();
    }

    for (Item* item : items)
    {
        Throwable* ptr = dynamic_cast<Throwable*>(item);
        if (ptr)
        {
            file << "[Throwable]\n";
            file << "ID: " << ptr->getID() << "\n";
            file << "TexID: " << TextureHandler::get().getIDForTexture(ptr->getTexture()) << "\n";
            file << *ptr << "\n";
        }

        else
        {
            file << "[Item]\n";
            file << "ID: " << item->getID() << "\n";
            file << "TexID: " << TextureHandler::get().getIDForTexture(item->getTexture()) << "\n";
            file << *item << "\n";
        }
    }
}

void ItemEditor::clearItems()
{
    for (Item* item : items)
        delete item;

    items.clear();
}
