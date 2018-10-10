#include<stdio.h>
#include<cstdlib>
#include<iostream>
#include<string.h>
#include<fstream>
#include<dirent.h>
#include <vector>
#include <alpr.h>
#include <iterator>

using namespace std;

vector<string> processimage(string path, string imageName);
void listFile();

int main(){
    listFile();
    return 0;
}

void listFile(){
    DIR *pDIR;
    std::vector<string> v;
    vector<string> platerec;

    struct dirent *entry;
    if( pDIR=opendir("/home/pinje/projects/CountingCars/images/") ){

        while(entry = readdir(pDIR)){
            if( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 )
            v.push_back(entry->d_name);
        }
        closedir(pDIR);
    }

//    std::cout << "images from folder:"<<endl;
    ofstream myfile;
    myfile.open ("platesrecognized.txt");

    std::ostream_iterator<std::string> output_iterator(myfile, "\n");

    for (unsigned i=0; i<v.size(); i++) {
//        std::cout << ' ' << v.at(i) << endl;
        platerec = processimage("/home/pinje/projects/CountingCars/images/", v.at(i));
//        std::cout << "Memory pointer " << platerec << endl;
//        std::cout << "Memory variable " << *platerec << endl;
        std::copy(platerec.begin(), platerec.end(), output_iterator);

    }
    myfile.close();

}

vector<string> processimage(string path, string imageName) {
    string imagelocation = path+imageName;
    string* bestPlate = new string;
    std::vector< string >  platerecimagepath;
//    std::cout << "Image Path "<< imagelocation << endl;
    alpr::Alpr *openalpr= new alpr::Alpr("eu,vn2", "/etc/openalpr/openalpr.conf");
    openalpr->setTopN(20);
    // comparing the plate text with the regional pattern.
    openalpr->setDefaultRegion("md");
    // Make sure the library loaded before continuing.
    // For example, it could fail if the config/runtime_data is not found
    if (!openalpr->isLoaded())
    {
        std::cerr << "Error loading OpenALPR" << std::endl;
//        return 1;
    }

    // Recognize an image file.  You could alternatively provide the image bytes in-memory.
    alpr::AlprResults results = openalpr->recognize(imagelocation);
    for (int i = 0; i < results.plates.size(); i++)
    {
        alpr::AlprPlateResult plate = results.plates[i];
//        std::cout << "plate" << i << ": " << plate.topNPlates.size() << " results" << std::endl;
        *bestPlate = results.plates[i].bestPlate.characters+"\n";
        if(bestPlate){
            platerecimagepath.push_back(imagelocation);
            platerecimagepath.push_back(*bestPlate);
        }

        for (auto k = 0; k < plate.topNPlates.size(); k++)
        {
            alpr::AlprPlate candidate = plate.topNPlates[k];
//            std::cout << "    - " << candidate.characters << "\t confidence: " << candidate.overall_confidence;
//            std::cout << "\t pattern_match: " << candidate.matches_template << std::endl;
//            std::cout << "Best plate result" << i << ": " << results.plates[i].bestPlate.characters << " Results" << std::endl;
        }
    }
    
    delete openalpr;
    return platerecimagepath;
}