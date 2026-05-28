#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>
#include <algorithm>   // for remove(), sort()
#include <tuple>       // for tuple in simplifyDebts()

using namespace std;

// ─────────────────────────────────────────────
//  USER
// ─────────────────────────────────────────────

class User {
private:
    string id;
    string name;
    string email;

public:
    User(const string& id, const string& name, const string& email)
        : id(id), name(name), email(email) {}

    // Getters
    string getId()    const { return id; }
    string getName()  const { return name; }
    string getEmail() const { return email; }

    // For easy printing/debugging
    string toString() const {
        return "User(" + name + ", " + email + ")";
    }

    // Needed to use User as a Map key
    //
    // Syntax breakdown:
    //   bool          → return type (comparisons always return true/false)
    //   operator==    → overloads the == symbol; defines what == means for two User objects
    //   (const User& other) → right-hand side of ==; passed by const-ref (no copy, no mutation)
    //   const         → (at the end) this method won't modify *this (the left-hand side)
    //   return id == other.id → two Users are equal if their ids match
    //
    // When you write:  userA == userB
    // C++ calls:       userA.operator==(userB)   →  userA is 'this', userB is 'other'
    bool operator==(const User& other) const { return id == other.id; }

    // operator< is required by std::map to maintain sorted order of keys
    // When you write:  userA < userB
    // C++ calls:       userA.operator<(userB)    →  true if userA.id comes before userB.id
    bool operator<(const User& other)  const { return id < other.id; }
};


// ─────────────────────────────────────────────
//  SPLIT
// ─────────────────────────────────────────────

class Split {
private:
    User user;       // who owes
    double amount;   // how much they owe

public:
    Split(const User& user, double amount)
        : user(user), amount(amount) {}

    // Getters
    User   getUser()   const { return user; }
    double getAmount() const { return amount; }

    string toString() const {
        return user.getName() + " owes ₹" + to_string(amount);
    }
};

// ─────────────────────────────────────────────
//  SPLIT STRATEGY  (Strategy Pattern)
// ─────────────────────────────────────────────
//
//  Why return vector<Split> instead of void?
//  The caller (SplitwiseService) needs the computed splits to update
//  the balance map. A void return with an inputs[] mutation won't work
//  because inputs is passed by value — changes die inside the function.
//
//  inputs meaning per strategy:
//    EqualSplit   → ignored (just pass {})
//    PercentSplit → percentage share per user  e.g. {50.0, 30.0, 20.0}
//    ExactSplit   → exact rupee amount per user e.g. {150.0, 90.0, 60.0}

class SplitStrategy {
public:
    // Pure virtual — every strategy MUST implement this
    virtual vector<Split> calculateSplits(double totalAmount,
                                          const vector<User>& users,
                                          const vector<double>& inputs) = 0;

    virtual ~SplitStrategy() = default; // virtual destructor for proper cleanup
};


// ── Strategy 1: Equal Split ───────────────────
//  totalAmount / N  for every participant
//  inputs: ignored

class EqualSplitStrategy : public SplitStrategy {
public:
    vector<Split> calculateSplits(double totalAmount,
                                  const vector<User>& users,
                                  const vector<double>& inputs) override {
        if (users.empty())
            throw invalid_argument("No participants provided");

        double share = totalAmount / users.size();  // equal share per person

        vector<Split> splits;
        for (const User& user : users) {
            splits.push_back(Split(user, share));
        }
        return splits;
    }
};


// ── Strategy 2: Percent Split ─────────────────
//  each user owes their % of totalAmount
//  inputs: percentage per user  e.g. {50.0, 30.0, 20.0}  — must sum to 100

