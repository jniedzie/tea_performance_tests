#include <ROOT/RDataFrame.hxx>
#include <TFile.h>
#include <TLorentzVector.h>
#include <iostream>

using namespace ROOT::RDF;
using namespace std;

// Helper function to compute ΔR between all good leptons and all good b-tagged jets
ROOT::VecOps::RVec<float> computeDeltaRs(
    const ROOT::VecOps::RVec<float> &mu_pt, const ROOT::VecOps::RVec<float> &mu_eta,
    const ROOT::VecOps::RVec<float> &mu_phi, const ROOT::VecOps::RVec<float> &mu_mass,
    const ROOT::VecOps::RVec<float> &el_pt, const ROOT::VecOps::RVec<float> &el_eta,
    const ROOT::VecOps::RVec<float> &el_phi, const ROOT::VecOps::RVec<float> &el_mass,
    const ROOT::VecOps::RVec<float> &jet_pt, const ROOT::VecOps::RVec<float> &jet_eta,
    const ROOT::VecOps::RVec<float> &jet_phi, const ROOT::VecOps::RVec<float> &jet_mass,
    const ROOT::VecOps::RVec<float> &jet_btag) {

  ROOT::VecOps::RVec<TLorentzVector> leptons;
  for (size_t i = 0; i < mu_pt.size(); ++i) {
    if (mu_pt[i] > 30. && fabs(mu_eta[i]) < 2.4) {
      TLorentzVector v; v.SetPtEtaPhiM(mu_pt[i], mu_eta[i], mu_phi[i], mu_mass[i]);
      leptons.push_back(v);
    }
  }
  for (size_t i = 0; i < el_pt.size(); ++i) {
    if (el_pt[i] > 30. && fabs(el_eta[i]) < 2.4) {
      TLorentzVector v; v.SetPtEtaPhiM(el_pt[i], el_eta[i], el_phi[i], el_mass[i]);
      leptons.push_back(v);
    }
  }

  ROOT::VecOps::RVec<TLorentzVector> jets;
  for (size_t i = 0; i < jet_pt.size(); ++i) {
    if (jet_pt[i] > 30. && fabs(jet_eta[i]) < 2.4 && jet_btag[i] > 0.5) {
      TLorentzVector v; v.SetPtEtaPhiM(jet_pt[i], jet_eta[i], jet_phi[i], jet_mass[i]);
      jets.push_back(v);
    }
  }

  ROOT::VecOps::RVec<float> deltaRs;
  for (auto &lep : leptons) {
    for (auto &jet : jets) {
      deltaRs.push_back(lep.DeltaR(jet));
    }
  }

  return deltaRs;
}

int rdataframes() {
  string inputFile = "../tea/samples/background_tt.root";
  string treeName = "Events";
  string outputFile = "../test_hists_rdataframes.root";

  ROOT::RDataFrame df(treeName, inputFile);

  auto dfWithDeltaR = df.Define("deltaRs", computeDeltaRs,
    {"Muon_pt", "Muon_eta", "Muon_phi", "Muon_mass",
     "Electron_pt", "Electron_eta", "Electron_phi", "Electron_mass",
     "Jet_pt", "Jet_eta", "Jet_phi", "Jet_mass", "Jet_btagDeepB"});

  auto hist = dfWithDeltaR.Histo1D({"deltaR", "deltaR", 100, 0, 10}, "deltaRs");


  TFile out(outputFile.c_str(), "RECREATE");
  hist->Write();
  out.Close();

  return 0;
}