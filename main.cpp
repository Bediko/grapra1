#include <Windows.h>
#include <opencv2/core/core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <iostream>
#include <cv.h>
#include <gl\glut.h>

#include <libMesaSR.h>
#include <definesSR.h>



using namespace std;
using namespace cv;

#define ABSTANDSWERTE 0
#define GRAUWERTE 1
#define VERTRAUENSWERTE 2
#define MINX 0
#define MAXX 1
#define MINY 2
#define MAXY 3
#define MINZ 4
#define MAXZ 5


void getImage(IplImage **img) {
	SRCAM *cam = new SRCAM();
	int count = 0, width, height;
	ImgEntry *images_tmp;
	char *data1, *data2;

	//Verbindung mit er Cam herstellen
	//IP der Cam: 192.168.15.21
	if(/*SR_OpenETH(cam, "192.168.15.21")*/SR_OpenFile(cam, "dinos0.tof"))
		cout << "Verbunden ...." << endl;
	else {
		cout << "Verbindung fehlgeschlagen!" << endl;
		return;
	}

	//Setzen des Aufnahmemodus
	SR_SetMode(*cam, 270);
	SR_Acquire(*cam);

	//Zählen der Bilder
	count = SR_GetImageList(*cam, &images_tmp);

	width = SR_GetCols(*cam);
	height = SR_GetRows(*cam);

	//lokale Kopie der Bilder erstellenn
	for(int temp = 0; temp < count; temp++) {

		img[temp] = cvCreateImage(cvSize(width, height), IPL_DEPTH_16U, 1);
		data1 = img[temp]->imageData;
		data2 = (char *)images_tmp[temp].data;

		for(int nRow = 0; nRow < (2*height); nRow++) {
			for(int nCol = 0; nCol < width; nCol++) {
				data1[(nRow * width) + nCol] = data2[(nRow * width) + nCol];
			}
		}
	}

	//Bilder speichern
	cvSaveImage("Abstandswerte.tif", img[ABSTANDSWERTE]);
	cvSaveImage("Grauwerte.tif", img[GRAUWERTE]);
	cvSaveImage("Vertrauenswerte.tif", img[VERTRAUENSWERTE]);
	cvShowImage("Abstandswerte.tif", img[ABSTANDSWERTE]);
	cvShowImage("Grauwerte.tif", img[GRAUWERTE]);
	cvShowImage("Vertrauenswerte.tif", img[VERTRAUENSWERTE]);
	//Verbindung mit der Cam trennen
	if(SR_Close(*cam) == 0) {
		cout << "Verbindung getrennt!" << endl;
	}
	else {
		cout << "Fehler beim Freigeben!" << endl;
	}

	return;
}

void scale(IplImage **img){
	
	double minVal, maxVal;
	double scale, shift;
	IplImage *tmp[3], *bigImg[3];

	for(int nImg = 0; nImg < 3; nImg++) {
		tmp[nImg] = cvCreateImage(cvSize(img[nImg]->width, img[nImg]->height), IPL_DEPTH_8U, 1);
		//Min und Max lokalisieren
		cvMinMaxLoc(img[nImg], &minVal, &maxVal, NULL, NULL, 0);

		//Skalierung und Verschiebung berechnen
		scale = 255 / (maxVal-minVal);
		shift = -minVal * 255 / (maxVal-minVal);

		cout << "minVal: " << minVal << endl;
		cout << "maxVal: " << maxVal << endl;
		cout << "scale: " << scale << endl;
		cout << "shift: " << shift << endl;

		//Skalierung und Verschiebung ausführen
		cvConvertScale(img[nImg], tmp[nImg], scale, shift);

		img[nImg] = tmp[nImg];

		bigImg[nImg] = cvCreateImage(cvSize(img[nImg]->width * 3, img[nImg]->height * 3), IPL_DEPTH_8U, 1);
		cvResize(img[nImg], bigImg[nImg], CV_INTER_LINEAR);

		//Zeiger umbiegen
		img[nImg] = bigImg[nImg];
	}

	//Bilder speichern
	cvSaveImage("Abstandswerte_skaliert_Big.tif", bigImg[ABSTANDSWERTE]);
	cvSaveImage("Grauwerte_skaliert_Big.tif", bigImg[GRAUWERTE]);
	cvSaveImage("Vertrauenswerte_skaliert_Big.tif", bigImg[VERTRAUENSWERTE]);

}

