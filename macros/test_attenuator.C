TGraph*       NewGraph      (TString name="graph", int mst=20, double msz=0.6, int mcl=kBlack, int lst=-1, int lsz=-1, int lcl=-1);
TGraphErrors* NewGraphErrors(TString name="graph", int mst=20, double msz=0.6, int mcl=kBlack, int lst=-1, int lsz=-1, int lcl=-1);
double CircleIntersectionArea(double x1, double y1, double r1, double x2, double y2, double r2);
double Gaussian2D(double x, double y, double xc, double yc, double sigmaX, double sigmaY);
double IntegrateGaussian2D(double xc, double yc, double sigmaX, double sigmaY, double x0, double y0, double r0, int nPoints=100);
double IntegrateGaussian2D_SumGamma(double sigma, double cx, double r0);
double ApproximateGaussianIntegral(double xc, double yc, double sigmaX, double sigmaY, double x0, double y0, double r0);

class Idx {
    public:
        int i;
        double j;
        Idx(int i_, double j_) : i(i_),j(j_) {}
        ~Idx() {}
};

void test_attenuator()
{
    bool put1dTogether = true;
    bool chooseHoleSize = true;
    //vector<Idx> indices = {Idx(2,0.2),Idx(3,0.14),Idx(53,0.18)};
    //vector<Idx> indices = {Idx(2,0.12),Idx(3,0.12),Idx(53,0.12)};
    vector<Idx> indices = {Idx(3,0.12),Idx(53,0.12)};

    auto top = new LKDrawingGroup(Form("attenuator%s",chooseHoleSize?"_x":""));

    double wFull = 40; // width of active tantalium aread
    double xMid = 0.5*wFull;
    int beam_type = 1;
    int ndivx = 200;
    int ndivy = 200;
    double sigma_factor = 3.;

    int beam_types[] = {1,0};

    //int attn_idxs[] = {2,3,4,53};
    //int attn_idxs[] = {53};
    int attn_idxs[] = {3};
    //int attn_idxs[] = {2};
    //int attn_idxs[] = {2};
    //int attn_idxs[] = {2,53,3};

    //vector<double> hole_diameter_array = {0.14,0.16,0.18,0.2,0.22,0.24};
    vector<double> hole_diameter_array = {0.12};
    //vector<double> beam_radius_array = {5,10,15};
    vector<double> beam_radius_array = {33.37/3};

    int countAttn = 0;
    for (auto attn_idx : attn_idxs)
    {
        countAttn++;
        double wActive = 36; // width of active tantalium aread
        double hActive = 36; // height of active tantalium aread
        double rActive = 0.5*36; // height of active tantalium aread
        double dSampleCount = 0.1;
        double attnResolutionMax = 0.03;
        int rmdr = 1;

        //vector<double> beam_radius_array = {10};
        double attn_factor = 0;
        int attn_constant = 1;
        int attn_exponent = 0;
        if (attn_idx==1) { attn_exponent = 1; attn_factor = 1.E-1; }
        else if (attn_idx==2) { attn_exponent = 2; attn_factor = 1.E-2; }
        else if (attn_idx==3) {
            attn_exponent = 3;
            attn_factor = 1.E-3;
            dSampleCount = 0.2;
        }
        else if (attn_idx==4)  { attn_exponent = 4; attn_factor = 1.E-4; dSampleCount = 0.5; rmdr = 0; }
        else if (attn_idx==53) { attn_constant = 5; attn_exponent = 3; attn_factor = attn_constant*1.E-3; dSampleCount = 0.2; }
        else if (attn_idx==43) { attn_constant = 4; attn_exponent = 3; attn_factor = attn_constant*1.E-3; dSampleCount = 0.2; }
        else if (attn_idx==54) { attn_constant = 5; attn_exponent = 4; attn_factor = attn_constant*1.E-4; dSampleCount = 0.2; }

        if (beam_type==1) {
            dSampleCount = dSampleCount*0.5;
        }

        //auto drawS = new LKDrawing("drawSummary"); 
        //auto frame = new TH2D("frame",";sample radius (mm); attenuation resolution(?)",100,0.1,0.3,100,0,attnResolutionMax);
        //drawS -> Add(frame);

        auto nSamples = beam_radius_array.size();
        if (chooseHoleSize) {
            hole_diameter_array.clear();
            nSamples = 0;
            for (auto idx : indices) {
                if (idx.i==attn_idx) {
                    nSamples = 1;
                    hole_diameter_array.push_back(idx.j);
                }
            }
        }

        for (auto iSample=0; iSample<nSamples; ++iSample)
        {
            double beam_radius = beam_radius_array[iSample];
            int beam_size_number = int(beam_radius);
            double beam_sigma = beam_radius/sigma_factor;
            auto nTestHoles = hole_diameter_array.size();
            auto dSampleCount0 = dSampleCount;
            if (attn_idx==3) {
                if (beam_radius<10) dSampleCount0 = 0.8;
            }
            dSampleCount0 = 0.1;

            //TString cname = Form("cvs_%dEm%d_%d",attn_constant,attn_exponent,beam_size_number);
            //auto cvs = LKPainter::GetPainter() -> CanvasResize(cname,600*nTestHoles,1800);
            //cvs -> Divide(nTestHoles,3,0.001,0.001);

            auto graphSDV = NewGraphErrors(Form("graphS_%dEm%d_%d",attn_constant,attn_exponent,beam_size_number));
            graphSDV -> SetLineColor(iSample+1);
            //drawS -> Add(graphSDV);

            int countDrawing = 0;
            LKDrawingGroup* group = top -> CreateGroup(Form("group_%d_%d_%d",attn_idx,beam_size_number,int(put1dTogether)));;
            LKDrawingGroup* sub = nullptr;
            int nCaseInSub = 6;
            for (auto iHole=0; iHole<nTestHoles; ++iHole)
            {
                double hole_diameter = hole_diameter_array[iHole];
                double hole_radius = 0.5*hole_diameter; // radius of the hole

                e_info << Form("%d*10^{%d}, hole_radius = %.2f, beam_radius = %.2f, resolution range = %.2f",attn_constant,attn_exponent,hole_radius,beam_radius,dSampleCount0) << endl;

                if (countDrawing%nCaseInSub==0) {
                    sub = group -> CreateGroup(Form("sub_%d_%d_%d_%d",attn_idx,beam_size_number,countDrawing/3,int(put1dTogether)));
                    sub -> SetPadVerticalNumbering();
                    if (chooseHoleSize) {}
                    else if (!put1dTogether) sub -> SetCanvasDivision(nCaseInSub,3);
                    //else sub -> SetCanvasDivision(4,nCaseInSub);
                }
                countDrawing++;

                double x0 = 0.5*(wFull - wActive);
                double y0 = 0.5*(wFull - hActive);
                double hole_size = hole_radius*hole_radius*TMath::Pi();
                double attn_size = wActive*hActive;
                int hole_number = int(100*hole_diameter);

                double number_of_holes = (attn_size*attn_factor)/hole_size;
                double x_dist = sqrt(2*hole_size/sqrt(3)/attn_factor);
                double y_dist = sqrt(3)*x_dist/2;

                auto ny = int(hActive/y_dist)+1;
                auto nx1 = int(wActive/x_dist)+1;
                auto nx = nx1;

                auto draw1 = sub -> CreateDrawing(Form("draw_%d_%d_1_%d",attn_idx,beam_size_number,int(put1dTogether)));
                if (chooseHoleSize)
                    draw1 -> SetCanvasMargin(0.11,0.24,0.11,0.11);

                TString hname = Form("hist_%dEm%d_h%d_s%d",attn_constant,attn_exponent,hole_number,beam_size_number);
                auto hist = new TH2D(hname,";x (mm);y (mm)",100,0,wFull,100,0,wFull);
                hist -> SetStats(0);
                draw1 -> Add(hist);

                vector<TGraph*> graphHoleArray;
                TGraph* graphHolesInOne;
                bool useSimpleHoles = (hole_radius<1);
                if (useSimpleHoles) {
                    graphHolesInOne = NewGraph("graphHolesInOne", 24, (attn_exponent<3?0.3:0.4), (chooseHoleSize?kBlue:kBlack));
                    graphHoleArray.push_back(graphHolesInOne);
                }

                auto graphActive = NewGraph("graphActive");
                graphActive -> SetLineStyle(2);
                for (auto i=0; i<=100; ++i)
                    graphActive -> SetPoint(graphActive->GetN(), rActive*cos(i*TMath::Pi()/50)+xMid, rActive*sin(i*TMath::Pi()/50)+xMid);
                draw1 -> Add(graphActive,"samel");

                for (auto yy : {y0,xMid,wFull-y0}) {
                    auto line = new TLine(0,yy,wFull,yy);
                    line -> SetLineStyle(2);
                    draw1 -> Add(line,"samel");
                }
                for (auto xx : {x0,xMid,wFull-x0}) {
                    auto line = new TLine(xx,0,xx,wFull);
                    line -> SetLineStyle(2);
                    draw1 -> Add(line,"samel");
                }

                double xLow  = x0;
                double xHigh = x0 + (nx-1)*x_dist;
                double yLow  = y0;
                double yHigh = y0 + (ny-1)*y_dist;
                double dxi = (xMid-0.5*(xLow+xHigh));
                double dyi = (xMid-0.5*(yLow+yHigh));
                if (attn_idx==3) dyi = dyi-0.5*y_dist;
                if (attn_idx==53) dyi = dyi-0.5*y_dist;
                double xi = x0+dxi;
                double yi = y0+dyi;
                vector<TVector3> points;
                int countY = 0;
                for (auto iy=0; iy<ny; ++iy)
                {
                    nx = nx1;
                    //if (iy%2==1) nx = nx2;
                    double yh = yi + iy*y_dist;
                    bool firstX = true;
                    TString noteX;
                    double xh1, xh2;
                    int countX = 0;
                    for (auto ix=0; ix<nx; ++ix)
                    {
                        double xh = xi + ix*x_dist;
                        if (iy%2==rmdr) xh = xh + 0.5*x_dist;
                        if (xh>x0+wActive) continue;
                        if ((xMid-xh)*(xMid-xh)+(xMid-yh)*(xMid-yh)>rActive*rActive) continue;
                        if (firstX) {
                            firstX = false;
                            noteX = Form(" %d) x=%.4f, y=%.4f",countY+1,xh,yh);
                            xh1 = xh;
                            xh2 = yh;
                        }
                        if (useSimpleHoles) {
                            graphHolesInOne -> SetPoint(graphHolesInOne->GetN(), xh, yh);
                        }
                        else {
                            auto graphHole = NewGraph("graphHoles_%d_%d",ix,iy);
                            graphHole -> SetLineColor(kBlack);
                            graphHoleArray.push_back(graphHole);
                            for (auto i=0; i<=20; ++i)
                                graphHole -> SetPoint(graphHole->GetN(), hole_radius*cos(i*TMath::Pi()/10)+xh, hole_radius*sin(i*TMath::Pi()/10)+yh);
                        }
                        points.push_back(TVector3(xh,yh,0));
                        countX++;
                    }
                    if (countX>0) {
                        countY++;
                        if (chooseHoleSize) {
                            auto tt = new TLatex(wFull,yh,noteX+", n="+countX);
                            tt -> SetTextAlign(12);
                            tt -> SetTextFont(132);
                            tt -> SetTextSize(0.015);
                            tt -> SetTextColor(kBlue);
                            draw1 -> Add(tt);
                        }
                    }
                }
                for (auto graphHole : graphHoleArray)
                    draw1 -> Add(graphHole,(useSimpleHoles?"samep":"samel"));

                //TString title = Form("%dx10^{-%d}, #phi_{H}=%.2f, n_{H}=%d, dist_{H}=%.2f, r_{S}=%d",attn_constant,attn_exponent,hole_diameter,int(points.size()),x_dist,beam_size_number);
                TString title = Form("%dx10^{-%d}, #phi=%.2f, n=%d, r=%d",attn_constant,attn_exponent,hole_diameter,int(points.size()),beam_size_number);
                if (chooseHoleSize)
                    title = Form("A%02d [%dx10^{-%d}] d=%.2fmm, n=%d, dx=%.4fmm, dy=%.4fmm",attn_idx,attn_constant,attn_exponent,hole_diameter,int(points.size()),x_dist,y_dist);
                hist -> SetTitle(title);

                auto draw1dTogether = sub -> CreateDrawing(Form("draw_%d_%d_1d_%d_%d",attn_idx,beam_size_number,beam_type,int(put1dTogether)),put1dTogether);
                draw1dTogether -> SetCanvasMargin(0.12,0.12,0.12,0.12);
                draw1dTogether -> SetPaveDx(0.5);
                draw1dTogether -> SetPaveLineDy(0.06);
                draw1dTogether -> SetStatsFillStyle(0);
                draw1dTogether -> SetLegendBelowStats();
                auto lgTG = new TLegend();
                //lgTG -> SetHeader(Form("%dx10^{-%d}, #phi=%.2f, r=%d",attn_constant,attn_exponent,hole_diameter,beam_size_number));
                lgTG -> AddEntry((TObject*)0,Form("%dx10^{-%d}, #phi=%.2f, r=%d",attn_constant,attn_exponent,hole_diameter,beam_size_number),"");
                lgTG -> SetFillStyle(0);

                TH1D* histSampleCount1[5];
                for (auto beam_type : beam_types)
                {
                    auto drawAttnRatio = sub -> CreateDrawing(Form("draw_%d_%d_2d_%d_%d",attn_idx,beam_size_number,beam_type,int(put1dTogether)),put1dTogether);
                    drawAttnRatio -> SetCanvasMargin(0.115,0.14,0.125,0.13);

                    auto graphCircle0 = NewGraph("graphCircle0",20,0.6,kBlue);
                    auto graphCircleR = NewGraph("graphCircleR"); graphCircleR -> SetLineColor(kBlue);
                    auto graphCircle3 = NewGraph("graphCircle3"); graphCircle3 -> SetLineColor(kRed);
                    auto graphCircle2 = NewGraph("graphCircle2"); graphCircle2 -> SetLineColor(kRed);
                    auto graphCircle1 = NewGraph("graphCircle1"); graphCircle1 -> SetLineColor(kRed);
                    auto graphSimCenter = NewGraph("graphSimCenter");
                    auto graphSimCenterRange = NewGraph("graphSimCenterRange");
                    //double xs1 = xMid - 0.5*x_dist;
                    //double xs2 = xMid + 0.5*x_dist;
                    //double ys1 = xMid - 0.5*y_dist;
                    //double ys2 = xMid + 0.5*y_dist;
                    double xs1 = xMid - 1.0*x_dist;
                    double xs2 = xMid + 1.0*x_dist;
                    double ys1 = xMid - 1.0*y_dist;
                    double ys2 = xMid + 1.0*y_dist;
                    graphSimCenterRange -> SetPoint(0,xs1,ys1);
                    graphSimCenterRange -> SetPoint(1,xs1,ys2);
                    graphSimCenterRange -> SetPoint(2,xs2,ys2);
                    graphSimCenterRange -> SetPoint(3,xs2,ys1);
                    graphSimCenterRange -> SetPoint(4,xs1,ys1);
                    int count = 0;
                    if (beam_type==0) histSampleCount1[beam_type] = new TH1D(hname+"_bt"+beam_type+"_1d",title+Form(";Attenuation / %dx10^{-%d}",attn_constant,attn_exponent),100,1-2.0*dSampleCount0,1+2.0*dSampleCount0);
                    if (beam_type==1) histSampleCount1[beam_type] = new TH1D(hname+"_bt"+beam_type+"_1d",title+Form(";Attenuation / %dx10^{-%d}",attn_constant,attn_exponent),100,1-0.5*dSampleCount0,1+0.5*dSampleCount0);
                    histSampleCount1[beam_type] -> SetStats(0);
                    if (beam_type==1) histSampleCount1[beam_type] -> SetLineColor(kRed);
                    auto histSampleCount2 = new TH2D(hname+"_bt"+beam_type+"_2d",title+";beam-center-x (mm); beam-center-y (mm)",ndivx,xs1,xs2,ndivy,ys1,ys2);
                    histSampleCount2 -> SetContour(200);
                    histSampleCount2 -> SetStats(0);
                    histSampleCount2 -> GetZaxis() -> SetMaxDigits(6);
                    histSampleCount2 -> GetZaxis() -> SetDecimals(1);
                    //histSampleCount2 -> GetZaxis() -> SetRangeUser(0.9,1.1);
                    for (auto iy=0; iy<ndivy; ++iy)
                    {
                        double y1 = ys1 + iy*(ys2-ys1)/ndivy;
                        for (auto ix=0; ix<ndivx; ++ix)
                        {
                            double x1 = xs1 + ix*(xs2-xs1)/ndivx;

                            double total = 0;
                            for (auto point : points) {
                                double x2 = point.x();
                                double y2 = point.y();
                                if ( ((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)) > beam_radius*beam_radius)
                                    continue;
                                double value = 0;
                                if (beam_type==0) value = CircleIntersectionArea(x1, y1, beam_radius, x2, y2, hole_radius);
                                if (beam_type==1) value = ApproximateGaussianIntegral(x1, y1, beam_sigma, beam_sigma, x2, y2, hole_radius);
                                total += value;
                            }
                            if (beam_type==0) total = total / (TMath::Pi()*beam_radius*beam_radius);
                            if (beam_type==1) total = total / 1;
                            double countRatio = total/attn_factor;
                            histSampleCount2 -> SetBinContent(ix+1,iy+1,countRatio);
                            histSampleCount1[beam_type] -> Fill(countRatio);
                            //cout << setw(3) << count << setw(10) << x1 << setw(10) << y1 << setw(15) << total << endl;

                            if ((ix==0 && iy==0))
                            {
                                graphCircle0 -> SetPoint(graphCircle0->GetN(),x1,y1);
                                for (auto i=0; i<=100; ++i) {
                                    graphCircleR -> SetPoint(graphCircleR->GetN(), beam_radius*3/sigma_factor*cos(i*TMath::Pi()/50)+x1, beam_radius*3/sigma_factor*sin(i*TMath::Pi()/50)+y1);
                                }
                            }
                            else if ((ix==ndivx-1 && iy==ndivy-1))
                            {
                                for (auto i=0; i<=100; ++i) {
                                    graphCircle3 -> SetPoint(graphCircle3->GetN(), beam_radius*3/sigma_factor*cos(i*TMath::Pi()/50)+x1, beam_radius*3/sigma_factor*sin(i*TMath::Pi()/50)+y1);
                                    graphCircle2 -> SetPoint(graphCircle2->GetN(), beam_radius*2/sigma_factor*cos(i*TMath::Pi()/50)+x1, beam_radius*2/sigma_factor*sin(i*TMath::Pi()/50)+y1);
                                    graphCircle1 -> SetPoint(graphCircle1->GetN(), beam_radius*1/sigma_factor*cos(i*TMath::Pi()/50)+x1, beam_radius*1/sigma_factor*sin(i*TMath::Pi()/50)+y1);
                                }
                            }
                            graphSimCenter -> SetPoint(graphSimCenter->GetN(),x1,y1);
                            count++;
                        }
                    }
                    //if (chooseHoleSize) continue;
                    graphSimCenterRange -> SetLineColor(kCyan+1);
                    draw1 -> Add(graphSimCenterRange,"samel");
                    graphSimCenter -> SetMarkerStyle(20);
                    graphSimCenter -> SetMarkerSize(0.6);
                    graphSimCenter -> SetMarkerColor(kRed);
                    if (beam_type==0) draw1 -> Add(graphCircle0,"samel");
                    if (beam_type==0) draw1 -> Add(graphCircleR,"samel");
                    if (beam_type==1) draw1 -> Add(graphCircle3,"samel");
                    if (beam_type==1) draw1 -> Add(graphCircle2,"samel");
                    if (beam_type==1) draw1 -> Add(graphCircle1,"samel");

                    drawAttnRatio -> Add(histSampleCount2,"colz");

                    auto sdv = histSampleCount1[beam_type] -> GetStdDev();
                    graphSDV -> SetPoint(graphSDV->GetN(),hole_diameter,sdv);
                    auto pv = new TPaveText();
                    pv -> SetFillColor(0);
                    pv -> SetFillStyle(0);
                    if (beam_type==0) pv -> AddText("Uniform distribution");
                    if (beam_type==1) pv -> AddText("2d-Gaus distribution");
                    drawAttnRatio -> Add(pv);
                    drawAttnRatio -> SetPaveCorner(1);

                    if (beam_type==0) lgTG -> AddEntry(histSampleCount1[beam_type],Form("Uniform (#mu=%.3f,#sigma=%.4f)",histSampleCount1[beam_type]->GetMean(),histSampleCount1[beam_type]->GetStdDev()),"l");
                    if (beam_type==1) lgTG -> AddEntry(histSampleCount1[beam_type],Form("2d-Gaus (#mu=%.3f,#sigma=%.4f)",histSampleCount1[beam_type]->GetMean(),histSampleCount1[beam_type]->GetStdDev()),"l");

                    auto lg1d = new TLegend();
                    //lg1d -> SetHeader(Form("%dx10^{-%d}, #phi=%.2f, r=%d",attn_constant,attn_exponent,hole_diameter,beam_size_number));
                    lg1d -> AddEntry((TObject*)0,Form("%dx10^{-%d}, #phi=%.2f, r=%d",attn_constant,attn_exponent,hole_diameter,beam_size_number),"");
                    lg1d -> SetFillStyle(0);
                    if (beam_type==0) lg1d -> AddEntry(histSampleCount1[beam_type],Form("Uniform (#mu=%.3f,#sigma=%.4f)",histSampleCount1[beam_type]->GetMean(),histSampleCount1[beam_type]->GetStdDev()),"l");
                    if (beam_type==1) lg1d -> AddEntry(histSampleCount1[beam_type],Form("2d-Gaus (#mu=%.3f,#sigma=%.4f)",histSampleCount1[beam_type]->GetMean(),histSampleCount1[beam_type]->GetStdDev()),"l");

                    auto draw1d = sub -> CreateDrawing(Form("draw_%d_%d_1d_%d_%d",attn_idx,beam_size_number,beam_type,int(put1dTogether)),!put1dTogether);
                    draw1d -> SetCanvasMargin(0.12,0.12,0.12,0.12);
                    draw1d -> SetPaveDx(1);
                    draw1d -> SetPaveLineDy(0.05);
                    draw1d -> SetStatsFillStyle(0);
                    draw1d -> SetLegendBelowStats();
                    draw1d -> Add(histSampleCount1[beam_type]);
                    draw1d -> Add(lg1d);
                }
                if (put1dTogether) {
                    auto max1 = histSampleCount1[0] -> GetMaximum();
                    auto max2 = histSampleCount1[1] -> GetMaximum();
                    if (max2>max1)
                        histSampleCount1[0] -> SetMaximum(max2);
                    draw1dTogether -> Add(histSampleCount1[0]);
                    draw1dTogether -> Add(histSampleCount1[1]);
                }
                draw1dTogether -> Add(lgTG);
            }
        }
    }

    TString option = "wx=1000:wy=630";
    if (chooseHoleSize)
        //option = "wx=550:wy=500";
        option = "wx=640:wy=500";
    else if (!put1dTogether)
        option = "wx=1000:wy=500";
    //top -> Draw(option+":v");
    top -> Draw(option+":v:l");
    top -> Save(1,0,1);
}

