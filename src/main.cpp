#include <iostream>
#include <fstream>
#include <utility>
#include <ctime>

#include<unistd.h>
int getopt(int argc, char ** argv, const char* optsring);
extern char* optarg;
extern int optind, opterr, optopt;

#include "../include/convert.h"

struct Convert_param
{
    double smooth_coef;
    double radius_smoothing;
};

enum FileFormat{
    STL_Binary,
    STL_Text
};

class STLfile
{
public:
    std::string filename;
public:
    STLfile(std::string& file): filename(file)
    {
        if(!data.loadSTLText(file,text))
            if(!data.loadSTLBin(file,text))
            {
                std::cerr << "can't load " << file << std::endl;
                exit(-1);
            }
        start_volume = data.volume();
    };

    float triangles_num() {return data.f.size();};
    float points_num()    {return data.v.size();};
    float volume()        {return data.volume();};
    float diff()          {return start_volume - data.volume();}
    float percent_diff()  {return (100.0f*start_volume - 100.0f*data.volume())/start_volume;};

    void convert(Convert_param & param)
    {
        convert_cubic(data, param.radius_smoothing, param.smooth_coef);
    }

    bool save(std::string& file, FileFormat format)
    {
        switch (format)
        {
        case FileFormat::STL_Binary:
            return data.saveSTLBin(file, text);

        case FileFormat::STL_Text:
            return data.saveSTLText(file, text);

        default:
            return false;
        }
    }

private:
    float start_volume;
    mesh::Poly3gon data;
    std::string text;
};

int main(int argc, char ** argv)
{
    if(argc != 4)
    {
        std::cerr<< "too low argument" << std::endl;
        exit (-2);
    }
    std::string file(argv[1]);
    STLfile res(file);

    std::cout << "load suc, tr:" << res.triangles_num() << ", points:" << res.points_num() << ", volume:" << res.volume() << std::endl;

    Convert_param param;
    param.smooth_coef = std::atof(argv[2]);
    param.radius_smoothing = std::atof(argv[3]);

    res.convert(param);

    std::cout << "new volume: "<< res.volume() <<
    std::endl << "diff: "<< res.diff() << " = " << res.percent_diff() <<" %" << std::endl;

    file = file.substr(0,file.size() -4) + "_work.stl";
    res.save(file, FileFormat::STL_Binary);

    return 0;
}
