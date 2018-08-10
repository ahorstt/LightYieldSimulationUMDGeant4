import FWCore.ParameterSet.Config as cms

process = cms.Process("CopyJob")
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring("/store/mc/SAM/RelValProdTTbar/GEN-SIM-DIGI-RECO/MC_38Y_V13_SAM-v2/0106/8E699A2C-6824-E011-B3D7-0026189438BF.root")
                           )

process.out = cms.OutputModule("PoolOutputModule",
                               fileName = cms.untracked.string('myout.root'),
                               #     fastCloning = cms.untracked.bool(False),
                               outputCommands = cms.untracked.vstring('drop *')
                              )
process.e = cms.EndPath(process.out)
