#include <iostream>
#include <vector>
#include <set>
#include <climits>
#include <string>
#include <algorithm>

using namespace std;

// ─────────────────────────────────────────────────────────────────────────────
// CONSTANTS
// ─────────────────────────────────────────────────────────────────────────────
static const int NUM_ELEVATORS = 3;
static const int NUM_FLOORS    = 10;   // floors 0–9

// ─────────────────────────────────────────────────────────────────────────────
// ENUMS  (from diagram)
// ─────────────────────────────────────────────────────────────────────────────
enum class Direction { UP, DOWN, IDLE };

// PICK_UP / PICK_DOWN = hall call (user pressed button on a floor)
// DESTINATION         = cabin call (passenger pressed button inside elevator)
enum class RequestType { PICK_UP, PICK_DOWN, DESTINATION };

// ─────────────────────────────────────────────────────────────────────────────
// REQUEST  (from diagram)
// ─────────────────────────────────────────────────────────────────────────────
class Request {
private:
    int         floor_;
    RequestType type_;
public:
    Request(int floor, RequestType type) : floor_(floor), type_(type) {}

    int         getFloor() const { return floor_; }
    RequestType getType()  const { return type_; }

    // Needed to store in std::set — order by floor, then by type
    bool operator<(const Request& o) const {
        if (floor_ != o.floor_) return floor_ < o.floor_;
        return static_cast<int>(type_) < static_cast<int>(o.type_);
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// HELPERS
// ─────────────────────────────────────────────────────────────────────────────
static string dirStr(Direction d) {
    return d == Direction::UP ? "UP" : d == Direction::DOWN ? "DOWN" : "IDLE";
}
static string reqStr(RequestType t) {
    return t == RequestType::PICK_UP ? "PICK_UP"
         : t == RequestType::PICK_DOWN ? "PICK_DOWN" : "DESTINATION";
}

// ─────────────────────────────────────────────────────────────────────────────
// ELEVATOR  (from diagram)
//
// Algorithm — SCAN (Look):
//   Moving UP:   service floors in ascending order, then reverse to DOWN.
//   Moving DOWN: service floors in descending order, then reverse to UP.
//
// Hall calls are directional: a PICK_UP stop is only served while going UP,
// a PICK_DOWN stop only while going DOWN.
// DESTINATION stops are served regardless of direction.
// ─────────────────────────────────────────────────────────────────────────────
class Elevator {
private:
    int          id_;
    int          floor_;
    Direction    direction_;
    set<Request> requests_;   // pending stops, sorted by floor

    // ── Direction logic ──────────────────────────────────────────────────────
    // After servicing a floor, decide whether to keep going, reverse, or idle.
    void updateDirection() {
        if (requests_.empty()) {
            direction_ = Direction::IDLE;
            return;
        }

        if (direction_ == Direction::UP || direction_ == Direction::IDLE) {
            // Any stops above current floor? Keep going UP.
            for (const auto& r : requests_)
                if (r.getFloor() > floor_) { direction_ = Direction::UP; return; }
            // Nothing above → reverse DOWN.
            for (const auto& r : requests_)
                if (r.getFloor() < floor_) { direction_ = Direction::DOWN; return; }
        }

        if (direction_ == Direction::DOWN) {
            // Any stops below current floor? Keep going DOWN.
            for (const auto& r : requests_)
                if (r.getFloor() < floor_) { direction_ = Direction::DOWN; return; }
            // Nothing below → reverse UP.
            for (const auto& r : requests_)
                if (r.getFloor() > floor_) { direction_ = Direction::UP; return; }
        }

        direction_ = Direction::IDLE;
    }

    // ── Service current floor ────────────────────────────────────────────────
    // Remove and log all requests that can be satisfied at this floor given
    // the current direction of travel.
    void serviceCurrentFloor() {
        vector<Request> toRemove;

        for (const auto& r : requests_) {
            if (r.getFloor() != floor_) continue;

            bool serve = false;
            if (r.getType() == RequestType::DESTINATION)                           serve = true;
            if (r.getType() == RequestType::PICK_UP   && direction_ == Direction::UP)   serve = true;
            if (r.getType() == RequestType::PICK_DOWN && direction_ == Direction::DOWN) serve = true;

            if (serve) toRemove.push_back(r);
        }

        for (const auto& r : toRemove) {
            requests_.erase(r);
            cout << "    ✓ [E" << id_ << "] Served " << reqStr(r.getType())
                 << " at floor " << floor_ << "\n";
        }
    }

public:
    Elevator(int id, int startFloor = 0)
        : id_(id), floor_(startFloor), direction_(Direction::IDLE) {}

    // ── addRequest ───────────────────────────────────────────────────────────
    // Returns false for invalid requests (out-of-range floor).
    // No-ops (request for current floor while IDLE) are accepted but not queued.
    bool addRequest(const Request& req) {
        if (req.getFloor() < 0 || req.getFloor() >= NUM_FLOORS) return false;

        // FR-8: request for current floor while idle → no-op
        if (req.getFloor() == floor_ && direction_ == Direction::IDLE) {
            cout << "  [E" << id_ << "] No-op: already on floor " << floor_ << "\n";
            return true;
        }

        requests_.insert(req);

        // Wake from IDLE: pick a direction toward the new stop
        if (direction_ == Direction::IDLE)
            updateDirection();

        return true;
    }

    // ── step ─────────────────────────────────────────────────────────────────
    // One discrete time tick:
    //   1. Move one floor in current direction.
    //   2. Service stops at the new floor.
    //   3. Re-evaluate direction (may reverse or become IDLE).
    void step() {
        if (requests_.empty()) {
            direction_ = Direction::IDLE;
            return;   // nothing to do this tick
        }

        // 1. Move
        if      (direction_ == Direction::UP)   floor_++;
        else if (direction_ == Direction::DOWN) floor_--;

        cout << "  [E" << id_ << "] Now at floor " << floor_
             << " going " << dirStr(direction_) << "\n";

        // 2. Service stops at this floor
        serviceCurrentFloor();

        // 3. Decide next direction
        updateDirection();
    }

    // ── Getters (from diagram) ───────────────────────────────────────────────
    int       getId()        const { return id_; }
    int       getFloor()     const { return floor_; }
    Direction getDirection() const { return direction_; }

    bool isIdle()       const { return direction_ == Direction::IDLE && requests_.empty(); }
    int  pendingCount() const { return static_cast<int>(requests_.size()); }

    void printStatus() const {
        cout << "  E" << id_ << ": floor=" << floor_
             << "  dir=" << dirStr(direction_)
             << "  pending=[";
        for (const auto& r : requests_)
            cout << r.getFloor() << "(" << reqStr(r.getType()) << ") ";
        cout << "]\n";
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// ELEVATOR CONTROLLER  (from diagram — Facade / dispatcher)
//
// Dispatch algorithm — scored selection:
//   Score measures "cost" of assigning this elevator to a hall call.
//   Lower score = better choice.
//
//   Case 1 — Moving toward the request in the same direction:
//     cost = distance only  (elevator will pass through on its current sweep)
//
//   Case 2 — IDLE:
//     cost = distance + 1   (small penalty to prefer an already-en-route elevator)
//
//   Case 3 — Moving away or wrong direction:
//     cost = distance + NUM_FLOORS  (high penalty)
//
// Modification vs. diagram:
//   Added requestDestination(elevId, floor) to handle cabin calls, because
//   inside-elevator floor selections must target a specific elevator, not go
//   through dispatch.  The diagram only shows requestElevator() which covers
//   hall calls.
// ─────────────────────────────────────────────────────────────────────────────
class ElevatorController {
private:
    vector<Elevator> elevators_;

    // Lower score = better elevator to dispatch
    int dispatchScore(const Elevator& e, int targetFloor, Direction reqDir) const {
        int dist = abs(e.getFloor() - targetFloor);

        if (e.getDirection() == reqDir) {
            // Moving in same direction AND the target is still ahead of it
            bool onWay = (reqDir == Direction::UP   && e.getFloor() <= targetFloor)
                      || (reqDir == Direction::DOWN && e.getFloor() >= targetFloor);
            if (onWay) return dist;
        }

        if (e.isIdle()) return dist + 1;

        return dist + NUM_FLOORS;   // wrong direction / will overshoot
    }

public:
    ElevatorController() {
        for (int i = 0; i < NUM_ELEVATORS; ++i)
            elevators_.emplace_back(i, 0);   // all start at floor 0
    }

    // ── Hall call (from diagram: requestElevator) ────────────────────────────
    // FR-2: user presses UP or DOWN button on a floor.
    // System picks the best elevator and dispatches it.
    bool requestElevator(int floor, Direction direction) {
        // FR-7: validate
        if (floor < 0 || floor >= NUM_FLOORS) {
            cout << "[Controller] REJECTED: floor " << floor << " out of range.\n";
            return false;
        }
        if (direction == Direction::IDLE) {
            cout << "[Controller] REJECTED: IDLE is not a valid hall-call direction.\n";
            return false;
        }

        RequestType type = (direction == Direction::UP) ? RequestType::PICK_UP
                                                        : RequestType::PICK_DOWN;

        // Score each elevator and pick the best
        int bestIdx = 0, bestScore = INT_MAX;
        for (int i = 0; i < (int)elevators_.size(); ++i) {
            int s = dispatchScore(elevators_[i], floor, direction);
            if (s < bestScore) { bestScore = s; bestIdx = i; }
        }

        // FR-8: if the chosen elevator is already on that floor and idle → no-op
        // (addRequest handles logging)
        elevators_[bestIdx].addRequest(Request(floor, type));

        cout << "[Controller] Hall call floor=" << floor << " dir=" << dirStr(direction)
             << "  →  dispatched to E" << bestIdx << "\n";
        return true;
    }

    // ── Cabin call (diagram extension: requestDestination) ───────────────────
    // FR-3: passenger inside elevator 'elevId' selects a destination floor.
    bool requestDestination(int elevId, int floor) {
        if (elevId < 0 || elevId >= NUM_ELEVATORS) {
            cout << "[Controller] REJECTED: elevator id " << elevId << " is invalid.\n";
            return false;
        }
        if (floor < 0 || floor >= NUM_FLOORS) {
            cout << "[Controller] REJECTED: floor " << floor << " out of range.\n";
            return false;
        }

        // FR-8: same-floor no-op detected inside addRequest
        bool ok = elevators_[elevId].addRequest(Request(floor, RequestType::DESTINATION));
        if (ok)
            cout << "[Controller] Cabin call: E" << elevId
                 << " destination floor=" << floor << "\n";
        return ok;
    }

    // ── step (from diagram) ──────────────────────────────────────────────────
    // FR-4: advance every elevator by one discrete time tick.
    void step() {
        cout << "\n────── STEP ──────\n";
        for (auto& e : elevators_) e.step();
    }

    void printStatus() const {
        cout << "=== System Status ===\n";
        for (const auto& e : elevators_) e.printStatus();
        cout << "=====================\n";
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// MAIN — demonstration
// ─────────────────────────────────────────────────────────────────────────────
int main() {
    ElevatorController ctrl;

    cout << "\n--- Initial state ---\n";
    ctrl.printStatus();

    cout << "\n--- Issuing requests ---\n";
    ctrl.requestElevator(3, Direction::UP);    // hall: floor 3 wants to go up
    ctrl.requestElevator(7, Direction::DOWN);  // hall: floor 7 wants to go down
    ctrl.requestElevator(5, Direction::UP);    // hall: floor 5 wants to go up
    ctrl.requestDestination(0, 8);             // cabin: E0 passenger wants floor 8
    ctrl.requestDestination(0, 2);             // cabin: E0 passenger also wants floor 2

    cout << "\n--- Invalid request tests ---\n";
    ctrl.requestElevator(-1, Direction::UP);   // FR-7: out of range
    ctrl.requestElevator(10, Direction::UP);   // FR-7: out of range (max is 9)
    ctrl.requestElevator(3, Direction::IDLE);  // FR-7: IDLE not a valid hall-call dir
    ctrl.requestDestination(5, 3);             // FR-7: elevator id out of range

    cout << "\n--- No-op tests ---\n";
    // E1 and E2 have no requests yet, so they're IDLE at floor 0
    ctrl.requestDestination(1, 0);             // FR-8: E1 is already on floor 0, IDLE

    ctrl.printStatus();

    cout << "\n--- Simulating 12 steps ---\n";
    for (int i = 1; i <= 12; ++i) {
        cout << "\n[Time=" << i << "]";
        ctrl.step();
        ctrl.printStatus();
    }

    return 0;
}
