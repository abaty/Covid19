#include "TF1.h"
#include "TH1D.h"
#include "TFile.h"
#include "TTree.h"
#include "TPaveStats.h"
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

    TH1D * h = new TH1D(Form("%s",(*locality+"_"+*stateCountry).c_str()),Form("%s;Days Since First Recorded Case in Data;Cases",(*locality+"_"+*stateCountry).c_str()),(int)confirmedCases->size()-firstCase+1,-0.5,(int)confirmedCases->size()-firstCase+0.5);
    t->Draw(Form("(day-firstCaseDay)>>%s",(*locality+"_"+*stateCountry).c_str()),"confirmedCases*(confirmedCases>0)","HIST",1,i);
    for(int i = 1; i<h->GetSize()-1; i++){
      if( h->GetBinContent(i) > 0) h->SetBinError( i , TMath::Sqrt(h->GetBinContent(i)));
    }
    //h->SetStats(0);
    h->SetFillColor(kRed+1);
    h->SetLineColor(kBlack);
    h->SetLineWidth(2);
    format(h);

    //do a fit
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(1);    
    gStyle->SetStatX(0.55);
    gStyle->SetStatY(0.9);
    
    int offset = 0;
    if( stateCountry->compare("US") == 0 ) offset = 8;
    else offset = 17;

    TF1 * f1 = new TF1("f1","[0] * TMath::Exp([1] * (x-[2]))",confirmedCases->size()-firstCase+0.5 - offset,confirmedCases->size()-firstCase+0.5);
    f1->SetParameters(1,0,0);
    f1->SetParNames("A","k","t0");
    f1->SetLineColor(kBlue);
    h->Fit("f1","R");
    h->DrawCopy("F HIST E1");
    h->Draw("p E1 same");
    f1->Draw("same");

    TLegend * l = new TLegend(0.1,0.45,0.5,0.7);
    l->SetBorderSize(0);
    l->SetFillStyle(0);
    l->AddEntry((TObject*)0,"N(t) = Ae^{k(t-t0)}","");
    l->AddEntry((TObject*)0,Form("Doubling Rate: %1.2f days",TMath::Log(2)/f1->GetParameter(1)),"");
    l->AddEntry((TObject*)0,Form("Cases in 5 days: %3.0f",f1->Eval(confirmedCases->size()-firstCase+0.5 + 5)),"");
    l->AddEntry((TObject*)0,Form("Cases in 15 days: %3.0f",f1->Eval(confirmedCases->size()-firstCase+0.5 + 15)),"");
    l->AddEntry((TObject*)0,Form("Cases in 30 days: %3.0f",f1->Eval(confirmedCases->size()-firstCase+0.5 + 30)),"");
    l->Draw("same");

    TPaveStats *st = (TPaveStats*)h->FindObject("stats");
 
    c1->SaveAs(Form("plots/%s.png",(*locality+"_"+*stateCountry).c_str()));

    h->Write();
    delete st;
    delete f1;
    delete h;
    delete l;
  }
}
