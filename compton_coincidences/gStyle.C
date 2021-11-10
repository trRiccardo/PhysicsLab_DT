void style(TStyle *gStyle){
	gStyle->SetOptStat("");
	gStyle->SetOptFit(0000);
	gStyle->SetTitleBorderSize(0);
	gStyle->SetTitleX(0.5);
	gStyle->SetTitleAlign(23);
	gStyle->SetLineColor(kRed-3);
	gStyle->SetHistLineColor(1);
	gStyle->SetHistFillColor(kAzure-9);//kAzure-4
	gStyle->SetStatY(0.85);
	gStyle->SetStatX(0.48);
	gStyle->SetStatW(0.17);
	gStyle->SetStatH(0.08);
	gStyle->SetPadRightMargin(0.05);
	gStyle->SetPadLeftMargin(0.12);
}
