import awkward as ak
import numpy as np
from coffea import processor
from coffea.nanoevents import NanoEventsFactory, NanoAODSchema
from hist import Hist
from hist.axis import Regular


class DeltaRHistogrammer(processor.ProcessorABC):

  def __init__(self):
    self._hist = Hist(
      Regular(100, 0.0, 10.0, name="deltaR", label=r"ΔR(lepton, b-jet)")
    )

  def process(self, events):
    hist = self._hist.copy()

    leptons = ak.concatenate([
      events.Muon[(events.Muon.pt > 30) & (abs(events.Muon.eta) < 2.4)],
      events.Electron[(events.Electron.pt > 30)
                      & (abs(events.Electron.eta) < 2.4)]
    ],
                             axis=1)

    bjets = events.Jet[(events.Jet.pt > 30) & (abs(events.Jet.eta) < 2.4) &
                       (events.Jet.btagDeepB > 0.5)]

    pairs = ak.cartesian([leptons, bjets], axis=1)
    lep = pairs["0"]
    jet = pairs["1"]

    dr = jet.delta_r(lep)
    flat_dr = ak.flatten(dr)
    flat_dr = flat_dr[~ak.isnan(flat_dr)]
    hist.fill(deltaR=ak.to_numpy(flat_dr))

    return hist

  def postprocess(self, accumulator):
    return accumulator


if __name__ == "__main__":
  from coffea.nanoevents import NanoEventsFactory, NanoAODSchema

  input_file = "../tea/samples/background_tt.root"
  output_file = "../test_hists_coffea.root"

  events = NanoEventsFactory.from_root({
    input_file: "Events"
  },
                                       schemaclass=NanoAODSchema).events()

  processor_instance = DeltaRHistogrammer()
  hist = processor_instance.process(events)
  print(hist)
