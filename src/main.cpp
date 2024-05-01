#include <iostream>
#include <fstream>
#include <utility>
#include <ctime>
#include <thread>
#include <unistd.h>
#include <cstring>

int getopt(int argc, char * const argv[], const char* optsring);
extern char* optarg;
extern int optind, opterr, optopt;

#include "../include/convert.h"

using ThreadInput = std::pair<std::string, mesh::Convert_param>;

void Schedule_parse(std::string & schedule, std::vector<ThreadInput>& res, mesh::Convert_param & param)
{
    std::ifstream read(schedule);
    if(!read)
        exit(-2);

    std::string buffer;
    while(!(read.eof() ||read.fail()))
    {
            std::getline(read, buffer);
            mesh::Convert_param p = param;
            if(!buffer.empty())
                res.push_back(ThreadInput(buffer,param));
    }
}

void CL_parse(int argc, char * const argv[], std::vector<ThreadInput>& res)
{
    if(argc == 1)
    {
        std::cout << "relax argument" <<
        std::endl << "-f Path to file" <<
        std::endl << "-c Value of smoothing coefficient" <<
        std::endl << "-r Radius of smoothing" <<
        std::endl << "-d The percentage differece limit" <<
        std::endl << "-q Send previous parameters to execution queue" << std::endl;
        exit(0);
    }

    const char * opts = "f:c:r:d:s:";
    std::string file, schedule;
    mesh::Convert_param param;
    int opt = 0;
    while(opt != -1)
    {
        opt = getopt(argc,argv, opts);
        switch (opt)
        {
            case 'f':
                file = optarg;
                break;
            case 'c':
                param.smooth_coef = std::atof(optarg);
                break;
            case 'r':
                param.radius_smoothing = std::atof(optarg);
                break;
            case 'd':
                param.max_diff = static_cast<float>(std::atof(optarg));
                break;
            case 's':
                schedule = optarg;
                break;
        }
    }
    if (!file.empty())
        res.push_back(ThreadInput(file,param));

    if (!schedule.empty())
        Schedule_parse(schedule, res, param);

};

void threadFunction(const std::vector<ThreadInput>& param ,const size_t start, const size_t end)
{
    for(size_t m = start ;m < end; ++m)
    {
    std::string file = param[m].first;
    mesh::STLfile res(file);
    std::string pseudonym = file.substr(file.rfind('/') + 1, file.size());
    std::cout << pseudonym << " load suc, tr:" << res.triangles_num() << ", points:" << res.points_num() << ", volume:" << res.volume() << std::endl;
    std::time_t t_start = std::time(nullptr);

    for(float i = res.percent_diff(), j = 0.0f; j < param[i].second.max_diff; i = j)
    {
        res.convert(param[i].second);
        j = res.percent_diff();
        if (j < i)
            break;
    }
    file = file.substr(0,file.size() -4) + "_work.stl";
    res.save(file, mesh::FileFormat::STL_Binary);
    std::cout << pseudonym << " complete, time: " << std::time(nullptr) - t_start << " finale diff: " << res.percent_diff()<< std::endl;
    }
};

int main(int argc, char ** argv)
{
    std::vector<ThreadInput> p;
    CL_parse(argc, argv,p);
    size_t num = p.size();
    size_t mod = num % 4;
    size_t n1 = num / 4, n2 = num /4 + n1, n3 = num /4 + n2;
    if (num == 0)
        std::cerr << "No files or schedules aren't ready to run" << std::endl;
    switch (mod)
    {
        case 3:
            ++n3;
            ++n3;
        case 2:
            ++n2;
            ++n1;
    }

    std::cout << n1 << ':' << n2 - n1 << ':' << n3 - n2 << ':' << num - n3 << std::endl;

    std::thread t0(threadFunction,std::ref(p),0, n1);
    std::thread t1(threadFunction,std::ref(p),n1, n2);
    std::thread t2(threadFunction,std::ref(p),n2, n3);
    std::thread t3(threadFunction,std::ref(p),n3, num);

    t0.join();
    t1.join();
    t2.join();
    t3.join();
    return 0;
}