class PercentSplitStrategy : public SplitStrategy {
public:
    vector<Split> calculateSplits(double totalAmount,
                                  const vector<User>& users,
                                  const vector<double>& inputs) override {
        if (users.size() != inputs.size())
            throw invalid_argument("Each user must have a percentage");

        // Validate percentages sum to 100
        double total = 0;
        for (double p : inputs) total += p;
        if (abs(total - 100.0) > 0.01)   // 0.01 tolerance for floating-point errors
            throw invalid_argument("Percentages must sum to 100, got " + to_string(total));

        vector<Split> splits;
        for (size_t i = 0; i < users.size(); ++i) {
            double amount = (inputs[i] / 100.0) * totalAmount;
            splits.push_back(Split(users[i], amount));
        }
        return splits;
    }
};


// ── Strategy 3: Exact Split ───────────────────
//  each user owes the exact amount specified
//  inputs: exact rupee amount per user  e.g. {200.0, 70.0, 30.0}  — must sum to totalAmount

class ExactSplitStrategy : public SplitStrategy {
public:
    vector<Split> calculateSplits(double totalAmount,
                                  const vector<User>& users,
                                  const vector<double>& inputs) override {
        if (users.size() != inputs.size())
            throw invalid_argument("Each user must have an exact amount");

        // Validate amounts sum to totalAmount
        double total = 0;
        for (double a : inputs) total += a;
        if (abs(total - totalAmount) > 0.01)   // 0.01 tolerance for floating-point errors
            throw invalid_argument("Exact amounts must sum to " + to_string(totalAmount)
                                   + ", got " + to_string(total));

        vector<Split> splits;
        for (size_t i = 0; i < users.size(); ++i) {
            splits.push_back(Split(users[i], inputs[i]));
        }
        return splits;
    }
};


// ─────────────────────────────────────────────
//  EXPENSE
// ─────────────────────────────────────────────
//
//  Key design decision: splits are computed INSIDE the constructor
//  by calling strategy->calculateSplits(). Once an Expense is created,
//  its splits are fixed and immutable — no one can change them later.
//
//  Why shared_ptr<SplitStrategy> instead of raw pointer?
//  Multiple expenses could share the same strategy object (e.g. two
//  expenses both using EqualSplitStrategy). shared_ptr handles lifetime
//  automatically — no manual delete, no dangling pointer risk.

class Expense {
private:
    string id;
    string description;
    double totalAmount;
    User   paidBy;
    vector<Split> splits;     // computed once at construction via strategy
    shared_ptr<SplitStrategy> strategy;
    string createdAt;         // kept as string for interview simplicity

public:
    // Constructor computes splits immediately using the provided strategy
    Expense(const string& id,
            const string& description,
            double totalAmount,
            const User& paidBy,
            const vector<User>& participants,
            shared_ptr<SplitStrategy> strategy,
            const vector<double>& inputs)
        : id(id),
          description(description),
          totalAmount(totalAmount),
          paidBy(paidBy),
          strategy(strategy),
          createdAt("2026-05-27")   // simplified; use chrono in production
    {
        // Delegate split computation entirely to the strategy
        // Expense doesn't care HOW splits are calculated — only the result
        splits = strategy->calculateSplits(totalAmount, participants, inputs);
    }

    // Getters — all const, Expense is immutable after creation
    string               getId()          const { return id; }
    string               getDescription() const { return description; }
    double               getTotalAmount() const { return totalAmount; }
    User                 getPaidBy()      const { return paidBy; }
    const vector<Split>& getSplits()      const { return splits; }   // ref — no copy
    string               getCreatedAt()   const { return createdAt; }

    void print() const {
        cout << "\n[Expense] " << description << " | ₹" << totalAmount
             << " | Paid by: " << paidBy.getName() << "\n";
        for (const Split& s : splits) {
            cout << "  " << s.toString() << "\n";
        }
    }
};


// ─────────────────────────────────────────────
//  SETTLEMENT
// ─────────────────────────────────────────────
//
//  A Settlement is a payment log entry — created when a user
//  clears their debt with another user.
//
//  It is IMMUTABLE after creation (no setters).
//  It answers: who paid, who received, how much, in which group, when.
//
//  groupId is optional:
//    - non-empty string → settlement inside a group
//    - empty string ""  → one-on-one settlement (no group)

