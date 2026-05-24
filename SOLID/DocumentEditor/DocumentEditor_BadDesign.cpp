#include<iostream>
#include<vector>
#include<string>
#include<fstream>

using namespace std;

class DocumentEditor {
    private:
    vector<string> documentElement;
    string renderedDocuments;

    public:
    void addText(string text) {
        documentElement.push_back(text);
    }
    void addImages(string path) {
        documentElement.push_back(path);
    }

    string renderDocument() {
        if(renderedDocuments.empty()) {
            string result;
            for (auto element : documentElement) {
                if (element.size() > 4 && (element.substr(element.size() - 4) == ".jpg" ||
                 element.substr(element.size() - 4) == ".png")) {
                    result += "[Image: " + element + "]" + "\n";
                } else {
                    result += element + "\n";
                }
            }
            renderedDocuments = result;
        }
        return renderedDocuments;
    }
    void saveToFile() {
        ofstream file("document.txt");
        if(file.is_open()) {
            file << renderDocument();
            file.close();
            cout << "Document saved to document.txt" << endl;
        }
        else {
            cout << "Error occured : Unable to save to DB" << endl;
        }
    }
};

int main() {
    DocumentEditor editor;
    editor.addText("Hello World!");
    editor.addImages("picture.jpg");
    editor.addText("This is a document editior");

    cout << editor.renderDocument();

    editor.saveToFile();
    return 0;
}