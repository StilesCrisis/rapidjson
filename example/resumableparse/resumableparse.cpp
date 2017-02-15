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

struct MyStringStream {
    typedef char Ch; // byte
    
    Ch Peek() const { return RAPIDJSON_UNLIKELY(pos_ == buf_.size()) ? '\0' : buf_[pos_]; }
    Ch Take() { return RAPIDJSON_UNLIKELY(pos_ == buf_.size())? '\0' : buf_[pos_++]; }
    size_t Tell() const { return pos_; }
    bool Seek(size_t offset) { pos_ = offset; RAPIDJSON_ASSERT(pos_ < buf_.size()); return true; }
    
    Ch* PutBegin() { RAPIDJSON_ASSERT(false); return 0; }
    void Put(Ch) { RAPIDJSON_ASSERT(false); }
    void Flush() { RAPIDJSON_ASSERT(false); }
    size_t PutEnd(Ch*) { RAPIDJSON_ASSERT(false); return 0; }
    
    // For encoding detection only.
    const Ch* Peek4() const {
        return pos_ + 4 <= buf_.size() ? &buf_[pos_] : 0;
    }
    
    std::string  buf_;
    size_t       pos_;
};

int main() {
    StringBuffer sb;
    PrettyWriter<StringBuffer> writer(sb);

    const char* json[] = {
        " { \"hello\" : \"world\", \"t\" : tr",
    //  " { \"hello\" : \"world\", \"t\" : trX", // to test parsing errors
        "ue, \"f\" : false, \"n\": null, \"i\":123, \"pi\": 3.14",
        "16, \"a\":[1",
        ",",
        " ",
        "2, 3, 4] ",
        "}"
    };

    MyStringStream  ss;
    
    ss.buf_ = " ";
    
    Reader reader;
    reader.IterativeParseInit();
    int jsonIndex = 0;
        
    while (!reader.IterativeParseComplete()) {
        if (!reader.IterativeParseNext<kParseResumableFlag>(ss, writer)) {
            if(jsonIndex < 7) {
                ss.buf_ += json[jsonIndex++];
            }
            if (!reader.IterativeParseResume())
                break;
        }
    }

    std::cout << sb.GetString() << std::endl;

    return EXIT_SUCCESS;
}