class Settlement {
private:
    string id;
    User   paidBy;    // the user who is clearing their debt
    User   paidTo;    // the user who was owed money
    double amount;
    string groupId;   // "" if one-on-one
    string timestamp; // simplified as string; use chrono in production

public:
    Settlement(const string& id,
               const User& paidBy,
               const User& paidTo,
               double amount,
               const string& groupId = "")   // default = one-on-one
        : id(id),
          paidBy(paidBy),
          paidTo(paidTo),
          amount(amount),
          groupId(groupId),
          timestamp("2026-05-27")
    {}

    // Getters — immutable after creation, so no setters needed
    string getId()        const { return id; }
    User   getPaidBy()    const { return paidBy; }
    User   getPaidTo()    const { return paidTo; }
    double getAmount()    const { return amount; }
    string getGroupId()   const { return groupId; }
    string getTimestamp() const { return timestamp; }

    bool isGroupSettlement() const { return !groupId.empty(); }

    void print() const {
        cout << "[Settlement] " << paidBy.getName()
             << " paid ₹" << amount
             << " to " << paidTo.getName();
        if (isGroupSettlement())
            cout << " (Group: " << groupId << ")";
        cout << " at " << timestamp << "\n";
    }
};


// ─────────────────────────────────────────────
//  GROUP
// ─────────────────────────────────────────────
//
//  balances[A][B] = X  means  A owes B  ₹X
//
//  Three responsibilities:
//    1. Membership management  (addMember / removeMember with guard)
//    2. Expense tracking       (addExpense → updates balance map)
//    3. Settlement             (settle → zeros balance + logs to settlements)
//  Bonus: simplifyDebts()     (greedy algorithm to minimise transactions)

class Group {
private:
    string id;
    string name;
    vector<User>       members;
    vector<Expense>    expenses;
    vector<Settlement> settlements;

    // balance[A][B] = X  →  A owes B  ₹X
    // Only the "debtor → creditor" direction is stored; reverse is always 0
    //balances = {
    //     Bob     → { Alice → 100 }        // Bob owes Alice ₹100
    //     Charlie → { Alice → 100,         // Charlie owes Alice ₹100
    //                 Bob   → 50  }        // Charlie owes Bob ₹50
    //     Alice   → { Bob   → 50  }        // Alice owes Bob ₹50
    // }
    map<User, map<User, double>> balances;

    // ── Private helper ────────────────────────
    // Returns true if user has any non-zero balance (owes or is owed)
    bool hasUnsettledBalance(const User& user) const {
        // Does user owe anyone?
        auto it = balances.find(user);
        if (it != balances.end()) {
            for (auto& [creditor, amount] : it->second)
                if (amount > 0.01) return true;
        }
        // Does anyone owe user?
        for (auto& [debtor, creditorMap] : balances) {
            auto it2 = creditorMap.find(user);
            if (it2 != creditorMap.end() && it2->second > 0.01) return true;
        }
        return false;
    }

public:
    Group(const string& id, const string& name) : id(id), name(name) {}

    // Getters
    string                    getId()          const { return id; }
    string                    getName()        const { return name; }
    const vector<User>&       getMembers()     const { return members; }
    const vector<Expense>&    getExpenses()    const { return expenses; }
    const vector<Settlement>& getSettlements() const { return settlements; }

    // ── Membership ────────────────────────────

    void addMember(const User& user) {
        for (const User& m : members)
            if (m == user) throw invalid_argument(user.getName() + " is already in the group");
        members.push_back(user);
    }

    // Guard: cannot leave if any balance is non-zero
    void removeMember(const User& user) {
        if (hasUnsettledBalance(user))
            throw runtime_error(user.getName() + " has unsettled balances. Settle before leaving.");

        // std::remove shifts matching elements to the end, erase deletes them
        // Syntax: vec.erase(remove(begin, end, val), end)  →  "erase-remove idiom"
        members.erase(remove(members.begin(), members.end(), user), members.end());
    }

