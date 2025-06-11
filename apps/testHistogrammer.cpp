#include "ConfigManager.hpp"
#include "CutFlowManager.hpp"
#include "EventReader.hpp"
#include "EventWriter.hpp"
#include "ExtensionsHelpers.hpp"
#include "HistogramsHandler.hpp"
#include "Profiler.hpp"
#include "HistogramsFiller.hpp"
#include "ArgsManager.hpp"

using namespace std;

int main(int argc, char **argv) {
  
  auto args = make_unique<ArgsManager>(argc, argv);
  if (!args->GetString("config").has_value()) {
    fatal() << "No config file provided" << endl;
    exit(1);
  }

  ConfigManager::Initialize(args->GetString("config").value());
  auto &config = ConfigManager::GetInstance();
  
  auto eventReader = make_shared<EventReader>();
  auto histogramsHandler = make_shared<HistogramsHandler>();
  
  float etaLepton, ptLepton, phiLepton, massLepton;
  float etaJet, ptJet, phiJet, massJet;

  TLorentzVector leptonVector, jetVector;

  for (int iEvent = 0; iEvent < eventReader->GetNevents(); iEvent++) {
    auto event = eventReader->GetEvent(iEvent);

    auto goodLeptons = event->GetCollection("GoodLeptons");
    auto goodBtaggedJets = event->GetCollection("GoodBtaggedJets");

    auto &leptons = *goodLeptons;
    auto &jets = *goodBtaggedJets;

    for (auto lepton : leptons) {
      etaLepton = lepton->Get("eta");
      ptLepton = lepton->Get("pt");
      phiLepton = lepton->Get("phi");
      massLepton = lepton->Get("mass");
      leptonVector.SetPtEtaPhiM(ptLepton, etaLepton, phiLepton, massLepton);

      for(auto jet : jets) {
        etaJet = jet->Get("eta");
        ptJet = jet->Get("pt");
        phiJet = jet->Get("phi");
        massJet = jet->Get("mass");
        jetVector.SetPtEtaPhiM(ptJet, etaJet, phiJet, massJet);

        histogramsHandler->Fill("deltaR", leptonVector.DeltaR(jetVector));
      }
    }
  }

  histogramsHandler->SaveHistograms();

  return 0;
}