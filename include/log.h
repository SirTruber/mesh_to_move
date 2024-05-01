#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

namespace da
{

struct nullstream: std::ostream
{
    nullstream(): std::ostream(0){};
    nullstream(const std::string& f) : std::ostream(std::ofstream(f)){};
};

template <typename T>
nullstream &operator<<(nullstream &o, T const & x) { return o;}

static nullstream log ;



class LogMessage
{
public:
    bool record = false;
public:
    inline std::ostream &stream()
    {
        return ofs;
    }

    LogMessage(const std::string& l): level(l), ofs(std::cerr)
    {
        stream() << "[" << level << "]\t";
    }

    LogMessage(std::ostream &o):level("ERROR"), ofs(o)
    {
        stream() << "[" << level << "]\t";
    }

    ~LogMessage()
    {
        if(record)
            LogMessage r(level);
        stream() << std::ctime(&mes_time);
    }
private:
    std::time_t mes_time = std::time(nullptr);
    std::ostream &ofs;
    std::string level;
};
}
