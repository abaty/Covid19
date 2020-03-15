#include "TTree.h"
#include "TFile.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>


void fillUS(std::string stateCountry, int lineNum, std::vector<int> &totals, int counter, int cases){

  int entryNum = lineNum-2;

  if( stateCountry.compare("US") == 0 ){
    if((entryNum>90 && entryNum < 160) || entryNum==420 || entryNum==424 || entryNum==440 ){

      std::cout << entryNum << " " << counter <<  std::endl;

      if(entryNum==100) totals.push_back(cases);
      else totals.at(counter) += cases;
      
    }
  }
}

void TreeMaker(std::string input){

  std::string buffer;
  std::ifstream inFile(input.data());

  std::string locality;
  std::string stateCountry;
  std::vector<int> confirmedCases;
  std::vector<int> day;
  int firstCase;

  std::vector<int> USTotals;

  size_t underscore = input.find("_");
  size_t suffix = input.find(".txt");
  std::string file = input.substr(underscore+1,suffix-underscore-1);
  TFile * output = TFile::Open((file+".root").c_str(),"recreate");
  TTree * t = new TTree("confirmed","confirmed");
 
  t->Branch("locality",&locality); 
  t->Branch("stateCountry",&stateCountry); 
  t->Branch("confirmedCases",&confirmedCases);
  t->Branch("day",&day);
  t->Branch("firstCaseDay",&firstCase);

  int line = 0;
  while(true)
  {
    inFile >> buffer;
    if(inFile.eof()) break;
   
    //skip first line that has the data format 
    line++;
    if(line==1) continue;

    locality = "";
    stateCountry = "";
    confirmedCases.clear();
    day.clear();
    firstCase = -1;

    //look for the first comma and get the string before it
    size_t found = buffer.find(","); 
    if (found != string::npos){ 
      locality = buffer.substr(0,found);
    }
   
    size_t found2 = buffer.find(",",found+1);
    if (found2 != string::npos){ 
      stateCountry = buffer.substr(found+1,found2-(found+1));
    }

    //skip latitude and longitude data 
    size_t found3 = buffer.find(",",found2+1);
    size_t found4 = buffer.find(",",found3+1);

    //beginning of time series data on 1/22/2020
    
    int dayIndex = 1;
    bool isMoreData = true;
    size_t foundLast = found4;
    while(isMoreData){
      size_t foundNext = buffer.find(",",foundLast+1);
      if (foundNext != string::npos){ 
        int nCases = std::stoi( buffer.substr( foundLast+1, foundNext-(foundLast+1) ) );
        confirmedCases.push_back( nCases );
        fillUS( stateCountry, line, USTotals, dayIndex-1 , nCases);
        day.push_back( dayIndex );
        dayIndex++;
        foundLast = foundNext;
        if(firstCase==-1 && nCases!=0) firstCase = confirmedCases.size()-1; 
      }
      else{
        int nCases = std::stoi( buffer.substr( foundLast+1 ) );
        confirmedCases.push_back( nCases );
        fillUS( stateCountry, line, USTotals, dayIndex-1 , nCases);
        day.push_back( dayIndex );
        dayIndex++;
        if(firstCase==-1 && nCases!=0) firstCase = confirmedCases.size()-1; 
        isMoreData = false;
      }
    }

    std::cout << locality << " " << stateCountry << " " << firstCase << " ";
    for(int i = 0; i<confirmedCases.size(); i++){
      std::cout << confirmedCases.at(i) << " ";
    } 
    std::cout << std::endl;
 
    t->Fill();  
 
  }

  //add US totals
  locality = "";
  stateCountry = "US";
  confirmedCases = USTotals;
  for(int i = 0; i<confirmedCases.size(); i++){
    if(confirmedCases.at(i) != 0){
      firstCase=i;
      break;
    }
  } 
  t->Fill();

  output->Write();
}