double CircleIntersectionArea(double x1, double y1, double r1, double x2, double y2, double r2)
{
    double d = std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    if (d >= r1 + r2) {
        return 0.0;
    }
    if (d <= std::abs(r1 - r2)) {
        double smallerRadius = std::min(r1, r2);
        return M_PI * smallerRadius * smallerRadius;
    }
    double r1Squared = r1 * r1;
    double r2Squared = r2 * r2;
    double angle1 = std::acos((d * d + r1Squared - r2Squared) / (2 * d * r1));
    double angle2 = std::acos((d * d + r2Squared - r1Squared) / (2 * d * r2));
    double triangleArea = 0.5 * std::sqrt((-d + r1 + r2) * (d + r1 - r2) * (d - r1 + r2) * (d + r1 + r2));
    double segmentArea1 = r1Squared * angle1;
    double segmentArea2 = r2Squared * angle2;
    return segmentArea1 + segmentArea2 - triangleArea;
}

double Gaussian2D(double x, double y, double xc, double yc, double sigmaX, double sigmaY) {
    double norm = 1.0 / (2 * TMath::Pi() * sigmaX * sigmaY);
    double dx = (x - xc) / sigmaX;
    double dy = (y - yc) / sigmaY;
    return norm * TMath::Exp(-0.5 * (dx * dx + dy * dy));
}