    // ── Expense ───────────────────────────────
    
    void addExpense(const Expense& expense) {
        expenses.push_back(expense);

        // Update balance map: for each split, split.user owes paidBy that amount
        // Skip the payer — they don't owe themselves
        for (const Split& split : expense.getSplits()) {
            if (split.getUser() == expense.getPaidBy()) continue;
            balances[split.getUser()][expense.getPaidBy()] += split.getAmount();
        }
    }

    // ── Settlement ────────────────────────────

    void settle(const User& from, const User& to, const string& settlementId) {
        double amount = balances[from][to];
        if (amount <= 0.01)
            throw runtime_error(from.getName() + " does not owe " + to.getName() + " anything");

        balances[from][to] = 0.0;   // clear the debt
        settlements.push_back(Settlement(settlementId, from, to, amount, id));

        cout << "[Settled] " << from.getName()
             << " paid ₹" << amount << " to " << to.getName() << "\n";
    }

    // ── Simplify Debts (Greedy) ───────────────
    //
    //  Goal: reduce N*(N-1) potential transactions to at most N-1
    //
    //  Algorithm:
    //    Step 1 — compute net balance per member
    //             net > 0 → they are owed money   (creditor)
    //             net < 0 → they owe money         (debtor)
    //    Step 2 — sort creditors desc, debtors asc
    //    Step 3 — greedily pair max-creditor with max-debtor
    //             settle min(creditAmt, debtAmt), advance whichever hits 0

    vector<tuple<User, User, double>> simplifyDebts() const {
        // Step 1: net balance per member
        map<User, double> net;
        for (const User& m : members) net[m] = 0.0;

        for (auto& [debtor, creditorMap] : balances)
            for (auto& [creditor, amount] : creditorMap)
                if (amount > 0.01) {
                    net[debtor]   -= amount;   // debtor's net goes down
                    net[creditor] += amount;   // creditor's net goes up
                }

        // Step 2: split into creditors and debtors, sort by magnitude
        vector<pair<double, User>> creditors, debtors;
        for (auto& [user, balance] : net) {
            if (balance  >  0.01) creditors.push_back({balance, user});
            if (balance  < -0.01) debtors.push_back({balance, user});
        }
        // Sort descending so index-0 is always the largest
        sort(creditors.begin(), creditors.end(), [](auto& a, auto& b){ return a.first > b.first; });
        sort(debtors.begin(),   debtors.end(),   [](auto& a, auto& b){ return a.first < b.first; });

        // Step 3: greedy matching
        vector<tuple<User, User, double>> result;
        int i = 0, j = 0;
        while (i < (int)creditors.size() && j < (int)debtors.size()) {
            auto& [creditAmt, creditor] = creditors[i];
            auto& [debtAmt,   debtor]   = debtors[j];

            double settleAmt = min(creditAmt, abs(debtAmt));
            result.emplace_back(debtor, creditor, settleAmt);  // debtor pays creditor

            creditAmt += -settleAmt;   // reduce creditor's pending amount
            debtAmt   +=  settleAmt;   // reduce debtor's pending amount

            if (creditAmt < 0.01) i++;
            if (abs(debtAmt) < 0.01) j++;
        }
        return result;
    }

    // ── Print helpers ─────────────────────────

    void printBalances() const {
        cout << "\n── Balances in group '" << name << "' ──\n";
        bool anyBalance = false;
        for (auto& [debtor, creditorMap] : balances)
            for (auto& [creditor, amount] : creditorMap)
                if (amount > 0.01) {
                    cout << "  " << debtor.getName()
                         << " owes " << creditor.getName()
                         << " ₹" << amount << "\n";
                    anyBalance = true;
                }
        if (!anyBalance) cout << "  All settled!\n";
    }

