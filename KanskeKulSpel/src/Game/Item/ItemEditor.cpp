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
    m_open = false;
    m_currentTab = Tab::items;
    m_currentItem = -1;
    m_currentSpell = -1;
    m_currentProjectile = -1;
}

ItemEditor::~ItemEditor()
{
    clearItems();
    clearSpells();
}

void ItemEditor::openWindow()
{
    m_open = true;

    readItems();
    readSpells();
    readProjectiles();
}

void ItemEditor::update(float dt, sf::Vector2f mouseWorldPos)
{
    ImGui::Begin("Item Editor", &m_open);

    if (ImGui::BeginTabBar("tabbaritems"))
    {
        if (ImGui::BeginTabItem("Items"))
        {
            m_currentTab = Tab::items;
            updateItems(dt, mouseWorldPos);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Spells"))
        {
            m_currentTab = Tab::spells;
            updateSpells(dt, mouseWorldPos);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Projectiles"))
        {
            m_currentTab = Tab::projectiles;
            updateProjectiles(dt, mouseWorldPos);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
    ImGui::End();
}

void ItemEditor::updateItems(float dt, sf::Vector2f mouseWorldPos)
{
    if (!m_items.empty())
    {
        if (ImGui::BeginCombo("Select item", m_items[m_currentItem]->getComponent<LogisticsComp>()->name.c_str()))
        {
            for (int i = 0; i < m_items.size(); i++)
            {
                if (ImGui::Selectable(m_items[i]->getComponent<LogisticsComp>()->name.c_str()))
                    m_currentItem = i;
            }


            ImGui::EndCombo();
        }

        Throwable* throwable = dynamic_cast<Throwable*>(m_items[m_currentItem]);
        Tome* tome = dynamic_cast<Tome*>(m_items[m_currentItem]);
        Item* item = dynamic_cast<Item*>(m_items[m_currentItem]);
        Consumable* consumable = dynamic_cast<Consumable*>(m_items[m_currentItem]);

        if (throwable)
            showThrowableData(throwable);

        else if (tome)
            showTomeData(tome);

        else if (consumable)
            showConsumableData(consumable);

        else if (item)
            showItemData(item);
    }

    if (ImGui::Button("save"))
        writeItems();

    if (ImGui::Button("Create Item"))
    {
        Item* item = new Item(sf::Vector2f(), m_items[m_currentItem]->getComponent<SpriteComp>()->getTexture());
        m_items.push_back(item);
        item->getComponent<LogisticsComp>()->id = m_items.size() - 1;
        m_currentItem = m_items.size() - 1;
    }

    ImGui::SameLine();

    if (ImGui::Button("Create Tome"))
    {
        Tome* item = new Tome(sf::Vector2f(), m_items[m_currentItem]->getComponent<SpriteComp>()->getTexture());
        m_items.push_back(item);
        item->getComponent<LogisticsComp>()->id = m_items.size() - 1;
        item->getComponent<LogisticsComp>()->useable = true;
        m_currentItem = m_items.size() - 1;
    }

    ImGui::SameLine();

    if (ImGui::Button("Create Throwable"))
    {
        Throwable* item = new Throwable(sf::Vector2f(), m_items[m_currentItem]->getComponent<SpriteComp>()->getTexture());
        m_items.push_back(item);
        item->getComponent<LogisticsComp>()->id = m_items.size() - 1;
        item->getComponent<LogisticsComp>()->useable = true;
        m_currentItem = m_items.size() - 1;
    }

    if (ImGui::Button("Create Consumable"))
    {
        Consumable* item = new Consumable(sf::Vector2f(), m_items[m_currentItem]->getComponent<SpriteComp>()->getTexture());
        m_items.push_back(item);
        item->getComponent<LogisticsComp>()->id = m_items.size() - 1;
        item->getComponent<LogisticsComp>()->useable = true;
        m_currentItem = m_items.size() - 1;
    }

    if (ImGui::Button("Delete"))
    {
        m_items.erase(m_items.begin() + m_currentItem);
        m_currentItem = m_items.size() - 1;
    }

}

void ItemEditor::updateSpells(float dt, sf::Vector2f mouseWorldPos)
{
    if (!m_spells.empty())
    {
        if (ImGui::BeginCombo("Select spell", m_spells[m_currentSpell]->getName().c_str()))
        {
            for (int i = 0; i < m_spells.size(); i++)
            {
                if (ImGui::Selectable(m_spells[i]->getName().c_str()))
                    m_currentSpell = i;
            }


            ImGui::EndCombo();
        }

        Fireball* fireball = dynamic_cast<Fireball*>(m_spells[m_currentSpell]);
        if (fireball)
            showFireballData(fireball);
       

    }

    if (ImGui::Button("save"))
        writeSpells();
}

void ItemEditor::updateProjectiles(float dt, sf::Vector2f mouseWorldPos)
{
    if (!m_projectiles.empty())
    {
        if (ImGui::BeginCombo("Select projectile", m_projectileNames[m_currentProjectile].c_str()))
        {
            for (int i = 0; i < m_projectiles.size(); i++)
            {
                if (ImGui::Selectable(m_projectileNames[i].c_str()))
                {
                    m_currentProjectile = i;
                    ProjectileHandler::addProjectile(m_currentProjectile, mouseWorldPos, sf::Vector2f(0, -1), DamageComp::DamageOrigin::neutral);
                }
            }


            ImGui::EndCombo();
        }

        if (MOUSE::MouseState::isButtonClicked(sf::Mouse::Right))
        {
            ProjectileHandler::addProjectile(m_currentProjectile, mouseWorldPos, sf::Vector2f(0, -1), DamageComp::DamageOrigin::neutral);
        }

        ImGui::Text("Index: %d", m_currentProjectile);
        showProjectileData(&m_projectiles[m_currentProjectile], &m_projectileNames[m_currentProjectile]);
    }

    if (ImGui::Button("Create Light Projectile"))
    {
        LightProjectile projectile;
        m_projectiles.push_back(projectile);
        m_projectileNames.push_back("Temp name. Please change me!");
        m_currentProjectile = m_projectiles.size() - 1;
    }

    if (ImGui::Button("save"))
        writeProjectiles();
}

void ItemEditor::showItemData(Entity* item)
{
    LogisticsComp* logistics = item->getComponent<LogisticsComp>();
    SpriteComp* sprite = item->getComponent<SpriteComp>();
    ImGui::Text("%s", ("ItemID: " + std::to_string(logistics->id)).c_str());

    const std::vector<sf::Texture>* textures = TextureHandler::get().getTextureVec();
    int currentTexture = TextureHandler::get().getIDForTexture(sprite->getTexture());
    if (ImGui::BeginCombo("Select texture", TextureHandler::get().getTextureName(currentTexture).c_str()))
    {
        
        for (int i = 0; i < textures->size(); i++)
        {
            if (ImGui::Selectable(TextureHandler::get().getTextureName(i).c_str()))
                sprite->setTexture(&textures->at(i));
        }


        ImGui::EndCombo();
    }

    const std::vector<Emitter>* emitters = ParticleHandler::getEmitterTemplates();
    int currentEmitter = logistics->emitterID;
    if (ImGui::BeginCombo("Select emitter", ParticleHandler::getEmitterName(currentEmitter).c_str()))
    {

        for (int i = 0; i < emitters->size(); i++)
        {
            if (ImGui::Selectable(ParticleHandler::getEmitterName(i).c_str()))
                logistics->emitterID = i;
        }

        if (ImGui::Selectable("None"))
            logistics->emitterID = -1;

        ImGui::EndCombo();
    }

    ImGui::InputText("Name", &logistics->name);

    ImGui::InputInt("Stack limit", &logistics->stackLimit);
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
    ImGui::DragFloat("Mass", &item->getComponent<MovementComp>()->mass, 0.01, 0, 1000);

    int armingTime = item->getArmingTime();
    ImGui::DragInt("Arming time, milliseconds", &armingTime, 0.01, 0, 100000);
    item->setArmingTime(armingTime);

    float bounce = item->getBounce();
    ImGui::DragFloat("Bounce", &bounce, 0.01, 0, 1);
    item->setBounce(bounce);

    bool detonateOnImpact = item->isDetonatingOnImpact();
    ImGui::Checkbox("DetonateOnImpact", &detonateOnImpact);
    item->setDetonatingOnImpact(detonateOnImpact);

    ImGui::DragInt("Collision damage", &item->getComponent<DamageComp>()->damage, 1, 0, 100000);

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

void ItemEditor::showConsumableData(Consumable* consumable)
{
    static int currentActive = 0;
    static int currentNonActive = 0;
    showItemData(consumable);
    ImGui::Separator();

    StatusComp::StatusList* activeStatusList = consumable->getStatusComp()->getStatusPtr();
    if (!activeStatusList->empty())
    {
        if (ImGui::BeginCombo("Active statuses", StatusComp::STATUS_NAME[currentActive].c_str()))
        {
            int i = 0;
            for (auto& pair : *activeStatusList)
            {
                if (ImGui::Selectable(StatusComp::STATUS_NAME[(int)pair.first].c_str()))
                    currentActive = i;

                i++;
            }

            ImGui::EndCombo();
        }

        ImGui::DragInt("Duration", &activeStatusList->at((StatusComp::Status)currentActive), 10, 0, 1000000, "%d ms");

        if (ImGui::Button("Remove"))
        {
            activeStatusList->erase((StatusComp::Status)currentActive--);
        }
    }

    StatusComp::StatusList templateList = StatusComp::getTemplateList();
    StatusComp::StatusList nonActiveList;
    for (auto& pair : templateList)
    {
        if (!activeStatusList->count(pair.first))
            nonActiveList.emplace(pair.first, pair.second);
    }

    if (!nonActiveList.empty())
    {
        if (ImGui::BeginCombo("Inactive statuses", StatusComp::STATUS_NAME[currentNonActive].c_str()))
        {
            int i = 0;
            for (auto& pair : nonActiveList)
            {
                if (ImGui::Selectable(StatusComp::STATUS_NAME[(int)pair.first].c_str()))
                    currentNonActive = i;

                i++;
            }

            ImGui::EndCombo();
        }
    
        if (ImGui::Button("Add Status"))
        {
            activeStatusList->emplace((StatusComp::Status)currentNonActive, 0);
            currentActive = currentNonActive;
        }
    }

    


    ImGui::Separator();
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

    ImGui::DragInt("Damage", &fireball->getDamageComp()->damage, 1, 0, 10000);

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

    ImGui::DragInt("Damage", &projectile->getDamageComp()->damage, 1, 0, 10000);

    float velocity = projectile->getVelocity();
    ImGui::DragFloat("Velocity", &velocity, 1, 0, 100000);
    projectile->setVelocity(velocity);

    float size = projectile->getComponent<ColliderComp>()->getSize().x;
    ImGui::DragFloat("size", &size, 1, 0, 100000);
    projectile->setSize(sf::Vector2f(size, size));
}

void ItemEditor::readItems()
{
    clearItems();
    std::ifstream file(DATA_PATH "Items.mop");

    if (!file.is_open())
    {
        printfCon("File not found, editing unavailible");
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
                Throwable* item = new Throwable(sf::Vector2f(), TextureHandler::get().getTexture(texID));
                item->getComponent<LogisticsComp>()->id = id;
                file >> *item;
                m_items.push_back(item);
            }

            if (itemType == "[Item]")
            {
                Item* item = new Item(sf::Vector2f(), TextureHandler::get().getTexture(texID));
                item->getComponent<LogisticsComp>()->id = id;
                file >> *item->getComponent<LogisticsComp>();
                m_items.push_back(item);
            }


            if (itemType == "[Tome]")
            {
                Tome* item = new Tome(sf::Vector2f(), TextureHandler::get().getTexture(texID));
                item->getComponent<LogisticsComp>()->id = id;
                file >> *item;
                m_items.push_back(item);
            }

            if (itemType == "[Consumable]")
            {
                Consumable* item = new Consumable(sf::Vector2f(), TextureHandler::get().getTexture(texID));
                item->getComponent<LogisticsComp>()->id = id;
                file >> *item;
                m_items.push_back(item);
            }


        }



        file.close();
    }

    m_currentItem = m_items.size() - 1;
}

void ItemEditor::readSpells()
{
    clearSpells();
    std::ifstream file(DATA_PATH "Spells.mop");

    if (!file.is_open())
    {
        printfCon("File not found, editing unavailible");
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
                m_spells.push_back(spell);
            }
        }



        file.close();
    }

    m_currentSpell = m_spells.size() - 1;
}

void ItemEditor::readProjectiles()
{
    clearProjectiles();
    std::ifstream file(DATA_PATH "Projectiles.mop");

    if (!file.is_open())
    {
        printfCon("File not found, editing unavailible");
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
                m_projectileNames.push_back(name);
                m_projectiles.push_back(projectile);
            }
        }



        file.close();
    }

    m_currentProjectile = m_projectiles.size() - 1;
}

