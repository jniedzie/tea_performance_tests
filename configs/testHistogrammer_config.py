nEvents = -1
printEveryNevents = 100000

inputFilePath = "../tea/samples/background_tt.root"
histogramsOutputFilePath = "../test_hists.root"

histParams = (
    ("deltaR",      100,  0,      10,     ""),
)

weightsBranchName = "genWeight"
eventsTreeName = "Events"

extraEventCollections = {
    "GoodLeptons": {
        "inputCollections": ("Muon", "Electron"),
        "pt": (30., 9999999.),
        "eta": (-2.4, 2.4),
    },
    "GoodBtaggedJets": {
        "inputCollections": ("Jet", ),
        "pt": (30., 9999999.),
        "eta": (-2.4, 2.4),
        "btagDeepB": (0.5, 9999999.),
    },
}

eventsTreeNames = ["Events",]
specialBranchSizes = {}
branchesToKeep = ["*"]
branchesToRemove = []
