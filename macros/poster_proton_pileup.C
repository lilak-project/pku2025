void poster_proton_pileup()
{
    //gStyle -> SetPalette(kRainBow);
    LKDrawing draw("draw");
    //TString name = "la3";
    TString name = "la78_2";

    auto file = new TFile(TString("data_for_poster/")+name+".root");
    auto pad = (TPad*) file -> Get("LKGETChannelViewer_3");
    auto list = pad -> GetListOfPrimitives();
    TIter next(list);
    TObject *obj;
    while ((obj = (TObject*) next()))
    {
        //cout << obj -> ClassName() << endl;
        TString cname = obj -> ClassName();
        if (cname.Index("TH")==0) {
            auto hist = (TH1*) obj;
            hist -> SetTitle("");
            hist -> GetXaxis() -> SetTitle("Time-bucket");
            hist -> GetYaxis() -> SetTitle("ADC");
            //hist -> GetYaxis() -> SetRangeUser(300,850);
            hist -> GetYaxis() -> SetRangeUser(300,1100);
            hist -> GetYaxis() -> SetNdivisions(508);
            hist -> GetXaxis() -> SetRangeUser(180,500);
            draw.Add(hist);
        }
        if (cname.Index("TGraph")==0) draw.Add(obj,"plc same l");
    }

    //draw.SetStyle("poster_7_87_1000");
    //draw.SetStyle("ds_poster_7_87_100_z");
    draw.SetStyle("default");
    draw.Draw();
    draw.GetCanvas()->SaveAs(TString("figures/")+name+".png");
    auto file2 = new TFile("data_for_poster/poster_proton_pileup.root","recreate");
    draw.Write();
}