    void printSimplifiedDebts() const {
        auto txns = simplifyDebts();
        cout << "\n── Simplified debts for group '" << name << "' ──\n";
        if (txns.empty()) { cout << "  Nothing to settle!\n"; return; }
        for (auto& [from, to, amount] : txns)
            cout << "  " << from.getName() << " → " << to.getName()
                 << " ₹" << amount << "\n";
    }
};


// ─────────────────────────────────────────────
//  SPLITWISE SERVICE  (Facade)
// ─────────────────────────────────────────────
//
//  Single entry point for all operations.
//  Owns all Users and Groups — resolves IDs to objects,
//  validates inputs, and delegates to Group for actual logic.
//
//  Why Facade?
//  Callers (main / API layer) shouldn't need to know about
//  Group internals, balance maps, or strategy objects.
//  SplitwiseService hides all that behind clean method calls.

class SplitwiseService {
private:
    map<string, User>  users;   // userId  → User
    map<string, Group> groups;  // groupId → Group

    int userCounter    = 1;     // auto-increment id generators
    int groupCounter   = 1;
    int expenseCounter = 1;
    int settleCounter  = 1;

    // ── Private helpers ───────────────────────

    // Throws if userId not found — keeps public methods clean
    User& getUser(const string& userId) {
        auto it = users.find(userId);
        if (it == users.end())
            throw invalid_argument("User not found: " + userId);
        return it->second;
    }

    // Throws if groupId not found
    Group& getGroup(const string& groupId) {
        auto it = groups.find(groupId);
        if (it == groups.end())
            throw invalid_argument("Group not found: " + groupId);
        return it->second;
    }

    // Generate a unique ID with a prefix: "u1", "g2", "e3", "s4"
    string nextId(const string& prefix, int& counter) {
        return prefix + to_string(counter++);
    }

public:

    // ── User management ───────────────────────

    User createUser(const string& name, const string& email) {
        string id = nextId("u", userCounter);
        User user(id, name, email);
        users.emplace(id, user);   // store in registry
        cout << "[Created] " << user.toString() << " with id=" << id << "\n";
        return user;
    }

    // ── Group management ──────────────────────

    Group& createGroup(const string& name) {
        string id = nextId("g", groupCounter);
        groups.emplace(id, Group(id, name));
        cout << "[Created] Group '" << name << "' with id=" << id << "\n";
        return groups.at(id);
    }

    // Validates user exists, then delegates to Group
    void joinGroup(const string& userId, const string& groupId) {
        User&  user  = getUser(userId);
        Group& group = getGroup(groupId);
        group.addMember(user);
        cout << "[Joined] " << user.getName() << " joined group '" << group.getName() << "'\n";
    }

    // Guard lives inside Group::removeMember — will throw if unsettled
    void leaveGroup(const string& userId, const string& groupId) {
        User&  user  = getUser(userId);
        Group& group = getGroup(groupId);
        group.removeMember(user);   // throws if user has unsettled balance
        cout << "[Left] " << user.getName() << " left group '" << group.getName() << "'\n";
    }

    // ── Expense management ────────────────────
    //
    //  groupId = ""  →  one-on-one expense between paidBy and participants[0]
    //  groupId = "g1" →  group expense, all participants must be group members
    //
    //  strategy choices: "equal" | "percent" | "exact"
    //  inputs: ignored for equal, percentages for percent, amounts for exact