void depthkill(IplImage **img){
	IplImage *product;

	product = cvCreateImage(cvSize(img[GRAUWERTE]->width, img[GRAUWERTE]->height), IPL_DEPTH_8U, 1);

	//Region-Growing ausführen
	cvFloodFill(img[GRAUWERTE], cvPoint(2, 2), cvScalar(0, 0, 0, 0), cvScalar(0, 0, 0, 0), cvScalar(1, 0, 0, 0), NULL, 4, NULL);
	//Nochmal das komplette Bild scannen
	cvThreshold(img[GRAUWERTE], img[GRAUWERTE], 40, 255, THRESH_BINARY);

	//Region-Labeling: nur EINEN Dino anzeigen
	CvMemStorage *mem = cvCreateMemStorage();
	CvSeq *contours = 0;
	int nContours = cvFindContours(img[GRAUWERTE], mem, &contours, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
	for(; contours != 0; contours = contours->h_next ) {
		if(contours->h_next != NULL)
			cvDrawContours(img[GRAUWERTE], contours, CV_RGB(0, 0, 0), CV_RGB(0, 0, 0), -1, CV_FILLED, 8, cvPoint(0, 0));
		else
			cvDrawContours(img[GRAUWERTE], contours, CV_RGB(255, 255, 255), CV_RGB(0, 0, 0), -1, CV_FILLED, 8, cvPoint(0, 0));
	}

	//cvShowImage("Floodfill+Threshold",img[GRAUWERTE]);

	//Maske anwenden, um nur Abstandswerte zu erhalten
	cvAnd(img[GRAUWERTE], img[ABSTANDSWERTE], product);
	//cvShowImage("Fertig", product);
	cvSaveImage("fertig.tif", product);
}

void initialize(void)
{
	glClearColor(0.3,0.3,0.3,0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glRotatef(30, 0, 1, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void checkBounds(int *minmax, int nCol, int nRow, int color) {
	if(nCol < minmax[MINX])
		minmax[MINX] = nCol;
	else if(nCol > minmax[MAXX])
		minmax[MAXX] = nCol;

	if(nRow < minmax[MINY])
		minmax[MINY] = nRow;
	else if(nRow > minmax[MAXY])
		minmax[MAXY] = nRow;

	if(color < minmax[MINZ])
		minmax[MINZ] = color;
	else if(color > minmax[MAXZ])
		minmax[MAXZ] = color;
}

GLuint dinoliste(IplImage *img,int *minmax)
{
	GLuint liste = glGenLists(1);
	int width = img->width;
	int height = img->height;
	int color;
	bool firstTime = true;
	glNewList(liste, GL_COMPILE_AND_EXECUTE);
	for( int row=0;row<height;row++)
	{
		for(int col=0;col<width;col++)
		{
			color=img->imageData[(row * width) + col];
			if(color>0 && (img->imageData[(row * width) + col+1] > 0) && (img->imageData[((row+1) * width) + col+1] > 0) && (img->imageData[((row+1) * width) + col] > 0))
			{
				if(firstTime) {
					minmax[MINX] = col;
					minmax[MAXX] = col;
					minmax[MINY] = row;
					minmax[MAXY] = row;
					minmax[MINZ] = img->imageData[(row * width) + col];
					minmax[MAXZ] = img->imageData[(row * width) + col];
					firstTime = false;
				}
				glBegin(GL_TRIANGLES);
				glColor3f(1, 0, 0);
				glVertex3i(col, row, -color);
				checkBounds(minmax, col, row, color);
				glVertex3i(col+1, row, -img->imageData[(row * width) + col+1]);
				checkBounds(minmax, col+1, row, img->imageData[(row * width) + col+1]);
				glVertex3i(col+1, row+1, -img->imageData[((row+1) * width) + col+1]);
				checkBounds(minmax, col+1, row+1, img->imageData[((row+1) * width) + col+1]);

				glVertex3i(col, row, -color);
				glVertex3i(col+1, row+1, -img->imageData[((row+1) * width) + col+1]);
				glVertex3i(col, row+1, -img->imageData[((row+1) * width) + col]);
				checkBounds(minmax, col, row+1, img->imageData[((row+1) * width) + col]);
				glEnd();
			}
		}
	}
		glEndList();

	return liste;
}


void showDino(void)
{
	int minmax[6];
	glClear(GL_COLOR_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	GLuint liste = dinoliste(cvLoadImage("fertig.tif", 0),minmax);
	glScalef(1, -1, 1);
	glTranslatef(-minmax[MINX], -minmax[MINY], minmax[MINZ]);

	minmax[MAXX] -= minmax[MINX];
	minmax[MINX] = 0;
	minmax[MAXY] -= minmax[MINY];
	minmax[MINY] = 0;
	minmax[MAXZ] -= minmax[MINZ];
	minmax[MINZ] = 0;

	int xscale = (minmax[MAXX]-minmax[MINX])/2;
	int yscale = (minmax[MAXY]-minmax[MINY])/2;
	int zscale = (minmax[MAXZ]-minmax[MINZ])/2;

	glMatrixMode(GL_PROJECTION);
	glOrtho(-(xscale + 10), xscale + 10, -(yscale + 10), yscale + 10, -(zscale + 10), zscale + 10);
	//glOrtho(-90, 90, -100, 100, -25, 25);
	glMatrixMode(GL_MODELVIEW);

	glTranslatef(-xscale, -yscale, zscale);
	
	glCallList(liste);
	

	glFlush();
}


int main()
{
    IplImage *img[3];
    getImage(img);
	scale(img);
	depthkill(img);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(20, 20);
	glutCreateWindow("Dino");
	initialize();
	
	glutDisplayFunc(showDino);

	glutMainLoop();


    cout << "Hello world!" << endl;
	//cvWaitKey(0);
    return 0;
}
