#include <iostream>
#include <string>
#include <memory>

// ─── Virtual Proxy Design Pattern ────────────────────────────────────────────
// Problem: Loading a high-resolution image is expensive (disk I/O, memory).
//          We want the object to exist immediately but defer the cost until
//          the image is actually needed for display.
//
// Solution: ImageProxy and RealImage both implement IDisplayable.
//           The proxy holds only the filename until display() is first called,
//           then loads the real image exactly once and caches it.
//           The client talks to IDisplayable — it never knows which it holds.
//
// Structure:
//   Client
//     │ uses IDisplayable*
//     ▼
//   ImageProxy          ← stands in for RealImage, defers loading
//     │ owns (lazily)
//     ▼
//   RealImage           ← expensive to construct, loaded on first display()


// ─── Subject Interface ────────────────────────────────────────────────────────
class IDisplayable {
public:
    virtual void display() const = 0;
    virtual ~IDisplayable() = default;
};


// ─── Real Subject ─────────────────────────────────────────────────────────────
// Expensive to construct — simulates disk I/O at construction time.
class RealImage : public IDisplayable {
public:
    explicit RealImage(const std::string& filename) : filename_(filename) {
        std::cout << "[RealImage] Loading from disk: " << filename_ << "\n";
    }

    void display() const override {
        std::cout << "[RealImage] Displaying: " << filename_ << "\n";
    }

private:
    std::string filename_;
};


// ─── Virtual Proxy ────────────────────────────────────────────────────────────
// Cheap to construct — stores only the filename, no disk access.
// RealImage is created on the first display() call and reused on every subsequent one.
class ImageProxy : public IDisplayable {
public:
    explicit ImageProxy(const std::string& filename) : filename_(filename) {
        std::cout << "[ImageProxy] Proxy created for: " << filename_ << " (not loaded yet)\n";
    }

    void display() const override {
        // mutable allows realImage_ to be assigned inside this const method.
        // This is the only place RealImage is ever constructed.
        if (!realImage_)
            realImage_ = std::make_unique<RealImage>(filename_);
        realImage_->display();
    }

private:
    std::string filename_;
    mutable std::unique_ptr<RealImage> realImage_; // null until first display()
};


int main() {
    // Client holds IDisplayable* — works identically whether proxy or real image
    std::unique_ptr<IDisplayable> image = std::make_unique<ImageProxy>("high_res_photo.jpg");

    std::cout << "\n-- First display() call: triggers lazy load --\n";
    image->display();

    std::cout << "\n-- Second display() call: uses cached RealImage --\n";
    image->display();

    return 0;
}
