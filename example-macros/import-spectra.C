////////////
// This macro is to be used with the raw files from the X-ray experiment
// consisting in a header part like this one:
//
// DATE:
// 27/9/2018
// TIME:
// 0.0 153.0
// DATA:
// 0 4096
//
// followed by the data in one column:
//         0
//         1
//         0
//         2
//         0
//         and so on...
////////////



#include <fstream>
#include <iostream>
#include <TH1.h>
#include <string>
#include <sstream>
#include <TCanvas.h>

void SpecXrays(string infile){

  TH1I *spec = new TH1I("spec","spec",4096,-.5,4095.5);
  ifstream input;
  input.open(infile.c_str());
  if(!input.good())return;
  string aline, codeword;
  bool reachedData = false;
  int goodline=-1;
  int binContent;

  while(getline(input,aline)){
    if(aline.empty()) continue;
    stringstream oneLine (aline);
    oneLine >> codeword;

    if(!codeword.compare("DATA:")){
      cout <<"Reached the data" << endl;
      reachedData = true;
      getline(input,aline);
      goodline = 0;
      continue;
    }
    if(!reachedData) continue;
    spec->SetBinContent(goodline, atoi( codeword.c_str() ) );
    goodline++;
  }

  spec->Draw();
  
}

