#include "TH1D.h"
#include "TFile.h"
#include "TTree.h"
#include <iostream>
#include <string>
#include <vector>


void format(TH1D * h){
  h->GetXaxis()->CenterTitle();
  h->GetYaxis()->CenterTitle();
}

void PlotMaker(std::string inputFile){

  TFile * in = TFile::Open(inputFile.c_str(),"read");
  TTree * t = (TTree*)in->Get("confirmed");

  std::string * locality = 0;
  std::string * stateCountry = 0;
  std::vector<int> * confirmedCases = 0;
  std::vector<int> * day = 0;
  int firstCase = 0;

  t->SetBranchAddress("locality",&locality);
  t->SetBranchAddress("stateCountry",&stateCountry);
  t->SetBranchAddress("firstCaseDay",&firstCase);
  t->SetBranchAddress("confirmedCases",&confirmedCases);
  t->SetBranchAddress("day",&day);

  TCanvas * c1 = new TCanvas("c1","c1",800,800);
  c1->SetLeftMargin(0.15);

  TFile * out = TFile::Open("histograms.root","recreate");

  for(int i = 0; i<t->GetEntries(); i++){
    t->GetEntry(i);
    std::cout << *locality << " " << *stateCountry << std::endl;

    std::cout << confirmedCases->size() << " " << firstCase << std::endl;

    TH1D * h = new TH1D(Form("%s",(*locality+"_"+*stateCountry).c_str()),";Days Since First Case;Cases",(int)confirmedCases->size()-firstCase+1,0,(int)confirmedCases->size()-firstCase+1);
    t->Draw(Form("(day-firstCaseDay)>>%s",(*locality+"_"+*stateCountry).c_str()),"confirmedCases*(confirmedCases>0)","HIST",1,i);
    h->SetStats(0);
    h->SetFillColor(kRed+1);
    h->SetLineColor(kBlack);
    format(h);
    h->Draw("HIST F");
    c1->SaveAs(Form("plots/%s.png",(*locality+"_"+*stateCountry).c_str()));

    h->Write();

    delete h;
  }
}
