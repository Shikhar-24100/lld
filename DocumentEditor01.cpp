#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

//ABSTRACTION FOR DOCUMENT ELEMENTS
class DocumentElement {
    public:
        virtual string render() = 0; 
};

//concrete implementation for text elements
class TextElement : public DocumentElement {
    private:
        string text;

    public:
        TextElement(string text) {
            this->text = text;
        }

        string render() override {
            return text;
        }
};

//concrete implementation for image elements
class ImageElement : public DocumentElement {
    private:
        string imagePath;

    public:
        ImageElement(string imagePath) {
            this->imagePath = imagePath;
        }

        string render() override {
            return "[Image: " + imagePath + "]";
        }
};


//newLineElement = represents s line break in the document
class NewLineElement : public DocumentElement {
    public:
        string render() override {
            return "\n";
        }
};


//tabspaceElement = represents a tab space in the document
class TabSpaceElement : public DocumentElement {
    public:
        string render() override {
            return "\t";
        }
};




class Document {
    private:
        vector<DocumentElement*> documentElements;

    public:
        void addElement(DocumentElement* element) {
            documentElements.push_back(element);
        }


        string render() {
            string result;
            for(const auto& element : documentElements) {
                result += element->render();
            }
            return result;
        }
};

//persistence interface
class Persistence {
    public:
        virtual void save(string data) = 0;
};

//Fielstorage implementatio of Persistence interface
class FileStorage: public Persistence {
    public:
        void save(string data) override {
            ofstream file("document.txt");
            if(file.is_open()){
                file << data;
                file.close();
                cout<< "Document saved to document.txt" << endl;
            }else{
                cout<< "Unable to open file for writing." << endl;
            }
        }
};

//placeholder for the dbStorage impleme
class DBStorage: public Persistence {
    public:
        void save(string data) override {
            // Placeholder for database storage implementation
            cout << "Document saved to database (placeholder)." << endl;
        }
};



class DocumentEditor {
    private:
        Document * document;
        Persistence* storage;
        string renderedDocument;

    public:
        DocumentEditor(Document* document, Persistence* storage) {
            this->document = document;
            this->storage = storage;
        }

        //adds text as a plain string
        void addText(string text) {
            document->addElement(new TextElement(text));
        }

        //add images represented by its file path
        void addImage(string imagePath) {
            document->addElement(new ImageElement(imagePath));
        }


        void addNewLine() {
            document->addElement(new NewLineElement());
        }

        void addTabSpace() {
            document->addElement(new TabSpaceElement());
        }




        //renders the document by checking the type of each element at runtime
        string renderDocument() {
            if(renderedDocument.empty()) {
                renderedDocument = document->render();
            }
            return renderedDocument;
        }


        void saveDocument() {
            storage->save(renderDocument());
        }
};



int main(){
    Document* document = new Document();
    Persistence* storage = new FileStorage(); // or new DBStorage();
    
    DocumentEditor* editor = new DocumentEditor(document, storage);



    editor->addText("This is a sample document.");
    editor->addNewLine();
    editor->addText("This is another line of text.");
    editor->addNewLine();
    editor->addTabSpace();
    editor->addText("This line is indented with a tab space.");
    editor->addNewLine();
    editor->addImage("image1.png");

    cout<< editor->renderDocument() << endl;
    editor->saveDocument();
    return 0;
}