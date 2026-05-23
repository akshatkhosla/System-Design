#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>

// ─── Prototype Interface ───────────────────────────────────────────────────────
// Every character must be able to clone itself.
// Callers depend only on this interface — they never need to know the concrete type.
class ICharacter {
public:
    virtual std::unique_ptr<ICharacter> clone() const = 0;
    virtual void display() const = 0;
    virtual ~ICharacter() = default;
};


// ─── Concrete Prototypes ───────────────────────────────────────────────────────
// Each class uses the compiler-generated copy constructor for cloning.
// This is safe here because all fields are value types (int, string) — no raw pointers —
// so the default copy gives us a fully independent deep copy automatically.

// std:move is used in constructors to efficiently initialize string fields, 
// but the copy constructor will still perform a deep copy of the strings when cloning. 
// If we had raw pointers, we would need to implement a custom copy constructor to ensure a deep copy, but with value types, the default behavior is sufficient.
class Warrior : public ICharacter {
public:
    Warrior(int health, std::string weapon, std::string armor)
        : health_(health), weapon_(std::move(weapon)), armor_(std::move(armor)) {}

    // Clone: wraps the copy constructor — Warrior knows best how to copy itself
    std::unique_ptr<ICharacter> clone() const override {
        // std::make_unique calls the copy constructor of Warrior, which performs a deep copy of all fields
        return std::make_unique<Warrior>(*this);
    }

    void display() const override {
        std::cout << "[Warrior] HP=" << health_
                  << " | Weapon=" << weapon_
                  << " | Armor=" << armor_ << "\n";
    }

private:
    int health_;
    std::string weapon_;
    std::string armor_;
};

class Mage : public ICharacter {
public:
    Mage(int health, int spellPower, int mana)
        : health_(health), spellPower_(spellPower), mana_(mana) {}

    std::unique_ptr<ICharacter> clone() const override {
        // std::make_unique calls the copy constructor of Mage, which performs a deep copy of all fields
        return std::make_unique<Mage>(*this);
    }

    void display() const override {
        std::cout << "[Mage] HP=" << health_
                  << " | SpellPower=" << spellPower_
                  << " | Mana=" << mana_ << "\n";
    }

private:
    int health_;
    int spellPower_;
    int mana_;        // int, not string — mana is a numeric resource
};

class Archer : public ICharacter {
public:
    Archer(int health, int range, int arrowCount)
        : health_(health), range_(range), arrowCount_(arrowCount) {}

    std::unique_ptr<ICharacter> clone() const override {
        return std::make_unique<Archer>(*this);
    }

    void display() const override {
        std::cout << "[Archer] HP=" << health_
                  << " | Range=" << range_
                  << " | Arrows=" << arrowCount_ << "\n";
    }

private:
    int health_;
    int range_;
    int arrowCount_;
};


// ─── Character Spawner (Prototype Registry) ────────────────────────────────────
// Holds one pre-configured prototype per character type.
// Spawning any character is just: find its prototype → call clone().
// The spawner never needs to know what fields a Warrior or Mage has.
class CharacterSpawner {
public:
    // Register a prototype under a name — takes ownership of the prototype
    void registerPrototype(const std::string& type, std::unique_ptr<ICharacter> prototype) {
        registry_[type] = std::move(prototype);
    }

    // Spawn returns a fresh independent clone of the named prototype
    std::unique_ptr<ICharacter> spawn(const std::string& type) const {
        auto it = registry_.find(type);
        if (it == registry_.end())
            throw std::runtime_error("Unknown character type: " + type);
        return it->second->clone();
    }

private:
    // Prototypes live here — map owns them via unique_ptr
    std::unordered_map<std::string, std::unique_ptr<ICharacter>> registry_;
};


int main() {
    CharacterSpawner spawner;

    // Register one pre-configured prototype per type.
    // These are the "master copies" — expensive to build, cheap to clone.
    spawner.registerPrototype("warrior", std::make_unique<Warrior>(100, "Sword", "Plate"));
    spawner.registerPrototype("mage",    std::make_unique<Mage>(60, 200, 150));
    spawner.registerPrototype("archer",  std::make_unique<Archer>(80, 50, 30));

    std::cout << "=== Spawning wave 1 ===\n";
    auto w1 = spawner.spawn("warrior");
    auto m1 = spawner.spawn("mage");
    auto a1 = spawner.spawn("archer");
    w1->display();
    m1->display();
    a1->display();

    // Each clone is fully independent — modifying one doesn't affect the prototype
    // or any other clone. This would fail silently with a shallow copy of pointers.
    std::cout << "\n=== Spawning wave 2 (same prototypes, new clones) ===\n";
    auto w2 = spawner.spawn("warrior");
    auto w3 = spawner.spawn("warrior");
    w2->display();
    w3->display();

    std::cout << "\nAre w2 and w3 different objects? "
              << (w2.get() != w3.get() ? "YES — independent clones" : "NO") << "\n";

    // No manual delete needed — unique_ptr handles cleanup automatically
    return 0;
}