//double IntegrateGaussian2D(double xc, double yc, double sigmaX, double sigmaY, double x0, double y0, double r0, int nPoints)
//{
//    double integral = 0.0;
//    double dTheta = 2 * TMath::Pi() / nPoints; // Step size in angle
//    double dr = r0 / nPoints; // Step size in radius
//
//    for (int i = 0; i < nPoints; ++i) {
//        double r = i * dr;
//        for (int j = 0; j < nPoints; ++j) {
//            double theta = j * dTheta;
//            double x = x0 + r * TMath::Cos(theta);
//            double y = y0 + r * TMath::Sin(theta);
//
//            // Accumulate the Gaussian value at this point
//            integral += Gaussian2D(x, y, xc, yc, sigmaX, sigmaY) * r * dr * dTheta;
//        }
//    }
//
//    return integral;
//}

double IntegrateGaussian2D(double xc, double yc, double sigmaX, double sigmaY, double x0, double y0, double r0, int nPoints)
{
    double cx = TMath::Sqrt((x0-xc)*(x0-xc) + (y0-yc)*(y0-yc));
    return IntegrateGaussian2D_SumGamma(sigmaX, cx, r0);
}

double IntegrateGaussian2D_SumGamma(double sigma, double cx, double r0)
{
    lk_debug << "cx = " << cx << endl;
    cx = cx/sigma;
    r0 = r0/sigma;
    double valueSum = 0;
    for (int k=0; k<4; ++k)
    {
        double k_factorial = TMath::Factorial(k);
        double value_add = TMath::Power(cx*cx/2,k)/(k_factorial*k_factorial) * TMath::Gamma(k+1,r0*r0/2);
        lk_debug << k << ") " << TMath::Power(cx*cx/2,k) << " / " << (k_factorial*k_factorial) << " * " << TMath::Gamma(k+1,r0*r0/2) << " = " << value_add << endl;
        valueSum += value_add;
    }
    lk_debug << TMath::Exp(-cx*cx/2) << endl;
    lk_debug << TMath::Exp(-cx*cx/2) * valueSum << endl;
    double integral = 1 - TMath::Exp(-cx*cx/2) * valueSum;
    lk_debug << integral << endl;
    return integral;
}

