#include <TH1D.h>
#include <TF1.h>
#include <TCanvas.h>

//Complete this program!

void random_distr(){
	TH1D *histo = new TH1D("my_distr", "My distribution" ,1000, 0, 5);
	

	histo->FillRandom("gaus", 10000);

	TCanvas *cv = new TCanvas("cv");

	histo->Draw();

	histo->Fit("gaus", "R", "", 0, 5);

}
