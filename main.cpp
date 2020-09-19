#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include <fstream>
#include <cassert>
#include <ctime>

struct Param
{
    std::string name;
    std::vector<std::string> values;
    int modulo;
    int divider;
};

std::vector<Param> params;
std::vector<Param> SYSparams;
std::vector<std::string> paramGroups;

std::string path;
std::string mytime;


void addParam(std::string str)
{
    size_t pos1 = str.find("=");
    Param param;

    param.name = str.substr(0,pos1);

    size_t pos2 = str.find(",");
    while(pos2!=std::string::npos){
        param.values.push_back( str.substr(pos1+1,pos2-pos1-1));
        pos1=pos2;
        pos2 = str.find(",",pos1+1);
    }

    param.values.push_back( str.substr(pos1+1));

    if(param.name.find("PROG")!=std::string::npos){
        SYSparams.push_back(param);
    } else if(param.name.find("PBS")!=std::string::npos){
        SYSparams.push_back(param);
    } else {
        params.push_back(param);
    }
}

int calcNumberOfParamGroups(std::vector<Param> &params)
{
    int out=1;
    for(auto param : params){
        out *= param.values.size();
    }
    return out;
}

void calcDividers(std::vector<Param> &params)
{
    int tmp=1;
    for(auto &param : params){
        param.modulo = param.values.size();
        param.divider = tmp;
        tmp *= param.modulo;
    }
}



std::vector<std::string> generateParamsGroups(std::vector<Param> &params)
{
    std::vector<std::string> out;

    for(int i=0; i<calcNumberOfParamGroups(params); i++){
        std::string group=" ";
        for(auto &param : params){
            group.append(param.name + "=" + param.values[int(i/param.divider) % param.modulo] + " ");
        }
        out.push_back(group);
        std::cout << group << std::endl;
    }
    return out;
}

Param* param(std::string name)
{
    for(auto &param : params)
    {
        if(param.name==name) return &param;
    }
    for(auto &param : SYSparams)
    {
        if(param.name==name) return &param;
    }
    std::cerr << "ERROR: Unknown param name! ->" << name << std::endl;
    assert(false);
    return nullptr;
}

std::string p(std::string name, int num=0)
{
    return param(name)->values[num];
}

void generateScriptForGroup(int num)
{
    std::string outName = path + "/" + std::to_string(num)+".data";

    std::fstream file(std::string(path+"/"+std::to_string(num)+".sh"), std::ios::out);

    file << "#PBS -S /bin/bash" << std::endl;
    file << "#PBS -q main" << std::endl;
    file << "#PBS -l walltime=" << p("PBStime") << std::endl;
    file << "#PBS -l select=1:ncpus=1:mem=" << p("PBSmem") << std::endl;

    file << std::endl;

    file << p("PROGpath") << "/" << p("PROGname");
    file << paramGroups[num];
    file << "> " << outName << std::endl;

    file << std::endl;

    file << "qstat -f $PBS_JOBID | grep resources_used.vmem > " << outName << std::endl;
    file << "qstat -f $PBS_JOBID | grep resources_used.walltime > " << outName << std::endl;

    file.close();
}

std::string mtime()
{
    std::time_t t = std::time(0);
    std::tm* now = std::localtime(&t);

    std::string out="";
    out += std::to_string(now->tm_hour) + '_';
    out += std::to_string(now->tm_min) + '_';
    out += std::to_string(now->tm_sec) + '_';
    out += std::to_string(now->tm_mday) + '_';
    out += std::to_string(now->tm_mon+1) + '_';
    out += std::to_string(now->tm_year+1900);
    return out;
}

void run()
{
    for(int i=0; i<paramGroups.size(); i++){
        std::cout << "qsub -N "+std::to_string(i) + "_"+mytime+" "+path+"/"+std::to_string(i)+".sh" <<std::endl;
        if(p("PBSrun")=="1"){
            system(("qsub -N "+std::to_string(i) + "_"+mytime+" "+std::to_string(i)+".sh").c_str());
        }
    }
}

void writeHelp()
{
    std::cout << "SpecialArgs:" << std::endl;
    std::cout << "  PBSmem" << std::endl;
    std::cout << "  PBStime" << std::endl;
    std::cout << "  PBSrun=1" << std::endl;
    std::cout << "  PROGpath" << std::endl;
    std::cout << "  PROGname" << std::endl;
}


int main(int argc, char *argv[])
{
    for(int i=1; i<argc; i++){
        addParam(argv[i]);
    }

    calcDividers(params);
    paramGroups = generateParamsGroups(params);

    mytime = mtime();
    path = p("PROGpath")+"/data_"+mytime;

    system(("mkdir "+path).c_str());

    for(int i=0; i<paramGroups.size(); i++){
        generateScriptForGroup(i);
    }

    run();
    return 0;
}

