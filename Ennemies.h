#pragma once
#include "Character.h"
#include "Player.h"
#include "Collider.h"
#include "CollisionManager.h"
#include "src/Game/GameManager.h"
#include "PowerUp.h"

// Forward declare CollisionManager to avoid including its header here
class CollisionManager;
class GameManager;

enum class BossState
{
    Alive,
    Dying,
    Dead
};

/*----------------------------------*/
//                                  //
// MADE BY HENRICK BARIL 2026-04-09 //
//                                  //
/*----------------------------------*/

// Plural feel odd here. Singular would be better :) 
class Ennemies : public Character
{
public:
    Ennemies();
    Ennemies(olc::vf2d pos, float radius, float maxH);

    virtual ~Ennemies();

    // --- Fonctions virtuelles ---
    virtual void Update(AcademiaEngine& engine, float elapsedTime);
    virtual void Draw(AcademiaEngine& engine) override;
    virtual void TakeDamage(float damage, float elapsedTime);
    virtual int GetScale() { return Scale; }
    virtual olc::Sprite* GetSprite(AcademiaEngine& engine)
    {
        return &engine.RedSlimeSheet;
    }
    virtual bool IsDialogueActive() const { return false; }
    virtual void DrawDialogue(AcademiaEngine& engine, std::vector<std::string> dialogue){}
    virtual void InitializeCollision(class CollisionManager* collisionManager);
    virtual void ShutdownCollision(class CollisionManager* collisionManager);

    // --- Stats virtuelles ---
    virtual float GetSpeed() const { return 60.0f; }
    virtual float GetDamage() const { return 10.0f; }
    virtual float GetMass()   const { return 1.0f; }

    void SetGameManager(GameManager* gm) { gameManager = gm; }


    BossState GetState() const { return State; }
    BossState SetState(BossState state) { State = state; }

    std::vector<std::string> GetIntroDialogue() {
        return _Dialogue;
    }
    std::vector<std::string> GetDeathDialogue() {
        return _DeathDialogue;
    }

    void AddForce(AcademiaEngine& engine, float force, olc::vf2d direction, float elapsedTime);

    static void RemoveEnnemie(std::deque<std::unique_ptr<Ennemies>>& ennemies, AcademiaEngine& engine);

    const olc::vf2d& GetPlayerPosition(AcademiaEngine& engine) const;
    void GetDirection(AcademiaEngine& engine, olc::vf2d playerPosition);
    Player* GetPlayer() const { return player; }
    void SetPlayer(Player* p);
    void SetMaxHealth(float h);
    void SpawnPowerUp(AcademiaEngine& engine);

    float GetRadius() const { return Radius; }
    float GetHealth() const { return Health; }

    bool hasExploded = false;
    GameManager* gameManager = nullptr;
    CollisionManager* collisionManager = nullptr;
    std::unique_ptr<Collider> collider = nullptr;
    olc::vf2d direction;
    olc::vf2d recoilVelocity;

protected:
    float Radius = 15.0f;
    olc::Pixel Color = olc::Pixel(255, 59, 59);
    float Health;
    float MaxHealth = 50.0f;
    int Frame = 0;
    float Timer = 0.0f;
    int Scale = 2;
    BossState State = BossState::Alive;

private:
    Player* player = nullptr;

    //FOR BOSS USAGE
    std::vector<std::string> _Dialogue = {
    "...",
    "I HAVE WATCHED YOU.",
    "THOUSANDS HAVE FALLEN.",
    "YOU ARE NO DIFFERENT.",
    "COME MORTAL."
    };
    std::vector<std::string> _DeathDialogue = {
        "AURGHHHHHHHHHHHHHHHHHHH",
        "HOW COME WOU'VE DEFEAT ME ?",
        "YOU MUST BE A GOOD PILOT THEN",
        "...",
        "YOUR NOT READY FOR THE REVENGER",
        "MOUAHAHAHAHAHahahaurgh..."
    };
};