double ApproximateGaussianIntegral(double xc, double yc, double sigmaX, double sigmaY,
                                   double x0, double y0, double r0) {
    double dx = (x0 - xc) / sigmaX;
    double dy = (y0 - yc) / sigmaY;
    double gaussianValue = (1.0 / (2 * TMath::Pi() * sigmaX * sigmaY)) *
                           TMath::Exp(-0.5 * (dx * dx + dy * dy));

    double circleArea = TMath::Pi() * r0 * r0;

    return circleArea * gaussianValue;
}

TGraph *NewGraph(TString name, int mst, double msz, int mcl, int lst, int lsz, int lcl)
{
    auto graph = new TGraph();
    graph -> SetName(name);
    if (mst<=0) mst = 20;
    if (msz<=0) msz = 1;
    if (mcl<0) mcl = kBlack;
    graph -> SetMarkerStyle(mst);
    graph -> SetMarkerSize(msz);
    graph -> SetMarkerColor(mcl);
    if (lst<0) lst = 1;
    if (lsz<0) lsz = 1;
    if (lcl<0) lcl = mcl;
    graph -> SetLineStyle(lst);
    graph -> SetLineWidth(lsz);
    graph -> SetLineColor(lcl);
    graph -> SetFillStyle(0);
    return graph;
}

TGraphErrors *NewGraphErrors(TString name, int mst, double msz, int mcl, int lst, int lsz, int lcl)
{
    auto graph = new TGraphErrors();
    graph -> SetName(name);
    if (mst<=0) mst = 20;
    if (msz<=0) msz = 1;
    if (mcl<0) mcl = kBlack;
    graph -> SetMarkerStyle(mst);
    graph -> SetMarkerSize(msz);
    graph -> SetMarkerColor(mcl);
    if (lst<0) lst = 1;
    if (lsz<0) lsz = 1;
    if (lcl<0) lcl = mcl;
    graph -> SetLineStyle(lst);
    graph -> SetLineWidth(lsz);
    graph -> SetLineColor(lcl);
    graph -> SetFillStyle(0);
    return graph;
}
