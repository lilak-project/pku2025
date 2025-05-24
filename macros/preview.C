LKRun* fRun = nullptr;
LKParameterContainer *fPar = nullptr;

TClonesArray* fHitArray = nullptr;
TClonesArray* fHitChannelArray = nullptr;

TH2D* fZT = nullptr;
TH2D* fXT = nullptr;
TH2D* fZTA = nullptr;
TH2D* fXTA = nullptr;
TH1D* fEnergy = nullptr;
TH2Poly* fMM = nullptr;
TH2Poly* fMMA = nullptr;
TString fMappingFileName = "../common/MATE_4000ch_trianglePad_map_merged.txt";
TString fMappingFormat = "id:cobo:asad:aget:chan:x0:z0:x1:z1:x2:z2:x3:z3";
LKDrawingGroup* fTop = nullptr;
LKDrawingGroup* fGroupEvent = nullptr;
bool fOmitDrawAtExe = true;

int fCountEvents = 0;
int fCountEvents2 = 0;
int fNumEvents = 0;
int fRunID = 0;

void Init(int mode=1)
{
    fHitArray = fRun -> KeepBranchA("Hit");
    fHitChannelArray = fRun -> KeepBranchA("HitChannel");

    fZT = new TH2D("mate_tpc_ZT",Form("[Run-%d];z [cm] (beam-axis);Time-bucket",fRunID),46,0,28.336,100,0,500);
    fZTA = new TH2D("mate_tpc_ZTA",Form("[Run-%d];z [cm] (beam-axis);Time-bucket",fRunID),46,0,28.336,100,0,500);
    fXT = new TH2D("mate_tpc_XT",Form("[Run-%d];x [cm];Time-bucket",fRunID),56,-14.646,14.616,100,0,500);
    fXTA = new TH2D("mate_tpc_XTA",Form("[Run-%d];x [cm];Time-bucket",fRunID),56,-14.646,14.616,100,0,500);
    fEnergy = new TH1D("mate_tpc_energy",Form("[Run-%d];energy [ADC]",fRunID),200,0,1500);
    fMM = new TH2Poly("mate_tpc_MM",Form("[Run-%d];z [cm] (beam-axis);x [cm]",fRunID),100,-1,31,100,-15,15);
    fMMA = new TH2Poly("mate_tpc_MMA",Form("[Run-%d];z [cm] (beam-axis);x [cm]",fRunID),1000,-1,31,100,-15,15);
    if (mode==2) fMMA -> SetTitle("");
    if (mode==2) fZT -> SetTitle("");
    if (mode==2) fXT -> SetTitle("");
    double xx[8] = {0};
    double zz[8] = {0};

    LKListFileParser parser;
    parser.SetClass("GETChannelMapData");
    parser.ReadFile(fMappingFileName,fMappingFormat,true);
    auto dataArray = parser.GetDataArray();
    auto n = dataArray -> GetEntries();
    int countc = 0;
    for (auto i=0; i<n; ++i) 
    {
        auto data = (GETChannelMapData*) dataArray -> At(i);
        double xcenter=0, zcenter=0;
        for (auto iv=0; iv<data->n; ++iv) {
            xcenter += data -> x[iv];
            zcenter += data -> z[iv];
        }
        xcenter = xcenter/data->n;
        zcenter = zcenter/data->n;
        //if (xcenter>-0.6 && xcenter<-0.1) {
        //    lk_debug << countc++ << " " << i << " " << zcenter << " " << xcenter << " " << data -> z[2] << endl;
        //}
        //else
        //    continue;
        for (auto iv=0; iv<data->n; ++iv) {
            xx[iv] = data -> x[iv];
            zz[iv] = data -> z[iv];
        }
        fMM  -> AddBin(data->n, zz, xx);
        fMMA -> AddBin(data->n, zz, xx);
    }

    TIter iter(fMM->GetBins());
    while(auto bin = (TH2PolyBin *) iter()) {
        TGraph *gr = (TGraph *) bin->GetPolygon();
        gr->SetLineColor(kGray);
    }

    TIter iter2(fMMA->GetBins());
    while(auto bin = (TH2PolyBin *) iter2()) {
        TGraph *gr = (TGraph *) bin->GetPolygon();
        gr->SetLineColor(kGray);
    }

    for (TH1* hist : {(TH1*)fXT,(TH1*)fZT,(TH1*)fXTA,(TH1*)fZTA,(TH1*)fMM,(TH1*)fMMA}) {
        hist -> SetStats(0);
        hist -> SetMarkerSize(0.5);
    }

    double dx=2000, dy=1500;
    if (mode==1) {
        dx=2000, dy=1500;
    }
    //fPar -> UpdatePar(dx,"MTPreviewTask/cvs_dx");
    //fPar -> UpdatePar(dy,"MTPreviewTask/cvs_dy");
    fPar -> UpdatePar(fOmitDrawAtExe,"MTPreviewTask/omit_draw_at_exe");

    fTop = new LKDrawingGroup("top");

    fGroupEvent = fTop -> CreateGroup("display");
    fGroupEvent -> SetCanvasSize(dx,dy,false);

    if (mode==2) {
        auto file2 = new TFile("data_for_poster/poster_proton_pileup.root","read");
        auto draw_pp = (LKDrawing*) file2 -> Get("draw");
        draw_pp -> SetStyle("ds_poster_7_87_100_z");
        fGroupEvent -> AddDrawing(draw_pp);
    }

    if (mode==1)
    {
        //auto drawEnergy = fGroupEvent -> CreateDrawing("tpc_energy");
        //drawEnergy -> SetCanvasMargin(0.1,0.08,0.08,0.08);
        //drawEnergy -> Add(fEnergy);
    }

    if (mode==2 || mode==0 || mode==1)
    {
        auto drawMMA = fGroupEvent -> CreateDrawing("tpc_accumulate");
        drawMMA -> SetCanvasMargin(0.1,0.13,0.08,0.08);
        drawMMA -> Add(fMMA,"col0");
        if (mode==2) {
            auto box = new TGraph();
            box -> SetPoint(0, 0,-2);
            box -> SetPoint(1,30,-2);
            box -> SetPoint(2,30, 1);
            box -> SetPoint(3, 0, 1);
            box -> SetPoint(4, 0,-2);
            box -> SetFillColor(kCyan-10);
            box -> SetLineColor(kCyan);
            box -> SetLineWidth(2);
            drawMMA -> Add(box,"samefl");
        }
        drawMMA -> Add(fMMA,"col0");
        drawMMA -> Add(fMMA,"l");
        //if (mode==2) {
        //    auto box2 = new TGraph();
        //    box2 -> SetPoint(0,14.5,-14);
        //    box2 -> SetPoint(1,17.5,-14);
        //    box2 -> SetPoint(2,17.5,+14);
        //    box2 -> SetPoint(3,14.5,+14);
        //    box2 -> SetPoint(4,14.5,-14);
        //    //box2 -> SetFillColor(kYellow-10);
        //    box2 -> SetLineColor(kOrange);
        //    box2 -> SetLineWidth(2);
        //    drawMMA -> Add(box2,"samel");
        //}
    }

    if (mode==0 || mode==1)
    {
        auto drawZTA = fGroupEvent -> CreateDrawing("tpc_event_ZTA");
        drawZTA -> SetCanvasMargin(0.1,0.13,0.08,0.08);
        drawZTA -> Add(fZTA,"colz1");
    }

    if (mode==0 || mode==1)
    {
        auto drawXTA = fGroupEvent -> CreateDrawing("tpc_event_XTA");
        drawXTA -> SetCanvasMargin(0.1,0.13,0.08,0.08);
        drawXTA -> Add(fXTA,"colz1");
    }

    if ( mode==1)
    {
        auto drawZT = fGroupEvent -> CreateDrawing("tpc_event_ZT");
        drawZT -> SetCanvasMargin(0.1,0.13,0.08,0.08);
        drawZT -> Add(fZT,"colz1");
        if (mode==2) {
            auto box = new TGraph();
            box -> SetPoint(0, 0,10);
            box -> SetPoint(1,30,10);
            box -> SetPoint(2,30,40);
            box -> SetPoint(3, 0,40);
            box -> SetPoint(4, 0,10);
            box -> SetFillColor(kCyan-10);
            box -> SetLineColor(kCyan);
            box -> SetLineWidth(2);
            drawZT -> Add(box,"samefl");
            auto box2 = new TGraph();
            box2 -> SetPoint(0,14.5,210);
            box2 -> SetPoint(1,17.0,210);
            box2 -> SetPoint(2,17.0,260);
            box2 -> SetPoint(3,14.5,260);
            box2 -> SetPoint(4,14.5,210);
            box2 -> SetFillColor(kYellow-10);
            box2 -> SetLineColor(kOrange);
            box2 -> SetLineWidth(2);
            drawZT -> Add(box2,"samefl");
        }
        drawZT -> Add(fZT,"colz1");
    }

    if (mode==2|| mode==1)
    {
        auto drawXT = fGroupEvent -> CreateDrawing("tpc_event_XT");
        drawXT -> SetCanvasMargin(0.1,0.13,0.08,0.08);
        drawXT -> Add(fXT,"col1");
        if (mode==2) {
            //auto box = new TGraph();
            //box -> SetPoint(0,-3,10);
            //box -> SetPoint(1, 0,10);
            //box -> SetPoint(2, 0,40);
            //box -> SetPoint(3,-3,40);
            //box -> SetPoint(4,-3,10);
            //box -> SetFillColor(kCyan-10);
            //box -> SetLineColor(kCyan);
            //box -> SetLineWidth(2);
            //drawXT -> Add(box,"samefl");
            auto box2 = new TGraph();
            box2 -> SetPoint(0,  14,210);
            box2 -> SetPoint(1, -14,210);
            box2 -> SetPoint(2, -14,260);
            box2 -> SetPoint(3,  14,260);
            box2 -> SetPoint(4,  14,210);
            box2 -> SetFillColor(kYellow-10);
            box2 -> SetLineColor(kOrange);
            box2 -> SetLineWidth(2);
            drawXT -> Add(box2,"samefl");
        }
        drawXT -> Add(fXT,"col1");
    }

    if (mode==2 || mode==1) {
        auto drawMM = fGroupEvent -> CreateDrawing("tpc_event");
        drawMM -> SetCanvasMargin(0.1,0.13,0.08,0.08);
        drawMM -> Add(fMM,"col0");
        if (mode==2) {
            auto box = new TGraph();
            box -> SetPoint(0, 0,-2);
            box -> SetPoint(1,30,-2);
            box -> SetPoint(2,30, 1);
            box -> SetPoint(3, 0, 1);
            box -> SetPoint(4, 0,-2);
            box -> SetFillColor(kCyan-10);
            box -> SetLineColor(kCyan);
            box -> SetLineWidth(2);
            drawMM -> Add(box,"samefl");
            auto box2 = new TGraph();
            box2 -> SetPoint(0,14.5,-14);
            box2 -> SetPoint(1,17.5,-14);
            box2 -> SetPoint(2,17.5,+14);
            box2 -> SetPoint(3,14.5,+14);
            box2 -> SetPoint(4,14.5,-14);
            box2 -> SetFillColor(kYellow-10);
            box2 -> SetLineColor(kOrange);
            box2 -> SetLineWidth(2);
            drawMM -> Add(box2,"samefl");
        }
        drawMM -> Add(fMM,"l");
        drawMM -> Add(fMM,"col0");
    }

}