void ItemEditor::writeItems()
{
    std::ofstream file(DATA_PATH "Items.mop");

    if (!file.is_open())
    {
        printfCon("File not found, editing unavailible");
        closeWindow();
    }

    for (Entity* item : m_items)
    {
        LogisticsComp* logistics = item->getComponent<LogisticsComp>();
        SpriteComp* sprite = item->getComponent<SpriteComp>();
        Throwable* thrw = dynamic_cast<Throwable*>(item);
        Tome* tome = dynamic_cast<Tome*>(item);
        Consumable* consumable = dynamic_cast<Consumable*>(item);
        if (thrw)
        {
            file << "[Throwable]\n";
            file << "ID: " << logistics->id << "\n";
            file << "TexID: " << TextureHandler::get().getIDForTexture(sprite->getTexture()) << "\n";
            file << *thrw << "\n";
        }

        else if (tome)
        {
            file << "[Tome]\n";
            file << "ID: " << logistics->id << "\n";
            file << "TexID: " << TextureHandler::get().getIDForTexture(sprite->getTexture()) << "\n";
            file << *tome << "\n";
        }

        else if (consumable)
        {
            file << "[Consumable]\n";
            file << "ID: " << logistics->id << "\n";
            file << "TexID: " << TextureHandler::get().getIDForTexture(sprite->getTexture()) << "\n";
            file << *consumable << "\n";
        }

        else
        {
            file << "[Item]\n";
            file << "ID: " << logistics->id << "\n";
            file << "TexID: " << TextureHandler::get().getIDForTexture(sprite->getTexture()) << "\n";
            file << *logistics << "\n";
        }
    }

    file.close();
}

void ItemEditor::writeSpells()
{
    std::ofstream file(DATA_PATH "Spells.mop");

    if (!file.is_open())
    {
        printfCon("File not found, editing unavailible");
        closeWindow();
    }

    for (Spell* spell : m_spells)
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
        printfCon("File not found, editing unavailible");
        closeWindow();
    }

    auto it = m_projectileNames.begin();
    for (const LightProjectile& projectile : m_projectiles)
    {
            file << "[LightProjectile]\n";
            file << *it++ << "\n";
            file << projectile << "\n";
    }

    file.close();
}

void ItemEditor::clearItems()
{
    for (Entity* item : m_items)
        delete item;

    m_items.clear();
}

void ItemEditor::clearSpells()
{
    for (Spell* spell : m_spells)
        delete spell;

    m_spells.clear();
}

void ItemEditor::clearProjectiles()
{
    m_projectiles.clear();
    m_projectileNames.clear();
}

void ItemEditor::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    switch (m_currentTab)
    {
    case Tab::items:
        if (!m_items.empty())
            target.draw(*m_items[m_currentItem]->getComponent<SpriteComp>(), states);
        break;
    default:
        break;
    }
}
