#include "MTPreviewTask.h"
#include "LKRun.h"
#include "LKListFileParser.h"
#include "GETChannelMapData.h"

MTPreviewTask::MTPreviewTask()
:LKTask("MTPreviewTask", "MTPreviewTask")
{
}

bool MTPreviewTask::Init() 
{
    fHitArray = fRun -> KeepBranchA("Hit");
    fHitChannelArray = fRun -> KeepBranchA("HitChannel");

    fZT = new TH2D("mate_tpc_zy","MATE-TPC ZY;z [cm];t [cm] (beam-axis)",100,-1,30,100,0,500);
    fEnergy = new TH1D("mate_tpc_energy","MATE-TPC Energy;energy [ADC]",200,0,5000);
    fMM = new TH2Poly("mate_tpc_mm","MATE-TPC MM;x [cm];z [cm] (beam-axis)",100,-15,15,100,-1,30);
    fMMA = new TH2Poly("mate_tpc_mm_a","MATE-TPC MM;x [cm];z [cm] (beam-axis)",100,-15,15,100,-1,30);
    double xx[8] = {0};
    double zz[8] = {0};

    LKListFileParser parser;
    parser.SetClass("GETChannelMapData");
    parser.ReadFile(fMappingFileName,fMappingFormat,true);
    auto dataArray = parser.GetDataArray();
    auto n = dataArray -> GetEntries();
    for (auto i=0; i<n; ++i) 
    {
        auto data = (GETChannelMapData*) dataArray -> At(i);
        for (auto iv=0; iv<data->n; ++iv) {
            xx[iv] = data -> x[iv];
            zz[iv] = data -> z[iv];
        }
        auto bin = fMM -> AddBin(data->n, xx, zz);
        fMMA -> AddBin(data->n, xx, zz);
    }

    fZT -> SetStats(0);
    fZT -> SetMarkerSize(0.5);
    fMM -> SetStats(0);
    fMM -> SetMarkerSize(0.5);
    fMMA -> SetStats(0);
    fMMA -> SetMarkerSize(0.5);

    double dx=700, dy=700;
    fPar -> UpdatePar(dx,"MTPreviewTask/cvs_dx");
    fPar -> UpdatePar(dy,"MTPreviewTask/cvs_dy");
    fPar -> UpdatePar(fOmitDrawAtExe,"MTPreviewTask/omit_draw_at_exe");

    fTop = new LKDrawingGroup("top");

    auto groupMM = fTop -> CreateGroup("display");
    groupMM -> SetCanvasSize(dx,dy,false);

    auto drawMM = groupMM -> CreateDrawing("tpc_event");
    drawMM -> SetCanvasMargin(0.08,0.12,0.08,0.08);
    drawMM -> Add(fMM,"colz0");
    drawMM -> Add(fMM,"l");

    auto drawZY = groupMM -> CreateDrawing("tpc_event_zy");
    drawZY -> SetCanvasMargin(0.08,0.12,0.08,0.08);
    drawZY -> Add(fZT,"colz1");

    auto drawEnergy = groupMM -> CreateDrawing("tpc_energy");
    drawEnergy -> Add(fEnergy);

    auto drawMMA = groupMM -> CreateDrawing("tpc_accumulate");
    drawMMA -> SetCanvasMargin(0.08,0.12,0.08,0.08);
    drawMMA -> Add(fMMA,"colz0");
    drawMMA -> Add(fMMA,"l");

    //int numCobos = 4;
    //auto groupCh = fTop -> CreateGroup("channels");
    //for (auto cobo=0; cobo<numCobos; ++cobo)
    //{
    //    auto groupCh = fTop -> CreateGroup("channels");
    //}

    return true;
}

void MTPreviewTask::Exec(Option_t*)
{
    fMM -> Reset("MICES");

    auto numHits = fHitArray -> GetEntries();
    fMM -> SetTitle(Form("[%lld] #hits=%d",fRun->GetCurrentEventID(),numHits));
    lk_info << "Filling " << numHits << " hits " << endl;

    for (auto iHit=0; iHit<numHits; ++iHit)
    {
        auto hit = (LKHit*) fHitArray -> At(iHit);
        auto x = hit -> GetX();
        auto z = hit -> GetZ();
        auto t = hit -> GetTb();
        auto e = hit -> GetCharge();
        if (x>-2&&x<2)
            continue;
        fMM -> Fill(x,z);
        fMMA -> Fill(x,z);
        fZT -> Fill(z,t);
        fEnergy -> Fill(e);
    }

    if (!fOmitDrawAtExe) fTop -> Draw();
}

bool MTPreviewTask::EndOfRun()
{
    fTop -> Draw();
    return true;
}
