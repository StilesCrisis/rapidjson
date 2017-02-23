// Example of resuming JSON parsing


#include "../../include/rapidjson/error/en.h"
#include "../../include/rapidjson/reader.h"
#include "../../include/rapidjson/writer.h"
#include "../../include/rapidjson/prettywriter.h"
#include "../../include/rapidjson/istreamwrapper.h"
#include "../../include/rapidjson/ostreamwrapper.h"
#include <deque>
#include <iostream>

using namespace rapidjson;
using namespace std;

struct MyStringStream {
    typedef char Ch; // byte
    
    Ch Peek() const { return RAPIDJSON_UNLIKELY(pos_ == buf_.size()) ? '\0' : buf_[pos_]; }
    Ch Take() { return RAPIDJSON_UNLIKELY(pos_ == buf_.size())? '\0' : buf_[pos_++]; }
    size_t Tell() const { return pos_; }
    bool Seek(size_t offset) { pos_ = offset; RAPIDJSON_ASSERT(pos_ <= buf_.size()); return true; }
    
    Ch* PutBegin() { RAPIDJSON_ASSERT(false); return 0; }
    void Put(Ch) { RAPIDJSON_ASSERT(false); }
    void Flush() { RAPIDJSON_ASSERT(false); }
    size_t PutEnd(Ch*) { RAPIDJSON_ASSERT(false); return 0; }
    
    // For encoding detection only.
    const Ch* Peek4() const {
        return pos_ + 4 <= buf_.size() ? &buf_[pos_] : 0;
    }
    
    std::string  buf_;
    size_t       pos_ = 0;
};


struct MyHandler {
    bool Null() { cout << "Null()" << endl; return true; }
    bool Bool(bool b) { cout << "Bool(" << boolalpha << b << ")" << endl; return true; }
    bool Int(int i) { cout << "Int(" << i << ")" << endl; return true; }
    bool Uint(unsigned u) { cout << "Uint(" << u << ")" << endl; return true; }
    bool Int64(int64_t i) { cout << "Int64(" << i << ")" << endl; return true; }
    bool Uint64(uint64_t u) { cout << "Uint64(" << u << ")" << endl; return true; }
    bool Double(double d) { cout << "Double(" << d << ")" << endl; return true; }
    bool RawNumber(const char* str, SizeType length, bool copy) {
        cout << "Number(" << str << ", " << length << ", " << boolalpha << copy << ")" << endl;
        return true;
    }
    bool String(const char* str, SizeType length, bool copy) {
        cout << "String(" << str << ", " << length << ", " << boolalpha << copy << ")" << endl;
        return true;
    }
    bool StartObject() { cout << "StartObject()" << endl; return true; }
    bool Key(const char* str, SizeType length, bool copy) {
        cout << "Key(" << str << ", " << length << ", " << boolalpha << copy << ")" << endl;
        return true;
    }
    bool EndObject(SizeType memberCount) { cout << "EndObject(" << memberCount << ")" << endl; return true; }
    bool StartArray() { cout << "StartArray()" << endl; return true; }
    bool EndArray(SizeType elementCount) { cout << "EndArray(" << elementCount << ")" << endl; return true; }
};



int main() {
    const char* json = " { \"hello\" : \"world\", \"unicode\": \"\\u0055\\u006e\\u0069\\u0063od\\u0065\", \"t\" : true, \"f\" : false, \"n\": null, \"i\":123, \"pi\": 3.1416, \"Infinity\": Inf, \"a\":[1, 2, 3,4] }  ";
    const char* jsonPtr = json;
    
    MyStringStream  ss;
    ss.buf_ = "";
    
    Reader reader;
    reader.IterativeParseInit();

    MyHandler writer;
    
    while (!reader.IterativeParseComplete()) {
        if (!reader.IterativeParseNext<kParseResumableFlag | kParseNanAndInfFlag>(ss, writer)) {
            if (!reader.IterativeParseResume()) {
                cout << "*** invalid JSON (" << reader.GetParseErrorCode() << ")" << endl;
                break;
            }
            if(*jsonPtr) {
                ss.buf_.push_back(*jsonPtr++);
                cout << "parsing stream: " << ss.buf_ << endl;
            }
        }
    }

    return EXIT_SUCCESS;
}

