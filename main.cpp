#include <Windows.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>
#include <libMesaSR.h>



using namespace std;
using namespace cv;


void getImage(IplImage **img) {
	SRCAM *cam = new SRCAM();
	int count = 0, width, height;
	ImgEntry *images_tmp;

	//IP der Cam: 192.168.15.21
	if(SR_OpenETH(cam, "192.168.15.21"))//SR_OpenFile(cam, "TOF-stream\\dinos0.tof"))
		cout << "Verbunden ...." << endl;
	else {
		cout << "Verbindung fehlgeschlagen!" << endl;
		return;
	}
	SR_SetMode(*cam, 270);
	SR_Acquire(*cam);
	count = SR_GetImageList(*cam, &images_tmp);
	width = SR_GetCols(*cam);
	height = SR_GetRows(*cam);
	for(int i=0; i<3; i++)
	{
				img[i] = cvCreateImage(cvSize(width, height),IPL_DEPTH_16U, 1);
                img[i]->imageData = (char *) images_tmp[i].data;
				
	}
	cvShowImage("Abstand" ,img[0]);
	cvShowImage("Grauwerte" ,img[1]);
	cvShowImage("Vertrauenswerte" ,img[2]);
	cvSaveImage("Abstand.tif" ,img[0]);
	cvSaveImage("Grauwerte.tif" ,img[1]);
	cvSaveImage("Vertrauenswerte.tif" ,img[2]);
	if(SR_Close(*cam) == 0) {
		cout << "Verbindung getrennt!" << endl;
	}
	else {
		cout << "Fehler beim Freigeben!" << endl;
	}
	return;
}

void scale(IplImage **img){
	double minVal,maxVal;
	double scale,shift;
	IplImage *tmp,*big;
	tmp=cvCreateImage(cvSize(img[1]->width,img[1]->height),IPL_DEPTH_8U, 1);
	cvMinMaxLoc(img[1],&minVal,&maxVal,NULL,NULL,0);
	scale=255/(maxVal-minVal);
	shift=-minVal*255/(maxVal-minVal);
	cout<<minVal<<endl;
	cout<<maxVal<<endl;
	cout<<scale<<endl;
	cout<<shift<<endl;
	cvConvertScale(img[1],tmp,scale,shift);
	
	cvSaveImage("Grauscale.tif",tmp);
	big=cvCreateImage(cvSize(img[1]->width*3,img[1]->height*3),IPL_DEPTH_8U, 1);
	cvResize(tmp,big,CV_INTER_LINEAR);
	
	cvSaveImage("Bigscale.tif",big);


}

void depthkill(IplImage **img){
	IplImage *tmp,*product;
	product=cvCreateImage(cvSize(img[1]->width,img[1]->height),IPL_DEPTH_8U, 1);
	tmp=cvLoadImage("Grauscale.tif",0);//cvCreateImage(cvSize(img[1]->width+2,img[1]->height+2),IPL_DEPTH_8U, 1);
	cvFloodFill(img[1],cvPoint(2,2),cvScalar(0,0,0,0),cvScalar(5,0,0,0),cvScalar(5,0,0,0),NULL,4,NULL);
	cvThreshold(img[1], tmp, 70, 255, THRESH_BINARY);
	cvShowImage("Floodfill2",img[1]);
	img[1]=cvLoadImage("Grauscale.tif",0);
	cvShowImage("threshold",tmp);
	cvAnd(tmp,img[2],product);
	cvShowImage("Fertig",product);

}

int main()
{
    IplImage *img[3];
    getImage(img);
	img[0]=cvLoadImage("Abstand.tif",0);
	img[1]=cvLoadImage("Grauwerte.tif",0);
	img[2]=cvLoadImage("Vertrauenswerte.tif",0);
	scale(img);
	img[1]=cvLoadImage("Grauscale.tif",0);
	depthkill(img);
    cout << "Hello world!" << endl;
	cvWaitKey(0);
    return 0;
}
