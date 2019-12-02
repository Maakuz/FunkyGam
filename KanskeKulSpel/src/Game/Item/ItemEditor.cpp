#include "ItemEditor.h"
#include "Game/Misc/Definitions.h"
#include "Misc/ConsoleWindow.h"
#include "Game/Handlers/TextureHandler.h"
#include "Game/Particles/ParticleHandler.h"
#include "Game/Item/Projectile/ProjectileHandler.h"
#include "Game/Misc/MouseState.h"
#include "Imgui/imgui.h"
#include "Imgui/misc/cpp/imgui_stdlib.h"
#include <fstream>

ItemEditor::ItemEditor()
{
    this->open = false;
    this->currentTab = Tab::items;
    currentItem = -1;
    currentSpell = -1;
    currentProjectile = -1;
}

ItemEditor::~ItemEditor()
{
    clearItems();
    clearSpells();
}

void ItemEditor::openWindow()
{
    open = true;

    readItems();
    readSpells();
    readProjectiles();
}

void ItemEditor::update(float dt, sf::Vector2f mouseWorldPos)
{
    ImGui::Begin("Item Editor", &this->open);

    if (ImGui::BeginTabBar("tabbaritems"))
    {
        if (ImGui::BeginTabItem("Items"))
        {
            currentTab = Tab::items;
            updateItems(dt, mouseWorldPos);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Spells"))
        {
            currentTab = Tab::spells;
            updateSpells(dt, mouseWorldPos);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Projectiles"))
        {
            currentTab = Tab::projectiles;
            updateProjectiles(dt, mouseWorldPos);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
    ImGui::End();
}

void ItemEditor::updateItems(float dt, sf::Vector2f mouseWorldPos)
{
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
        Tome* tome = dynamic_cast<Tome*>(items[currentItem]);
        if (throwable)
            showThrowableData(throwable);

        else if (tome)
            showTomeData(tome);

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

    if (ImGui::Button("Create Tome"))
    {
        Tome* item = new Tome(sf::Vector2f(), items[currentItem]->getTexture());
        this->items.push_back(item);
        item->setID(this->items.size() - 1);
        item->setUseable(true);
        this->currentItem = this->items.size() - 1;
    }

    ImGui::SameLine();

    if (ImGui::Button("Create Throwable"))
    {
        Throwable* item = new Throwable(sf::Vector2f(), items[currentItem]->getTexture(), (sf::Vector2f)items[currentItem]->getTexture()->getSize());
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

}

void ItemEditor::updateSpells(float dt, sf::Vector2f mouseWorldPos)
{
    if (!spells.empty())
    {
        if (ImGui::BeginCombo("Select spell", this->spells[this->currentSpell]->getName().c_str()))
        {
            for (int i = 0; i < spells.size(); i++)
            {
                if (ImGui::Selectable(spells[i]->getName().c_str()))
                    this->currentSpell = i;
            }


            ImGui::EndCombo();
        }

        Fireball* fireball = dynamic_cast<Fireball*>(spells[currentSpell]);
        if (fireball)
            showFireballData(fireball);
       

    }

    if (ImGui::Button("save"))
        writeSpells();
}

void ItemEditor::updateProjectiles(float dt, sf::Vector2f mouseWorldPos)
{
    if (!projectiles.empty())
    {
        if (ImGui::BeginCombo("Select projectile", this->projectileNames[this->currentProjectile].c_str()))
        {
            for (int i = 0; i < projectiles.size(); i++)
            {
                if (ImGui::Selectable(projectileNames[i].c_str()))
                {
                    this->currentProjectile = i;
                    ProjectileHandler::addProjectile(currentProjectile, mouseWorldPos, sf::Vector2f(0, -1), nullptr);
                }
            }


            ImGui::EndCombo();
        }

        if (MOUSE::MouseState::isButtonClicked(sf::Mouse::Right))
        {
            ProjectileHandler::addProjectile(currentProjectile, mouseWorldPos, sf::Vector2f(0, -1), nullptr);
        }

        ImGui::Text("Index: %d", currentProjectile);
        showProjectileData(&projectiles[currentProjectile], &projectileNames[currentProjectile]);
    }

    if (ImGui::Button("Create Light Projectile"))
    {
        LightProjectile projectile;
        this->projectiles.push_back(projectile);
        this->projectileNames.push_back("Temp name. Please change me!");
        this->currentProjectile = this->projectiles.size() - 1;
    }

    if (ImGui::Button("save"))
        writeProjectiles();
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

void ItemEditor::showExplosionData(Explosion* explosion)
{
    ImGui::Separator();
    ExplosionType selected = explosion->type;
    if (ImGui::BeginCombo("Explosion type", ExplosionStrings::types[selected].c_str()))
    {

        for (int i = 0; i < ExplosionStrings::TYPE_COUNT; i++)
        {
            if (ImGui::Selectable(ExplosionStrings::types[i].c_str()))
                explosion->type = ExplosionType(i);
        }

        ImGui::EndCombo();
    }

    ImGui::DragFloat("Exposion radius", &explosion->radius, 1, 0, 100000);

    ImGui::DragFloat("Exposion falloff", &explosion->falloff, 0.01, 0, 1);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("How far in percentages the blast deal full damage");

    ImGui::DragInt("Exposion damage, negative damage heals", &explosion->damage, 1, -100000, 100000);
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

    showExplosionData(item->getExplosionPtr());
}

void ItemEditor::showTomeData(Tome* item)
{
    showItemData(item);

    std::string spell = item->getSpell();
    ImGui::InputText("Spell name", &spell);
    item->setSpell(spell);

    const std::vector<Emitter>* emitters = ParticleHandler::getEmitterTemplates();
    int currentEmitter = item->getChannelEmitter();
    if (ImGui::BeginCombo("Select channell emitter", ParticleHandler::getEmitterName(currentEmitter).c_str()))
    {

        for (int i = 0; i < emitters->size(); i++)
        {
            if (ImGui::Selectable(ParticleHandler::getEmitterName(i).c_str()))
                item->setChannelEmitter(i);
        }

        ImGui::EndCombo();
    }
}

void ItemEditor::showFireballData(Fireball* fireball)
{
    std::string name = fireball->getName();
    ImGui::InputText("Name", &name);
    fireball->setName(name);

    const std::vector<Emitter>* emitters = ParticleHandler::getEmitterTemplates();
    int currentTrailEmitter = fireball->getTrailEmitterID();
    if (ImGui::BeginCombo("Select trail emitter", ParticleHandler::getEmitterName(currentTrailEmitter).c_str()))
    {

        for (int i = 0; i < emitters->size(); i++)
        {
            if (ImGui::Selectable(ParticleHandler::getEmitterName(i).c_str()))
                fireball->setTrailEmitterID(i);
        }

        ImGui::EndCombo();
    }

    currentTrailEmitter = fireball->getFullTrailEmitterID();
    if (ImGui::BeginCombo("Select big trail emitter", ParticleHandler::getEmitterName(currentTrailEmitter).c_str()))
    {

        for (int i = 0; i < emitters->size(); i++)
        {
            if (ImGui::Selectable(ParticleHandler::getEmitterName(i).c_str()))
                fireball->setFullTrailEmitterID(i);
        }

        ImGui::EndCombo();
    }

    int currentImpactEmitter = fireball->getImpactEmitterID();
    if (ImGui::BeginCombo("Select impact emitter", ParticleHandler::getEmitterName(currentImpactEmitter).c_str()))
    {

        for (int i = 0; i < emitters->size(); i++)
        {
            if (ImGui::Selectable(ParticleHandler::getEmitterName(i).c_str()))
                fireball->setImpactEmitterID(i);
        }

        ImGui::EndCombo();
    }


    currentImpactEmitter = fireball->getFullImpactEmitterID();
    if (ImGui::BeginCombo("Select big impact emitter", ParticleHandler::getEmitterName(currentImpactEmitter).c_str()))
    {

        for (int i = 0; i < emitters->size(); i++)
        {
            if (ImGui::Selectable(ParticleHandler::getEmitterName(i).c_str()))
                fireball->setFullImpactEmitterID(i);
        }

        ImGui::EndCombo();
    }

    int damage = fireball->getDamage();
    ImGui::DragInt("Damage", &damage, 1, 0, 10000);
    fireball->setDamage(damage);

    float maxTravel = fireball->getMaxTravelDistance();
    ImGui::DragFloat("Maximum distance", &maxTravel, 1, 0, 100000);
    fireball->setMaxTravelDistance(maxTravel);

    float tops = fireball->getTopSpeed();
    ImGui::DragFloat("Top speed", &tops, 0.001, 0, 100000);
    fireball->setTopSpeed(tops);

    int minCharge = fireball->getMinCharge();
    ImGui::DragInt("Minimum charge time", &minCharge, 1, 0, 10000);
    fireball->setMinCharge(minCharge);

    int maxCharge = fireball->getMaxCharge();
    ImGui::DragInt("Maximum charge time", &maxCharge, 1, 0, 10000);
    fireball->setMaxCharge(maxCharge);

    showExplosionData(fireball->getExplosionPtr());
}

void ItemEditor::showProjectileData(LightProjectile* projectile, std::string* name)
{
    ImGui::InputText("Name", name);

    const std::vector<Emitter>* emitters = ParticleHandler::getEmitterTemplates();
    int currentLightEmitter = projectile->getLightEmitterID();
    if (ImGui::BeginCombo("Select light emitter", ParticleHandler::getEmitterName(currentLightEmitter).c_str()))
    {

        for (int i = 0; i < emitters->size(); i++)
        {
            if (ImGui::Selectable(ParticleHandler::getEmitterName(i).c_str()))
                projectile->setLightEmitterID(i);
        }

        ImGui::EndCombo();
    }

    currentLightEmitter = projectile->getInitialEmitterID();
    if (ImGui::BeginCombo("Select initial emitter", ParticleHandler::getEmitterName(currentLightEmitter).c_str()))
    {

        for (int i = 0; i < emitters->size(); i++)
        {
            if (ImGui::Selectable(ParticleHandler::getEmitterName(i).c_str()))
                projectile->setInitialEmitterID(i);
        }

        if (ImGui::Selectable("None"))
            projectile->setInitialEmitterID(-1);

        ImGui::EndCombo();
    }

    currentLightEmitter = projectile->getImpactEmitterID();
    if (ImGui::BeginCombo("Select impact emitter", ParticleHandler::getEmitterName(currentLightEmitter).c_str()))
    {

        for (int i = 0; i < emitters->size(); i++)
        {
            if (ImGui::Selectable(ParticleHandler::getEmitterName(i).c_str()))
                projectile->setImpactEmitterID(i);
        }

        if (ImGui::Selectable("None"))
            projectile->setImpactEmitterID(-1);

        ImGui::EndCombo();
    }

    int damage = projectile->getDamage();
    ImGui::DragInt("Damage", &damage, 1, 0, 10000);
    projectile->setDamage(damage);

    float velocity = projectile->getVelocity();
    ImGui::DragFloat("Velocity", &velocity, 1, 0, 100000);
    projectile->setVelocity(velocity);

    float size = projectile->getCollider().getSize().x;
    ImGui::DragFloat("size", &size, 1, 0, 100000);
    projectile->setSize(sf::Vector2f(size, size));
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
                Throwable* item = new Throwable(sf::Vector2f(), TextureHandler::get().getTexture(texID), (sf::Vector2f)TextureHandler::get().getTexture(texID)->getSize());
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


            if (itemType == "[Tome]")
            {
                Tome* item = new Tome(sf::Vector2f(), TextureHandler::get().getTexture(texID));
                item->setID(id);
                file >> *item;
                this->items.push_back(item);
            }


        }



        file.close();
    }

    this->currentItem = this->items.size() - 1;
}

void ItemEditor::readSpells()
{
    clearSpells();
    std::ifstream file(DATA_PATH "Spells.mop");

    if (!file.is_open())
    {
        printCon("File not found, editing unavailible");
        closeWindow();
    }

    else
    {
        while (!file.eof())
        {
            std::string spellType;
            std::string trash;

            file >> spellType;

            if (spellType == "[Fireball]")
            {
                Fireball* spell = new Fireball(sf::Vector2f());
                file >> *spell;
                this->spells.push_back(spell);
            }
        }



        file.close();
    }

    this->currentSpell = this->spells.size() - 1;
}

void ItemEditor::readProjectiles()
{
    clearProjectiles();
    std::ifstream file(DATA_PATH "Projectiles.mop");

    if (!file.is_open())
    {
        printCon("File not found, editing unavailible");
        closeWindow();
    }

    else
    {
        while (!file.eof())
        {
            std::string projectileType;
            std::string name;
            std::string trash;

            file >> projectileType;

            if (projectileType == "[LightProjectile]")
            {
                LightProjectile projectile;

                file.ignore();
                std::getline(file, name);
                while (name.size() > 0 && name[0] == ' ')
                    name.erase(name.begin());

                file >> projectile;
                this->projectileNames.push_back(name);
                this->projectiles.push_back(projectile);
            }
        }



        file.close();
    }

    this->currentProjectile = this->projectiles.size() - 1;
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
        Throwable* thrw = dynamic_cast<Throwable*>(item);
        Tome* tome = dynamic_cast<Tome*>(item);
        if (thrw)
        {
            file << "[Throwable]\n";
            file << "ID: " << thrw->getID() << "\n";
            file << "TexID: " << TextureHandler::get().getIDForTexture(thrw->getTexture()) << "\n";
            file << *thrw << "\n";
        }

        else if (tome)
        {
            file << "[Tome]\n";
            file << "ID: " << tome->getID() << "\n";
            file << "TexID: " << TextureHandler::get().getIDForTexture(tome->getTexture()) << "\n";
            file << *tome << "\n";
        }

        else
        {
            file << "[Item]\n";
            file << "ID: " << item->getID() << "\n";
            file << "TexID: " << TextureHandler::get().getIDForTexture(item->getTexture()) << "\n";
            file << *item << "\n";
        }
    }

    file.close();
}

void ItemEditor::writeSpells()
{
    std::ofstream file(DATA_PATH "Spells.mop");

    if (!file.is_open())
    {
        printCon("File not found, editing unavailible");
        closeWindow();
    }

    for (Spell* spell : spells)
    {
        Fireball* fireball = dynamic_cast<Fireball*>(spell);
        if (fireball)
        {
            file << "[Fireball]\n";
            file << *fireball << "\n";
        }
    }

    file.close();
}

void ItemEditor::writeProjectiles()
{
    std::ofstream file(DATA_PATH "Projectiles.mop");

    if (!file.is_open())
    {
        printCon("File not found, editing unavailible");
        closeWindow();
    }

    auto it = projectileNames.begin();
    for (const LightProjectile& projectile : projectiles)
    {
            file << "[LightProjectile]\n";
            file << *it++ << "\n";
            file << projectile << "\n";
    }

    file.close();
}

void ItemEditor::clearItems()
{
    for (Item* item : items)
        delete item;

    items.clear();
}

void ItemEditor::clearSpells()
{
    for (Spell* spell : spells)
        delete spell;

    spells.clear();
}

void ItemEditor::clearProjectiles()
{
    projectiles.clear();
    projectileNames.clear();
}

void ItemEditor::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    switch (this->currentTab)
    {
    case Tab::items:
        if (!items.empty())
            target.draw(*items[currentItem], states);
        break;
    default:
        break;
    }
}
