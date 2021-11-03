#include <Riostream.h>
#include <stdlib.h>
#include <TROOT.h>
#include <TSystem.h>
#include "TNtuple.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "math.h"

/*
 *	ROOT macro to retrieve histograms from a ROOT::TNtuple
 *	object, linked to KCl and to the background (K-40 related).
 *	The net spectrum of KCl is then considered, along with
 *	the energy calibration of the histograms.
 *	Finally, the macro computes the integral of the
 *	given histograms to determine the efficiency.
 *
 *	Usage:
 *	root -l
 * 	.L potNetto.C
 *	potNetto()
 *
 *	Updated: 2021-11-03, Riccardo
 */

void gamma_analysis() {

	// acquisition times for scaling
	const double timeKCl = 1801.5531;
	const double timeBack = 1801.3402;
	double t = timeKCl / timeBack;

	// calibration fit results
	const double a = 8.29553;
	const double b = 0.299671;

	// get raw KCl spectrum
	TCanvas* c1 = new TCanvas( "c1" );
	gROOT->SetStyle( "Plain" );
	gStyle->SetTitleBorderSize( 0 );
	gStyle->SetOptStat( "" );
	gStyle->SetOptFit( 1111 );
	gStyle->SetTitleAlign( 23 );

	TFile* f1 = TFile::Open( "KCl.root" );
	TNtuple* nt1 = ( TNtuple* ) ( f1->Get( "nt" ) );
	nt1->Draw( "qlong>>hInt1( 1500, 0, 16000 )" );
	TH1F* h = ( TH1F* ) ( gPad->GetPrimitive("hInt1") );

	// histogram energy calibration
	TH1F* hC = ( TH1F* ) ( h->Clone() );
	hC->GetYaxis()->SetTitle( "Conteggi" );
	hC->GetXaxis()->SetTitle( "Energia (keV)" );
	hC->SetTitle( "Spettro grezzo di KCl" );
	hC->GetYaxis()->SetTitleSize( 0.04 );
	hC->GetYaxis()->SetTitleOffset( 1.25 );
	hC->GetXaxis()->SetTitleSize( 0.04 );
	hC->GetXaxis()->SetTitleOffset( 1.25 );
	hC->SetLineColor( 12 );

	const int nBins = h->GetXaxis()->GetNbins();
	double newBins[nBins + 1];
	for( int i = 0; i <= nBins; i++ ) {
		newBins[i] = a + b * (h->GetBinLowEdge(i+1));
	}
	hC->SetBins( nBins, newBins );

	// clone for net spectrum (prior to the fit)
	TH1F* hN = ( TH1F* ) ( hC->Clone() );

	TF1* g1 = new TF1( "g1", "gaus", 1405, 1540 );
	g1->SetLineStyle( 1 );
	g1->SetLineWidth( 2 );
	g1->SetLineColor( 205 );
	g1->SetParameter( 1, 1460 );

	// fit
	TFitResultPtr r1 = hC->Fit( g1, "rsq" );
	r1->Print( "v" );

	hC->Draw();
	g1->Draw( "same" );

	// K-40 tag
	TLatex* t1 = new TLatex( 1300, 50, "{}^{40}K" );
	t1->SetTextSize( 0.05 );
	t1->Draw( "same" );


	// background
	TCanvas* c2 = new TCanvas( "c2" );
	gROOT->SetStyle( "Plain" );
	gStyle->SetTitleBorderSize( 0 );
	gStyle->SetOptStat( "" );
	gStyle->SetOptFit( 1111 );
	gStyle->SetTitleAlign( 23 );

	TFile* f2 = TFile::Open( "fondo.root" );
	TNtuple* nt2 = ( TNtuple* ) ( f2->Get( "nt" ) );
	nt2->Draw( "qlong>>hInt2( 1500, 0, 16000 )" );
	TH1F* hf = ( TH1F* ) ( gPad->GetPrimitive("hInt2") );

	TH1F* hfC = ( TH1F* ) ( hf->Clone() );	// calibrazione
	hfC->GetXaxis()->SetTitleSize( 0.04 );
	hfC->GetXaxis()->SetTitleOffset( 1.25 );
	hfC->GetYaxis()->SetTitleSize( 0.04 );
	hfC->GetYaxis()->SetTitleOffset( 1.25 );
	hfC->SetLineColor( 12 );
	hfC->GetYaxis()->SetTitle( "Conteggi" );
	hfC->GetXaxis()->SetTitle( "Energia (keV)" );
	hfC->SetTitle( "Fondo ambientale" );

	const int nBinsF = hf->GetXaxis()->GetNbins();
	double newBinsF[nBinsF + 1];
	for( int i = 0; i <= nBinsF; i++ ) {
		newBinsF[i] = a + b * (hf->GetBinLowEdge(i+1));
	}
	hfC->SetBins( nBinsF, newBinsF );

	TH1F* hfN = ( TH1F* ) ( hfC->Clone() );

	TF1* g2 = new TF1( "g2", "gaus", 1425, 1560 );
	g2->SetLineStyle( 1 );
	g2->SetLineWidth( 2 );
	g2->SetLineColor( 205 );

	TFitResultPtr r2 = hfC->Fit( g2, "rsq" );
	r2->Print( "v" );

	hfC->Draw();
	g2->Draw( "same" );

	TLatex* t2 = new TLatex( 1300, 30, "{}^{40}K" );
	t2->SetTextSize( 0.05 );
	t2->Draw( "same" );

	// net spectrum
	TCanvas* c3 = new TCanvas( "c3" );
	gROOT->SetStyle( "Plain" );
	gStyle->SetTitleBorderSize( 0 );
	gStyle->SetOptStat( "" );
	gStyle->SetOptFit( 1111 );
	gStyle->SetTitleAlign( 23 );

	// I should do hN - ( t * hfN ) but I have problems with the fit -> hN - hfN
	hN->Add( hfN, -1. );
	hN->GetYaxis()->SetTitle( "Conteggi" );
	hN->GetXaxis()->SetTitle( "Energia (keV)" );
	hN->SetTitle( "Spettro netto di KCl" );

	TF1* g3 = new TF1( "g3", "gaus", 1370, 1520 );
	g3->SetLineStyle( 1 );
	g3->SetLineWidth( 2 );
	g3->SetLineColor( 205 );
	g3->SetParameters( 13, 1443, 21 );

	TFitResultPtr r3 = hN->Fit( g3, "rsq" );
	r3->Print( "v" );

	hN->Draw();
	g3->Draw( "same" );

	TLatex* t3 = new TLatex( 1300, 12, "{}^{40}K" );
	t3->SetTextSize( 0.05 );
	t3->Draw( "same" );

	/*
	 *
	 *	Integrals of the spectra for efficiency determination.
	 *	In the commented code below, the integral is computed
	 *	by also considering the "halved bins" that the TH1F::Integral()
	 *	function rounds up (or down).
	 *
	 */

	// raw spectrum
	int bMin1 = hC->GetXaxis()->FindBin( 1340. );
	int bMax1 = hC->GetXaxis()->FindBin( 1550. );
	double int1 = hC->Integral( bMin1, bMax1 );
	/*int1 -= hC->GetBinContent( bMin1 ) * ( 1375. - hC->GetXaxis()->GetBinLowEdge( bMin1 ) ) /
		hC->GetXaxis()->GetBinWidth( bMin1 );
	int1 -= hC->GetBinContent( bMax1 ) * ( hC->GetXaxis()->GetBinUpEdge( bMax1 ) - 1520. ) /
		hC->GetXaxis()->GetBinWidth( bMax1 );*/

	// background
	int bMin2 = hfC->GetXaxis()->FindBin( 1340. );
	int bMax2 = hfC->GetXaxis()->FindBin( 1550. );
	double int2 = hfC->Integral( bMin2, bMax2 );
	/*int2 -= hfC->GetBinContent( bMin2 ) * ( 1375. - hfC->GetXaxis()->GetBinLowEdge( bMin2 ) ) /
		hfC->GetXaxis()->GetBinWidth( bMin2 );
	int2 -= hfC->GetBinContent( bMax2 ) * ( hfC->GetXaxis()->GetBinUpEdge( bMax2 ) - 1520. ) /
		hfC->GetXaxis()->GetBinWidth( bMax2 );*/

	// net spectrum
	int bMin3 = hN->GetXaxis()->FindBin( 1340. );
	int bMax3 = hN->GetXaxis()->FindBin( 1550. );
	double int3 = hN->Integral( bMin3, bMax3 );
	/*int3 -= hN->GetBinContent( bMin3 ) * ( 1375. - hN->GetXaxis()->GetBinLowEdge( bMin3 ) ) /
		hN->GetXaxis()->GetBinWidth( bMin3 );
	int3 -= hN->GetBinContent( bMax3 ) * ( hN->GetXaxis()->GetBinUpEdge( bMax3 ) - 1520. ) /
		hN->GetXaxis()->GetBinWidth( bMax3 );*/

	std::cout << "Integrale picco spettro grezzo KCl: " << int1 << std::endl;
	std::cout << "Integrale picco fondo ambientale: " << int2 << std::endl;
	std::cout << "Integrale picco spettro netto: " << int3 << std::endl;
	std::cout << "Fattore frazione tempi di acquisizone: " << t << std::endl;

	return;

}
