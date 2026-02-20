TTree *fTree = nullptr;
TCanvas *fCvs = nullptr;
LKParameter *fColors = nullptr;
LKChannelAnalyzer *fChAna = nullptr;
TClonesArray *fRawDataArray = nullptr;
bool drawFPN = true;
int fNumRawData = 0;
int fCountRawData = 0;
int fNumEvents = 0;
int fCountEvents = 0;
int fNumChannelsTogether = 20;

bool nextEvent()
{
    if (fCountEvents>=fNumEvents)
        return false;
    cout << "Event " << fCountEvents << endl;
    fTree -> GetEntry(fCountEvents++);
    fNumRawData = fRawDataArray -> GetEntries();
    fCountRawData = 0;
    return true;
}

bool next()
{
    if (fCountRawData>=fNumRawData) {
        if (nextEvent()==false)
            return false;
    }
    fCvs -> Clear("D");
    int iColor = 0;
    int iChannel = 0;
    int jChannel = 1;
    bool foundFPN11 = false;
    bool foundFPN22 = false;
    bool foundFPN45 = false;
    bool foundFPN56 = false;
    bool foundSignal = false;
    int preChan = 1000;
    auto histArray = new TObjArray();
    auto histFPNArray = new TObjArray();

    auto DumpHist = [histArray,histFPNArray,&jChannel,&iColor,&foundSignal]()
    {
        TH1D *hist;
        TIter nextHist(histArray);
        TIter nextHistFPN(histFPNArray);
        if (foundSignal)
        {
            if (drawFPN)
                while ((hist = (TH1D*) nextHistFPN())) {
                    fCvs -> cd(jChannel++);
                    hist -> SetLineColor(fColors -> GetColor(iColor));
                    hist -> Draw();
                }
            while ((hist = (TH1D*) nextHist()))
            {
                //fCvs -> cd(jChannel++);
                hist -> SetLineColor(fColors -> GetColor(iColor));
                auto histFPN = ((TH1D*)histFPNArray->At(0));
                hist -> Add(histFPN,-1);
                if (1) {
                    fCvs -> cd(jChannel++);
                    hist -> Draw();
                }
                else {
                    fChAna -> Analyze(hist);
                    auto draw = fChAna -> GetDrawing();
                    draw -> SetCanvas(fCvs -> cd(jChannel++));
                    draw -> Draw();
                }
            }
        }
        else 
        {
            if (drawFPN)
                while ((hist = (TH1D*) nextHistFPN())) {
                    fCvs -> cd(jChannel++);
                    hist -> SetLineColor(kGray);//fColors -> GetColor(iColor));
                    hist -> Draw();
                }
        }
        histArray -> Clear();
        histFPNArray -> Clear();
        foundSignal = 0;
        iColor++;
        if (iColor>=fColors->GetN())
            iColor = 0;
    };

    while (iChannel<fNumChannelsTogether)
    {
        cout << "[" << fCountEvents << "] Channel " << fCountRawData << " / " << fNumRawData << endl;
        auto rawData = (GETChannel*) fRawDataArray -> At(fCountRawData++);
        auto chan = rawData -> GetChan();
        if (chan<preChan)
            DumpHist();
        preChan = chan;
        auto hist = rawData -> GetHist();
        if (chan==11 ||chan==22 ||chan==45 ||chan==56)
        {
            if      (chan==11) foundFPN11 = 1;
            else if (chan==22) foundFPN22 = 1;
            else if (chan==45) foundFPN45 = 1;
            else if (chan==56) foundFPN56 = 1;
            histFPNArray -> Add(hist);
            if (drawFPN)
                iChannel++;
        }
        else {
            foundSignal = 1;
            histArray -> Add(hist);
            iChannel++;
        }
        if (fCountRawData>=fNumRawData)
            break;
    }
    DumpHist();
    return true;
}

void test_raw()
{
    //auto file = new TFile("data_conv/pku2025_0205.conv.root");
    auto file = new TFile("data_conv/pku2025_0226.conv.root");
    //auto file = new TFile("data_conv/pku2025_0221.conv.root");
    fTree = (TTree*) file -> Get("event");
    fTree -> SetBranchAddress("RawData",&fRawDataArray);
    fNumEvents = fTree -> GetEntries();
    fColors = new LKParameter("color_signals.mac");
    fChAna = new LKChannelAnalyzer();

    fCvs = LKPainter::GetPainter() -> CanvasResize("test_raw",1300,1000);
    LKPainter::DividePad(fCvs,fNumChannelsTogether);

    next();
}