bool gate_on_event(double x, double z, double t, double e, double p)
{
    //return true;
    if (x<-12) return true;
    //if (x>+12) return true;
    //if (x<1.5&&x>-1.5) return true;
    return false;
}

bool skip_event(double x, double z, double t, double e, double p)
{
    //if (z<10) return true;
    //if (z>20) return true;
    //if (x<-12) return true;
    //if (x>+12) return true;
    //if (x<1.5&&x>-1.5) return true;
    return false;
}

bool gate_on(double x, double z, double t, double e, double p)
{
    //if (z>17&&z<20&&x>2.5&&x<9) return false;

    //if (z>22&&z<25&&x>10) return false;
    //if (z>28) return false;
    //if (x<-10&&z<3) return false;

    //if (p>500) return false;
    //if (x>-1.5&&x<0) return false;
    return true;
}

bool next_event(int eventNo=-1)
{
    if (eventNo>=0) lk_debug << eventNo << endl;

    if (eventNo<0&&fCountEvents>=fNumEvents) {
        return false;
    }

    if (eventNo>=0)
        fRun -> GetEntry(eventNo);
    else
        fRun -> GetEntry(fCountEvents++);
    if (fCountEvents%1000==0) cout << fCountEvents << endl;

    fMM -> Reset("MICES");
    fZT -> Reset("MICES");
    fXT -> Reset("MICES");

    auto numHits = fHitArray -> GetEntries();
    if (eventNo>0) lk_debug << numHits << endl;
    fMM -> SetTitle("");//Form("[Evt-%lld] #hits=%d",fRun->GetCurrentEventID(),numHits));
    fZT -> SetTitle("");//Form("[Evt-%lld] #hits=%d",fRun->GetCurrentEventID(),numHits));

    bool skip = true;
    for (auto iHit=0; iHit<numHits; ++iHit)
    {
        if (eventNo>0) lk_debug << iHit << endl;
        auto hit = (LKHit*) fHitArray -> At(iHit);
        auto x = hit -> GetX();
        auto z = hit -> GetZ();
        auto t = hit -> GetTb();
        auto e = hit -> GetCharge();
        auto p = hit -> GetPedestal();
        //if (skip_event(x,z,t,e,p)) { skip = true; break; }
        if (gate_on_event(x,z,t,e,p)) { skip = false; break; }
    }
    if (eventNo<0&&skip)
        return next_event();

    for (auto iHit=0; iHit<numHits; ++iHit)
    {
        auto hit = (LKHit*) fHitArray -> At(iHit);
        auto x = hit -> GetX();
        auto z = hit -> GetZ();
        auto t = hit -> GetTb();
        auto e = hit -> GetCharge();
        auto p = hit -> GetPedestal();
        if (gate_on(x,z,t,e,p))
        {
            fMM -> Fill(z,x,e);
            fMMA -> Fill(z,x,e);
            fXT -> Fill(x,t,e);
            fZT -> Fill(z,t,e);
            fZTA -> Fill(z,t,e);
            fXTA -> Fill(x,t,e);
            fEnergy -> Fill(e);
        }
    }

    fCountEvents2++;

    if (!fOmitDrawAtExe) {
        lk_debug << fCountEvents << endl;
        fTop -> Draw();
    }

    return true;
}

