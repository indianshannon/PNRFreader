// PNRFreader_from_scratch.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

// PNRFLoadExample.cpp: Defines the entry point
// for the console application.
#include "stdafx.h" // standard stuff
#include<stdio.h>
#include<conio.h>
#include<iostream> // basic console I/O
#include<atlcomcli.h>
using namespace std;

// #import "percRecordingInterface.olb" no_namespace
#import "libid:8098371E-98AD-0070-BEF3-21B9A51D6B3E" no_namespace
// #import "percPNRFLoader.dll" no_namespace
#import "libid:8098371E-98AD-0062-BEF3-21B9A51D6B3E" no_namespace

char cAnyKey; // key input
double dStart, dEnd; // start and stop time
BSTR myUnits; // units
VARIANT myCompany; // company name

void ProcessFile(char *pnrf_fname)
{
	FILE *fptr;
	char fname[100];

	IRecordingLoaderPtr itfLoader;
	itfLoader.CreateInstance(__uuidof (PNRFLoader));
	// Enter the name of the pnrf recording here
	//IRecordingPtr itfRecording = itfLoader->LoadRecording("C:\\Users\\hvangala\\Downloads\\VT609.pnrf");
	IRecordingPtr itfRecording = itfLoader->LoadRecording(pnrf_fname);
	printf("Reading file: %s\n", pnrf_fname);

	for (int i = 3; i <= 3; i++) //Note that i=0, i>3 are not valid.
	{
		int last = strlen(pnrf_fname)-1;
		if (!strcmp(&pnrf_fname[last - 4], ".pnrf") || !strcmp(&pnrf_fname[last - 4], ".PNRF"))
			pnrf_fname[last - 4] = '\0';

		sprintf(fname, "%s-Channel-%d.txt", pnrf_fname, i);
		fptr = fopen(fname, "w");
		if (!fptr)
		{
			printf("ERROR: Unable to open output file (beside source .pnrf file), please check you have permissions to write. \nExiting.");
				exit(1);
		}

		// connect to data source channel i = 1(Volts), 2(Volts), 3(Amperes)  [note that i=0 is not valid]
		IDataSrcPtr MySource = itfRecording->Channels->Item[i]->DataSource[DataSourceSelect_Mixed];

		// fetch YUnits
		MySource->get_YUnit(&myUnits);
		printf("\nChannel     : %d \nY-axis Units: %s\n", i, (char*)myUnits);
		fprintf(fptr, "Seconds\t %s\n", (char*)myUnits);

		// fetch start and stop time
		MySource->Sweeps->get_StartTime(&dStart);
		MySource->Sweeps->get_EndTime(&dEnd);
		printf("\n  Start time: %lf s\n  End time  : %lf s", dStart, dEnd);

		CComVariant mydata;
		MySource->Data(dStart, dEnd, &mydata);

		if (mydata.vt == VT_EMPTY)
		{
			printf("\nEmpty dataset found.");
			return;
		}
		else
		{
			IDataSegmentsPtr itfSegments = mydata.punkVal;
			int iSegIndex = 1;
			int iCount = itfSegments->Count;
			long totalsamples = 0, nchars;
			printf("\n  Total Segments found         : %d", iCount);
			printf("\n  Segments finished processing : ");
			// loop through all available segments
			for (iSegIndex = 1; iSegIndex <= iCount; iSegIndex++)
			{
				nchars = printf("%d", iSegIndex); fflush(stdout);

				// pointer inside segment data
				IDataSegmentPtr itfSegment = NULL;
				itfSegments ->get_Item(iSegIndex, &itfSegment);
				int lCnt = itfSegment->NumberOfSamples;
				totalsamples += lCnt;
				
				// variant data array for segment data
				CComVariant varData;
				// fetch data
				itfSegment ->Waveform(DataSourceResultType_Double64, 1, lCnt, 1, &varData);
				//If there is no data, process next segment
				if (varData.vt == VT_EMPTY)
				continue;
				//If it isn't an array, something is wrong here
				if (!(varData.vt & VT_ARRAY))
				continue;
				// Get data out through the use of the safe array
				// and store locally
				SAFEARRAY* satmp = NULL;
				satmp = varData.parray;
				if (satmp->cDims > 1)
				{
					// It's a multi dimensional array
					printf("Too many dimensions.\n");
					continue;
				}
				double *pData;

				SafeArrayAccessData(satmp, (void**)&pData);
				double X0 = itfSegment->StartTime;
				double DeltaX = itfSegment->SampleInterval;
				double X, Y;
				//printf("%lf+i*%g,", X0,DeltaX);
				for (int i = 0; i < (int)satmp ->rgsabound[0].cElements; i++)
				{
					X = X0 + i * DeltaX;
					Y = pData[i];
					fprintf(fptr,"%g\t %g\n", X, Y);
				}
				SafeArrayUnaccessData(satmp);
				fprintf(fptr,"\n"); //Empty line to separate segments

				for (int temp = 0; temp < nchars; temp++)printf("\b"); fflush(stdout);
			}
			printf("\n\n  Total Samples In All Segments = %ld", totalsamples);
			printf("  \n   ** Output file %s written **", fname);
			fclose(fptr);
		}

		printf("\n\n");
	}
}

int main(int argc, char *argv[])
{
	CoInitialize(NULL);
	ProcessFile(argv[1]);
	/*
	cout << endl << "Done. Press any key to quit." << endl;
	_getch();
	*/
	CoUninitialize();
	return 0;
}