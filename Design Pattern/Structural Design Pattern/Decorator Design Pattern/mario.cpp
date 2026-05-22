#include <iostream>
#include <string>
#include <memory>

// ─── Decorator Design Pattern ─────────────────────────────────────────────────
// Problem: Mario gains and loses power-ups at runtime — we can't model every
//          combination (plain, mushroom, flower, star...) as separate subclasses.
// Solution: Each power-up is a decorator that wraps whatever Mario currently is,
//           adds its own ability on top, and delegates the rest down the chain.
//
// Wrapping chain built in main:
//   FireFlower → SuperMushroom → Mario (base)
//
// getAbilities() call unwinds the chain from outermost to innermost:
//   FireFlower::getAbilities()
//       → SuperMushroom::getAbilities()
//           → Mario::getAbilities()   ← base abilities printed first


// ─── Component Interface ──────────────────────────────────────────────────────
// Both the base character and every decorator implement this.
// The virtual destructor is required — we delete via ICharacter*, and without it
// only ~ICharacter() runs, leaking the derived object's resources.
class ICharacter {
public:
    virtual void getAbilities() const = 0;
    virtual ~ICharacter() = default;
};


// ─── Concrete Component ───────────────────────────────────────────────────────
// Base Mario — no power-ups. Fireball is a power-up, not a base skill.
class Mario : public ICharacter {
public:
    void getAbilities() const override {
        std::cout << "[Mario] Can jump and run.\n";
    }
};


// ─── Base Decorator ───────────────────────────────────────────────────────────
// Holds a reference to the wrapped character and forwards calls to it.
// Concrete decorators extend this — they never touch character_ directly
// because it's protected, so only the class hierarchy can access it.
class CharacterDecorator : public ICharacter {
public:
    explicit CharacterDecorator(std::unique_ptr<ICharacter> character)
        : character_(std::move(character)) {}

protected:
    std::unique_ptr<ICharacter> character_; // owns the wrapped layer
};


// ─── Concrete Decorators (Power-ups) ──────────────────────────────────────────
// Each decorator calls the wrapped layer first, then adds its own ability.
// This means abilities are printed bottom-up: base → mushroom → flower.

class SuperMushroom : public CharacterDecorator {
public:
    // explicit prevents accidental implicit conversions from raw pointers, which would cause double deletes
    explicit SuperMushroom(std::unique_ptr<ICharacter> character)
        : CharacterDecorator(std::move(character)) {}

    void getAbilities() const override {
        character_->getAbilities();              // delegate down the chain first
        std::cout << "[SuperMushroom] Can grow bigger and break blocks.\n";
    }
};

class FireFlower : public CharacterDecorator {
public:
    explicit FireFlower(std::unique_ptr<ICharacter> character)
        : CharacterDecorator(std::move(character)) {}

    void getAbilities() const override {
        character_->getAbilities();              // delegate down the chain first
        std::cout << "[FireFlower] Can throw fireballs.\n";
    }
};

class StarPower : public CharacterDecorator {
public:
    explicit StarPower(std::unique_ptr<ICharacter> character)
        : CharacterDecorator(std::move(character)) {}

    void getAbilities() const override {
        character_->getAbilities();              // delegate down the chain first
        std::cout << "[StarPower] Is invincible and defeats enemies on contact.\n";
    }
};


int main() {
    // Step 1 — plain Mario
    std::cout << "=== Plain Mario ===\n";
    std::unique_ptr<ICharacter> mario = std::make_unique<Mario>();
    mario->getAbilities();

    // Step 2 — wrap with SuperMushroom
    // Old way: mario = new SuperMushroom(mario)  — overwrites pointer, previous allocation is leaked
    // New way: move ownership into the decorator — no leak, no manual delete needed
    std::cout << "\n=== Mario gets Super Mushroom ===\n";
    mario = std::make_unique<SuperMushroom>(std::move(mario));
    mario->getAbilities();

    // Step 3 — wrap with FireFlower on top of SuperMushroom
    std::cout << "\n=== Mario gets Fire Flower ===\n";
    mario = std::make_unique<FireFlower>(std::move(mario));
    mario->getAbilities();

    // Step 4 — wrap with StarPower on top of everything
    std::cout << "\n=== Mario gets Star Power ===\n";
    mario = std::make_unique<StarPower>(std::move(mario));
    mario->getAbilities();

    // unique_ptr destructor unwinds the chain automatically:
    //   ~StarPower → ~FireFlower → ~SuperMushroom → ~Mario
    return 0;
}
