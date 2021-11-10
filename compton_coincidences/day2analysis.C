#include <iostream>
#include <string>
#include <vector>

#include "gStyle.C"
#include "utils.C"

/*
 *	Calibrate tagger (channel 0), scatterer (channel 1) and
 *	detector (channel 2) histograms. Select the 511 keV photon
 *	on the tagger, the electron on the scatterer and the scattered
 *	photon on the detector and verify the energy balance.
 *
 *	Usage: manually change the path in line 32 and load on ROOT.
 *
 *	Updated: 2021-11-09, Riccardo
 */

void day2analysis() {

	// set style
	style(gStyle);

	// get input files
	int n = 5;
	std::string files[5] = { "s-t-d_detector_0.root", "s-t-d_detector_20.root", 
							 "s-t-d_detector_40.root", "s-t-d_detector_60.root",
							 "s-t-d_detector_90.root" };
	
 	// write files path here
	for( auto &i: files ) 
		i = "data/" + i;

	// acquisition angles
	int angles[5] = { 0, 20, 40, 60, 90 };

	/*
	 *	Calibrate and plot raw histograms.
	 */

	// histogram array
	TH1D* h[n*3];

	TCanvas* cRaw = new TCanvas( "cRaw" );
	cRaw->Divide(5, 3);

	for( int i = 0; i < n * 3; i++ ) {

		// move to i+1-th canvas
		cRaw->cd(i+1) ;

		if( i < 5 ) {	// get tagger
			h[i] = getHistoForChannelFromTree( files[i].c_str(), 0, 700, 0, 25000 );
			h[i]->SetTitle( Form("Tagger spectrum at angle %i", angles[i]) );
			CalibrateHisto( h[i], 0.0607441, -7.28012 );
		}
		else if( i < 10 ) {	// get scatterer
			h[i] = getHistoForChannelFromTree( files[i-5].c_str(), 1, 700, 0, 25000 );
			h[i]->SetTitle( Form("Scatterer spectrum at angle %i", angles[i-5]) );
			CalibrateHisto( h[i], 0.0558585, -7.23693 );
		}
		else  {	// get detector
			h[i] = getHistoForChannelFromTree( files[i-10].c_str(), 2, 700, 0, 25000 );
			h[i]->SetTitle( Form("Detector spectrum at angle %i", angles[i-10]) );
			CalibrateHisto( h[i], 0.0597647, -7.01858 );
		}

		// get keV per count and set label
		float wBin = h[i]->GetXaxis()->GetBinWidth(0);
		h[i]->GetYaxis()->SetTitle(Form("Counts / %f keV", wBin));

		// draw i-th histogram 
		h[i]->Draw();

	}

	// save output file
	cRaw->SaveAs("rawHistograms.pdf");

	/*
	 *	Select peaks of interest.
	 */

	// histogram array
	TH1D* hSel[n*3];

	// timestamps arrays
	std::vector<double> timeTag[5];
	std::vector<double> timeScat[5];

	TCanvas* cSel = new TCanvas( "cSel" );
	cSel->Divide(5, 3);

	for( int i = 0; i < n * 3; i++ ) {

		// move to i+1-th canvas
		cSel->cd(i+1) ;

		if( i < 5 ) {	// get tagger
			hSel[i] = new TH1D(Form("hT %i", i), Form("Selected tagger spectrum at angle %i", angles[i]),
							 700, 0, 1500);
			timeTag[i] = selectTagger( files[i].c_str(), hSel[i], 0.0607441, -7.28012 );
		}
		else if( i < 10 ) {	// get scatterer
			hSel[i] = new TH1D(Form("hT %i", i), Form("Selected scatterer spectrum at angle %i", angles[i-5]),
							   700, 0, 1500);
			timeScat[i-5] = selectScatterer( files[i-5].c_str(), angles[i-5], hSel[i], 0.0558585, -7.23693 );
		}
		else  {	// get detector
			hSel[i] = new TH1D(Form("hT %i", i), Form("Selected detector spectrum at angle %i", angles[i-5]),
							   700, 0, 1500);
			selectDetector( files[i-10].c_str(), hSel[i], 0.0597647, -7.01858, timeTag[i-10], timeScat[i-10] );
		}

		// get keV per count and set label
		float wBin = hSel[i]->GetXaxis()->GetBinWidth(0);
		hSel[i]->GetYaxis()->SetTitle(Form("Counts / %f keV", wBin));

		// draw i-th histogram 
		hSel[i]->Draw();

	}

	// save output file
	cSel->SaveAs("selectedHistograms.pdf");

	/*
	 *	Multi-dimensional histograms and fits.
	 */
	
	// histogram array
	TH2F* h2d[5];
	TF1* f[5];

	// define canvas
	TCanvas* c2d = new TCanvas( "c2d" );
	c2d->Divide(2, 5);

	// keep track of canvas changes
	int counter = 1;

	// main loop
	for( int i = 0; i < 5; i++ ) {

		// move to i+1-th canvas
		c2d->cd(counter);

		// create histogram
		h2d[i] = createHisto2d( files[i].c_str(), angles[i], 0.0558585, -7.23693, 
								0.0597647, -7.01858, timeTag[i], timeScat[i] );

		// fit the histogram
		f[i] = new TF1( Form("f %i", i+1), "[0] + [1] * x", 0, 700 );
		f[i]->SetParameters(511., -1.);
		f[i]->SetLineColor(kRed);
		h2d[i]->Fit(f[i]);

		// draw the histogram
		h2d[i]->Draw();
		f[i]->Draw("SAME");

		// draw with another option
		c2d->cd(counter+1);
		h2d[i]->Draw("LEGO2Z");

		counter += 2;

	}

	c2d->SaveAs( "multiHistograms.pdf" );

	/*
	 *	Analysis of the previous results.
	 */

	// TO DO: compatibilità intercetta - 511 keV.

	return;
	
} 