    void addExpense(const string& paidByUserId,
                    double totalAmount,
                    const string& description,
                    const vector<string>& participantIds,
                    const string& strategyType,
                    const vector<double>& inputs,
                    const string& groupId = "")   // default = one-on-one
    {
        // Resolve payer
        User& paidBy = getUser(paidByUserId);

        // Resolve all participants from their IDs
        vector<User> participants;
        for (const string& uid : participantIds)
            participants.push_back(getUser(uid));

        // Build the correct strategy based on string type
        // Factory-style selection — service shields caller from strategy classes
        shared_ptr<SplitStrategy> strategy;
        if      (strategyType == "equal")   strategy = make_shared<EqualSplitStrategy>();
        else if (strategyType == "percent") strategy = make_shared<PercentSplitStrategy>();
        else if (strategyType == "exact")   strategy = make_shared<ExactSplitStrategy>();
        else throw invalid_argument("Unknown strategy: " + strategyType);

        // Build the Expense — constructor computes splits internally
        string expenseId = nextId("e", expenseCounter);
        Expense expense(expenseId, description, totalAmount, paidBy, participants, strategy, inputs);
        expense.print();

        if (groupId.empty()) {
            // One-on-one: create a temporary group-less context
            // For simplicity: directly update a global user balance
            // In production you'd have a separate UserBalanceManager
            // For this interview scope: require a group even for 1-on-1
            throw invalid_argument("One-on-one expenses require a group context in this implementation");
        } else {
            Group& group = getGroup(groupId);
            group.addExpense(expense);   // Group updates its balance map
        }
    }

    // ── Settlement ────────────────────────────

    // Settles the full balance that 'fromUserId' owes 'toUserId' in a group
    void settle(const string& fromUserId,
                const string& toUserId,
                const string& groupId) {
        User&  from  = getUser(fromUserId);
        User&  to    = getUser(toUserId);
        Group& group = getGroup(groupId);
        string sid   = nextId("s", settleCounter);
        group.settle(from, to, sid);    // Group zeros balance + logs Settlement
    }

    // ── Query / Print helpers ─────────────────

    void printGroupBalances(const string& groupId) {
        getGroup(groupId).printBalances();
    }

    void printSimplifiedDebts(const string& groupId) {
        getGroup(groupId).printSimplifiedDebts();
    }

    void printGroupSettlements(const string& groupId) {
        Group& group = getGroup(groupId);
        cout << "\n── Settlement history for group '" << group.getName() << "' ──\n";
        for (const Settlement& s : group.getSettlements())
            s.print();
    }
};


// ─────────────────────────────────────────────
//  MAIN  — demo / smoke test
// ─────────────────────────────────────────────

int main() {
    SplitwiseService service;

    // Create users
    User alice   = service.createUser("Alice",   "alice@mail.com");
    User bob     = service.createUser("Bob",     "bob@mail.com");
    User charlie = service.createUser("Charlie", "charlie@mail.com");

    // Create group and add members
    Group& goa = service.createGroup("Goa Trip");
    service.joinGroup(alice.getId(),   goa.getId());
    service.joinGroup(bob.getId(),     goa.getId());
    service.joinGroup(charlie.getId(), goa.getId());

    // Expense 1: Alice pays ₹300 for dinner, split equally
    service.addExpense(alice.getId(), 300, "Dinner",
                       {alice.getId(), bob.getId(), charlie.getId()},
                       "equal", {}, goa.getId());

    // Expense 2: Bob pays ₹150 for taxi, split by percentage
    service.addExpense(bob.getId(), 150, "Taxi",
                       {alice.getId(), bob.getId(), charlie.getId()},
                       "percent", {50.0, 30.0, 20.0}, goa.getId());

    // Expense 3: Charlie pays ₹120 for ice cream, split exact
    service.addExpense(charlie.getId(), 120, "Ice Cream",
                       {alice.getId(), bob.getId(), charlie.getId()},
                       "exact", {60.0, 40.0, 20.0}, goa.getId());

    // Print raw balances and simplified debts
    service.printGroupBalances(goa.getId());
    service.printSimplifiedDebts(goa.getId());

    // Bob settles with Alice
    service.settle(bob.getId(), alice.getId(), goa.getId());
    service.printGroupBalances(goa.getId());

    // Print settlement log
    service.printGroupSettlements(goa.getId());

    return 0;
}