void preview(int runID=0)
{
    gStyle -> SetPalette(kRainbow);

    fRun = new LKRun();
    fRun -> AddPar("run_3_preview.mac");
    //if (runID>0)
    fRun -> Init();
    fPar = fRun -> GetPar();
    fRun -> Print();
    fNumEvents = fRun -> GetNumEvents();
    fRunID = fRun -> GetRunID();

    auto mode = 2;
    if (mode==0)
    {
        Init(0);
        fOmitDrawAtExe = true;
        while(next_event()) {}
        fTop -> Draw();
        auto cvs = fGroupEvent -> GetCanvas();
        TString name = Form("preview_%d.acc",fRunID);
        cvs -> SaveAs(Form("figures/%s.png",name.Data()));
    }
    else if (mode==1) {
        Init(1);
        fOmitDrawAtExe = false;
    }
    else if (mode==2)
    {
        Init(2);
        fOmitDrawAtExe = true;
        while(next_event()) {}
        next_event(760);
        //next_event(25);
        //fGroupEvent -> SetCanvasDivision(3,1);
        //fGroupEvent -> SetCanvasSize(1200,400);
        fGroupEvent -> SetCanvasDivision(2,2);
        fGroupEvent -> SetCanvasSize(1200,1000);
        fGroupEvent -> SetStyle("ds_poster_7_87_100_z");
        fGroupEvent -> Print();
        fGroupEvent -> Draw();
        auto cvs = fGroupEvent -> GetCanvas();
        TString name = Form("preview_%d.poster",fRunID);
        cvs -> SaveAs(Form("figures/%s.png",name.Data()));
    }

    lk_debug << fCountEvents2 << endl;
}

void save_figure()
{
    auto cvs = fGroupEvent -> GetCanvas();
    TString name = Form("a_%d_%d",fRunID,fCountEvents);
    cvs -> SaveAs(Form("figures/%s.png",name.Data()));
}
