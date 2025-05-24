LKRun* fRun = nullptr;
LKParameterContainer *fPar = nullptr;

TClonesArray* fHitArray = nullptr;
TClonesArray* fHitChannelArray = nullptr;

TH2D* fXT = nullptr;
TH2Poly* fMM = nullptr;
TH2Poly* fMMA = nullptr;

TString fMappingFileName = "../common/MATE_4000ch_trianglePad_map_merged.txt";
TString fMappingFormat = "id:cobo:asad:aget:chan:x0:z0:x1:z1:x2:z2:x3:z3";
LKDrawingGroup* fTop = nullptr;
LKDrawingGroup* fGroupEvent = nullptr;

int fCountEvents = 0;
int fCountPassEvents = 0;
int fNumEvents = 0;
int fRunID = 0;

void Init()
{
    fHitArray = fRun -> GetBranchA("Hit");
    fHitChannelArray = fRun -> GetBranchA("HitChannel");
    fXT  = new TH2D   ("mate_tpc_XT" ,";x [cm];Time-bucket",  56,-14.646,14.616,  100,0,500);
    fMM  = new TH2Poly("mate_tpc_MM" ,";z [cm] (beam-axis);x [cm]", 100,-1,31,   100,-15,15);
    fMMA = new TH2Poly("mate_tpc_MMA",";z [cm] (beam-axis);x [cm]", 1000,-1,31,  100,-15,15);

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
        for (auto iv=0; iv<data->n; ++iv) {
            xx[iv] = data -> x[iv];
            zz[iv] = data -> z[iv];
        }
        fMM  -> AddBin(data->n, zz, xx);
        fMMA -> AddBin(data->n, zz, xx);
    }

    TIter iter(fMM->GetBins());
    TIter iter2(fMMA->GetBins());
    while(auto bin = (TH2PolyBin *) iter()) { ((TGraph *) bin->GetPolygon()) -> SetLineColor(kGray); }
    while(auto bin = (TH2PolyBin *) iter2()) { ((TGraph *) bin->GetPolygon()) -> SetLineColor(kGray); }
    for (TH1* hist : {(TH1*)fXT,(TH1*)fMM,(TH1*)fMMA}) { hist -> SetStats(0); }

    fTop = new LKDrawingGroup("top");
    fGroupEvent = fTop -> CreateGroup("display");
    auto drawMMA = fGroupEvent -> CreateDrawing("tpc_accumulate");
    drawMMA -> SetCanvasMargin(0.1,0.13,0.08,0.08);
    drawMMA -> Add(fMMA,"col0");
    {
        auto box2 = new TGraph();
        box2 -> SetPoint(0, 11.5095,  -14.0561);
        box2 -> SetPoint(1, 5.55176,  11.6);
        box2 -> SetPoint(2, 11.3193,  14.0435);
        box2 -> SetPoint(3, 20.7629,  14.0435);
        //box2 -> SetPoint(4, 26.4671,  11.4134);
        //box2 -> SetPoint(5, 19.4953,  -14.0561);
        box2 -> SetPoint(4, 25.7,  11.6);
        box2 -> SetPoint(5, 19.3,  -14.0561);
        box2 -> SetPoint(6, 11.5095,  -14.0561);
        box2 -> SetFillColor(kYellow-10);
        box2 -> SetLineColor(kOrange);
        box2 -> SetLineWidth(2);
        drawMMA -> Add(box2,"samefl");
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

    auto drawMM = fGroupEvent -> CreateDrawing("tpc_event");
    drawMM -> SetCanvasMargin(0.1,0.13,0.08,0.08);
    drawMM -> Add(fMM,"col0");
    {
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
    }
    drawMM -> Add(fMM,"l");
    drawMM -> Add(fMM,"col0");

    auto file2 = new TFile("data_for_poster/poster_proton_pileup.root","read");
    auto draw_pp = (LKDrawing*) file2 -> Get("draw");
    draw_pp -> SetStyle("ds_poster_7_87_100_z");
    fGroupEvent -> AddDrawing(draw_pp);

    auto drawXT = fGroupEvent -> CreateDrawing("tpc_event_XT");
    drawXT -> SetCanvasMargin(0.1,0.13,0.08,0.08);
    drawXT -> Add(fXT,"col1");
    {
        auto box = new TGraph();
        box -> SetPoint(0,-2,10);
        box -> SetPoint(1, 0.5,10);
        box -> SetPoint(2, 0.5,480);
        box -> SetPoint(3,-2,480);
        box -> SetPoint(4,-2,10);
        box -> SetFillColor(kCyan-10);
        box -> SetLineColor(kCyan);
        box -> SetLineWidth(2);
        drawXT -> Add(box,"samefl");
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

bool gate_on_event(double x, double z, double t, double e, double p)
{
    //return true;
    if (x<-12) return true;
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
    if (eventNo>=0) cout << "event: " << eventNo << endl;

    if (eventNo<0&&fCountEvents>=fNumEvents) {
        return false;
    }

    if (eventNo>=0)
        fRun -> GetEntry(eventNo);
    else
        fRun -> GetEntry(fCountEvents++);
    if (fCountEvents%1000==0) cout << fCountEvents << endl;

    fMM -> Reset("MICES");
    fXT -> Reset("MICES");

    auto numHits = fHitArray -> GetEntries();
    if (eventNo>0) cout << "number of hits: " << numHits << endl;
    fMM -> SetTitle("");

    bool skip = true;
    for (auto iHit=0; iHit<numHits; ++iHit)
    {
        auto hit = (LKHit*) fHitArray -> At(iHit);
        auto x = hit -> GetX();
        auto z = hit -> GetZ();
        auto t = hit -> GetTb();
        auto e = hit -> GetCharge();
        auto p = hit -> GetPedestal();
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
        }
    }

    //auto numChannels = fHitChannelArray -> GetEntries();
    //for (auto iChannel=0; iChannel<numChannels; ++iChannel)
    //{
    //    auto channel = (GETChannel*) fHitChannelArray -> At(iChannel);
    //    channel -> Print();
    //}

    fCountPassEvents++;

    return true;
}

void poster_tpc_events()
{
    gStyle -> SetPalette(kRainbow);

    fRun = new LKRun();
    fRun -> AddPar("run_3_preview.mac");
    fRun -> Init();
    fPar = fRun -> GetPar();
    fNumEvents = fRun -> GetNumEvents();
    fRunID = fRun -> GetRunID();

    Init();
    while(next_event()) {}
    next_event(760);
    fGroupEvent -> SetCanvasDivision(2,2);
    fGroupEvent -> SetCanvasSize(1200,1000);
    //fGroupEvent -> SetCanvasSize(1200*2,1000*2,false);
    fGroupEvent -> SetStyle("ds_poster_7_87_100_z");
    fGroupEvent -> Draw();
    fGroupEvent -> Save();
    auto cvs = fGroupEvent -> GetCanvas();
    TString name = Form("preview_%d.poster",fRunID);
    cvs -> SaveAs(Form("figures/%s.png",name.Data()));

    e_info << "good: " << fCountPassEvents << endl;
}
