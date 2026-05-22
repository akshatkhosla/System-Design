#include <iostream>
#include <string>


// ─── Adapter Design Pattern ───────────────────────────────────────────────────
// Problem: Our reporting system expects JSON data via IReports.
//          We have a legacy XMLDataProvider that only speaks XML.
// Solution: XmlToJsonAdapter wraps the XML provider and translates its output
//           into JSON — the client never knows XML was involved.
//
//   Client (main)
//       │ uses IReports*
//       ▼
//   XmlToJsonAdapter          ← Adapter: implements IReports, wraps XMLDataProvider
//       │ calls getXmlData()
//       ▼
//   XMLDataProvider           ← Adaptee: the incompatible legacy class


// ─── Target Interface ─────────────────────────────────────────────────────────
// What the client expects — JSON data as a return value.
class IReports {
public:
    virtual std::string getJsonData() const = 0;
    virtual ~IReports() = default;
};


// ─── Adaptee ──────────────────────────────────────────────────────────────────
// Existing legacy class with an incompatible interface.
// We cannot (or should not) modify this.
class XMLDataProvider {
public:
    std::string getXmlData() const {
        return "<report>"
                   "<title>Annual Report</title>"
                   "<content>Report content goes here...</content>"
               "</report>";
    }
};


// ─── Adapter ──────────────────────────────────────────────────────────────────
// Bridges XMLDataProvider → IReports.
// Injecting XMLDataProvider lets us swap in any XML source without changing the adapter.
class XmlToJsonAdapter : public IReports {
public:
    explicit XmlToJsonAdapter(XMLDataProvider& provider) : provider_(provider) {}

    std::string getJsonData() const override {
        std::string xml = provider_.getXmlData();
        std::string title   = extractTag(xml, "title");
        std::string content = extractTag(xml, "content");
        return "{ \"report\": { \"title\": \"" + title + "\", \"content\": \"" + content + "\" } }";
    }

private:
    // Pulls the text between <tag>...</tag> from a simple XML string.
    // Not a general-purpose XML parser — sufficient for this known fixed format.
    static std::string extractTag(const std::string& xml, const std::string& tag) {
        std::string open  = "<"  + tag + ">";
        std::string close = "</" + tag + ">";
        size_t start = xml.find(open)  + open.size();
        size_t end   = xml.find(close);
        return xml.substr(start, end - start);
    }

    XMLDataProvider& provider_;
};


int main() {
    XMLDataProvider xmlProvider;
    // Old way (manual memory management):
    //   IReports* report = new XmlToJsonAdapter(xmlProvider);
    //   std::cout << "JSON Data: " << report->getJsonData() << "\n";
    //   delete report;   // must be called manually — easy to forget or skip on exception

    // New way (unique_ptr owns the object and deletes it automatically when it goes out of scope):
    std::unique_ptr<IReports> report = std::make_unique<XmlToJsonAdapter>(xmlProvider);

    std::cout << "JSON Data: " << report->getJsonData() << "\n";

    return 0;
}
