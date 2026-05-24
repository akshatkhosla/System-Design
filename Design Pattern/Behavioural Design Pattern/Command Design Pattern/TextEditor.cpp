#include<iostream>
#include<string>
#include<stack>

using namespace std;

// Command Design Pattern says that you can 
// decouple the object that invokes the operation from the one that knows how to perform it. 
// This pattern is also known as Action or Transaction.

class ICommand {
public:
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual ~ICommand() {}
};

class TextEditor {
    string content;
    string clipboard;
public:
    void write(const string& text, int position) {
        content.insert(position, text);
    }
    void deletes(int position, int length) {
        content.erase(position, length);
    }
    void copy(int start, int end) {
        clipboard = content.substr(start, end - start);
    }
    void paste(int position) {
        content.insert(position, clipboard);
    }
    // Needed by DeleteCommand to snapshot text before erasing it.
    string getSubstring(int position, int length) const {
        return content.substr(position, length);
    }
    void getContent() const {
        cout << content << endl;
    }
};

class WriteCommand : public ICommand {
private:
    TextEditor* editor;
    string text;
    int position;
public:
    WriteCommand(TextEditor* editor, const string& text, int position) : editor(editor), text(text), position(position) {}
    void execute() override {
        editor->write(text, position);
    }
    void undo() override {
        editor->deletes(position, text.length());
    }
};

class DeleteCommand : public ICommand {
private:
    TextEditor* editor;
    string deletedText;
    int position;
    int length;
public:
    DeleteCommand(TextEditor* editor, int position, int length) : editor(editor), position(position), length(length) {}
    void execute() override {
        // Snapshot the text before erasing so undo can restore it.
        deletedText = editor->getSubstring(position, length);
        editor->deletes(position, length);
    }
    void undo() override {
        editor->write(deletedText, position);
    }
};

class CopyCommand : public ICommand {
private:    
    TextEditor* editor;
    int start;
    int end;
public:
    CopyCommand(TextEditor* editor, int start, int end) : editor(editor), start(start), end(end) {}
    void execute() override {
        editor->copy(start, end);
    }
    void undo() override {
        // Copy command doesn't have an undo operation
    }
};

class CommandHistory {
    stack<ICommand*> history;
public:
    ~CommandHistory() {
        while (!history.empty()) {
            delete history.top();
            history.pop();
        }
    }
    void executeCommand(ICommand* command) {
        command->execute();
        history.push(command);
    }
    void undo() {
        if (!history.empty()) {
            // Get the last executed command and undo it
            ICommand* command = history.top();
            history.pop();
            command->undo();
            delete command;
        }
    }
};  

int main() {
    TextEditor editor;
    CommandHistory history;

    history.executeCommand(new WriteCommand(&editor, "Hello ", 0));
    history.executeCommand(new WriteCommand(&editor, "World!", 6));
    editor.getContent(); // Output: Hello World!

    history.undo();
    editor.getContent(); // Output: Hello 

    history.undo();
    editor.getContent(); // Output: 

    return 0;
}
