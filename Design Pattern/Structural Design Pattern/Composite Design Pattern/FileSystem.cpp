#include <iostream>
#include <memory>
#include <string>
#include <vector>

// ─── Composite Design Pattern ─────────────────────────────────────────────────
// Problem: A file system has files and folders. Folders can contain files OR
//          other folders. Operations like getSize() or ls() must work the same
//          way regardless of whether you're dealing with a single file or a
//          deeply nested folder tree.
//
// Solution: Both File (leaf) and Folder (composite) implement the same
//           FileSystemNode interface. A Folder holds a list of FileSystemNodes,
//           so it can contain any mix of files and sub-folders. The client
//           calls the same methods on both — no special-casing needed.
//
// Tree structure built in main:
//
//   root/                        ← Folder (composite)
//   ├── resume.pdf               ← File   (leaf)
//   ├── notes.txt                ← File   (leaf)
//   └── projects/                ← Folder (composite)
//       ├── main.cpp             ← File   (leaf)
//       └── assets/              ← Folder (composite)
//           └── logo.png         ← File   (leaf)


// ─── Component Interface ──────────────────────────────────────────────────────
// The single interface that both File and Folder implement.
// Client code never needs to know which concrete type it holds.
class FileSystemNode {
public:
    virtual void ls(int indent = 0) const = 0;  // print tree, indented by depth
    virtual void openAll() const = 0;            // open this node (and all children for folders)
    virtual int getSize() const = 0;             // size in bytes (folders: sum of all children)
    virtual std::string getName() const = 0;
    virtual ~FileSystemNode() = default;
};


// ─── Leaf ─────────────────────────────────────────────────────────────────────
// A File has no children. Every operation is trivially answered from its own data.
class File : public FileSystemNode {
public:
    File(const std::string& name, int size) : name_(name), size_(size) {}

    void ls(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "- " << name_
                  << " (" << size_ << " bytes)\n";
    }

    void openAll() const override {
        std::cout << "  Opening file: " << name_ << "\n";
    }

    int getSize() const override { return size_; }
    std::string getName() const override { return name_; }

private:
    std::string name_;
    int size_;
};


// ─── Composite ────────────────────────────────────────────────────────────────
// A Folder holds any number of FileSystemNodes — files or other folders.
// Every operation delegates to its children recursively, so the caller
// never needs to know the depth or shape of the tree.
class Folder : public FileSystemNode {
public:
    explicit Folder(const std::string& name) : name_(name) {}

    // Takes ownership of the child node — folder is responsible for its lifetime
    void add(std::unique_ptr<FileSystemNode> child) {
        children_.push_back(std::move(child));
    }

    // Prints the folder name, then recursively prints each child indented one level deeper.
    // indent=0 at the root; each recursive call adds 2 spaces per level.
    void ls(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "[" << name_ << "/]\n";
        for (const auto& child : children_)
            child->ls(indent + 2);
    }

    // Opens every node in the subtree — folder itself has no "open" action,
    // so it just forwards to each child.
    void openAll() const override {
        std::cout << "  Entering folder: " << name_ << "\n";
        for (const auto& child : children_)
            child->openAll();
    }

    // Folder size = sum of all children's sizes, recursively.
    // A nested folder's getSize() will in turn sum its own children — no extra logic needed here.
    int getSize() const override {
        int total = 0;
        for (const auto& child : children_)
            total += child->getSize();
        return total;
    }

    std::string getName() const override { return name_; }

private:
    std::string name_;
    std::vector<std::unique_ptr<FileSystemNode>> children_; // owns all children
};


int main() {
    // Build the tree bottom-up: leaves first, then composites that own them.

    auto assets = std::make_unique<Folder>("assets");
    assets->add(std::make_unique<File>("logo.png", 204800));

    auto projects = std::make_unique<Folder>("projects");
    projects->add(std::make_unique<File>("main.cpp", 3200));
    projects->add(std::move(assets));          // folder owns sub-folder now

    auto root = std::make_unique<Folder>("root");
    root->add(std::make_unique<File>("resume.pdf", 51200));
    root->add(std::make_unique<File>("notes.txt", 1024));
    root->add(std::move(projects));            // folder owns sub-folder now

    // ── ls(): the same call works on the whole tree ──
    std::cout << "=== Directory listing ===\n";
    root->ls();

    // ── getSize(): sums recursively across the entire tree ──
    std::cout << "\n=== Total size ===\n";
    std::cout << root->getName() << " total: " << root->getSize() << " bytes\n";

    // ── openAll(): traverses every node uniformly ──
    std::cout << "\n=== Open all ===\n";
    root->openAll();

    // unique_ptr destructors unwind the tree automatically — no manual delete needed
    return 0;
}
