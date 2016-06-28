#include"highgui.h"
#include"cv.h"
#include <iostream>
#include <vector>
using namespace std;
CvSubdiv2D* init_delaunay( CvMemStorage* storage,//��ʼ�������ʷֽṹ��Ϊ����䵥Ԫ  
						  CvRect rect )  
{  
	CvSubdiv2D* subdiv;//�����ʷֵ����ݵ�Ԫ  

	subdiv = cvCreateSubdiv2D( CV_SEQ_KIND_SUBDIV2D, sizeof(*subdiv),  
		sizeof(CvSubdiv2DPoint),  
		sizeof(CvQuadEdge2D),  
		storage );  
	cvInitSubdivDelaunay2D( subdiv, rect );  

	return subdiv;  
}  

static void draw_subdiv_point( IplImage* img, CvPoint2D32f fp, CvScalar color )//���������ʷֵĶ���  
{  
	cvCircle( img, cvPoint(cvRound(fp.x), cvRound(fp.y)),1, color, 1, 8, 0 );  
}  



static void draw_subdiv_edge( IplImage* img, CvSubdiv2DEdge edge, CvScalar color )//���������ʷֵı�  
{  
	CvSubdiv2DPoint* org_pt;//Դ����  
	CvSubdiv2DPoint* dst_pt;//Ŀ�ض���  
	CvPoint2D32f org;  
	CvPoint2D32f dst;  
	CvPoint iorg, idst;  

	org_pt = cvSubdiv2DEdgeOrg(edge);//ͨ���߻�ȡ����  
	dst_pt = cvSubdiv2DEdgeDst(edge);  

	if( org_pt && dst_pt )//��������˵㲻Ϊ��  
	{  
		org = org_pt->pt;  
		dst = dst_pt->pt;  

		iorg = cvPoint( cvRound( org.x ), cvRound( org.y ));  
		idst = cvPoint( cvRound( dst.x ), cvRound( dst.y ));  

		cvLine( img, iorg, idst, color, 1, CV_AA, 0 );  
	}  
}  


static void draw_subdiv_d( IplImage* img, CvSubdiv2D* subdiv,  
						CvScalar delaunay_color, CvScalar voronoi_color  )//�����ʷֺ�ϸ��  
{  
	CvSeqReader  reader;  
	int i, total = subdiv->edges->total;//�ߵ�����  
	int elem_size = subdiv->edges->elem_size;//�ߵĴ�С  
	cout<<typeid(subdiv->edges).name()<<endl;  

	cvStartReadSeq( (CvSeq*)(subdiv->edges), &reader, 0 );//ʹ��CvSeqReader����Delaunay����Voronoi��  
    cvCalcSubdivVoronoi2D(subdiv);
	for( i = 0; i < total; i++ )  
	{  
		CvQuadEdge2D* edge = (CvQuadEdge2D*)(reader.ptr);  

		if( CV_IS_SET_ELEM( edge ))  
		{  
		    //draw_subdiv_edge( img, (CvSubdiv2DEdge)edge + 1, voronoi_color );  
			draw_subdiv_edge( img, (CvSubdiv2DEdge)edge, delaunay_color );  
			

		}  

		CV_NEXT_SEQ_ELEM( elem_size, reader );  
	}  

}  




static void draw_subdiv_v( IplImage* img, CvSubdiv2D* subdiv,  
						CvScalar delaunay_color, CvScalar voronoi_color  )//�����ʷֺ�ϸ��  
{  
	CvSeqReader  reader;  
	int i, total = subdiv->edges->total;//�ߵ�����  
	int elem_size = subdiv->edges->elem_size;//�ߵĴ�С  
	cout<<typeid(subdiv->edges).name()<<endl;  

	cvStartReadSeq( (CvSeq*)(subdiv->edges), &reader, 0 );//ʹ��CvSeqReader����Delaunay����Voronoi��  
    cvCalcSubdivVoronoi2D(subdiv);
	for( i = 0; i < total; i++ )  
	{  
		CvQuadEdge2D* edge = (CvQuadEdge2D*)(reader.ptr);  

		if( CV_IS_SET_ELEM( edge ))  
		{  
		    draw_subdiv_edge( img, (CvSubdiv2DEdge)edge + 1, voronoi_color );  
			//draw_subdiv_edge( img, (CvSubdiv2DEdge)edge, delaunay_color );  
			

		}  

		CV_NEXT_SEQ_ELEM( elem_size, reader );  
	}  

}  


void locate_point( CvSubdiv2D* subdiv, CvPoint2D32f fp, IplImage* img,//���������ʷֵı�  
				  CvScalar active_color )  
{  
	CvSubdiv2DEdge e;  
	CvSubdiv2DEdge e0 = 0;  
	CvSubdiv2DPoint* p = 0;  

	cvSubdiv2DLocate(subdiv, fp, &e0, &p );  

	if( e0 )  
	{  
		e = e0;  
		do  
		{  
			draw_subdiv_edge( img, e, active_color );  
			e = cvSubdiv2DGetEdge(e,CV_NEXT_AROUND_LEFT);  
		}  
		while( e != e0 );  
	}  

	draw_subdiv_point( img, fp, active_color );  
}  


int main()
{
	IplImage* img=cvLoadImage("2.bmp");
	int height=img->height;
	int width=img->width;
	int pixpbyte=img->nChannels;
	int step=(img->widthStep+3)&(~3);
	uchar*ptr=(uchar*)img->imageData;
	//uchar *b,*g,*r;

	CvScalar active_facet_color, delaunay_color, voronoi_color, bkgnd_color;  

	active_facet_color = CV_RGB( 255, 0, 0 );//��ɫ  
	delaunay_color  = CV_RGB(255,0,255);//��ɫ  
	voronoi_color = CV_RGB(0, 255, 255);//��ɫ  
	bkgnd_color = CV_RGB(255,255,255);//��ɫ  



	CvRect rect={0,0,width,height};
	CvMemStorage*storage;
	storage=cvCreateMemStorage(0);
	CvSubdiv2D*subdiv;
	subdiv=init_delaunay(storage,rect);
	//CvPoint2D32f* pcv_Imgpoints = NULL;
	//��ȡ��ɫ���ص�ĸ���
	vector<CvPoint2D32f>points; 
	int red_points_counts=0;
	cvNamedWindow( "win");  
	for (int i=0;i<height;i++)
	{
		for (int j=0;j<width;j++)
		{
			uchar b=(uchar)ptr[i*step+j*pixpbyte+0];
			uchar g=(uchar)ptr[i*step+j*pixpbyte+1];
			uchar r=(uchar)ptr[i*step+j*pixpbyte+2];
			if ((r==255)&&(b==0)&&(g==0))
			{
				red_points_counts++;
				CvPoint2D32f fp=cvPoint2D32f((float)j,(float)i);

				//cvCircle( img, cvPoint(j, i), 1, CV_RGB( 0, 255, 0 ), 1, 8, 0 );  

				points.push_back(fp);
				cvSubdivDelaunay2DInsert( subdiv, fp );
			}


		}
	}
	cvCalcSubdivVoronoi2D( subdiv );//locate_point( subdiv, fp, img, active_facet_color );
	draw_subdiv_d( img, subdiv, delaunay_color, voronoi_color);  
    cvSaveImage("2.PNG",img,0);

    IplImage*pImg;
	CvSize imgsize;
	imgsize.width=width;
	imgsize.height=height;
    pImg=cvCreateImage(imgsize,8,3);
    ptr=(uchar*)pImg->imageData;
	for (int i=0;i<height;i++)
	{
		for (int j=0;j<width;j++)
		{
			(uchar)ptr[i*step+j*pixpbyte+0]=255;
			(uchar)ptr[i*step+j*pixpbyte+1]=255;
		    (uchar)ptr[i*step+j*pixpbyte+2]=255;
		

		}
	}

	draw_subdiv_v(pImg ,subdiv, delaunay_color, voronoi_color);  
	
	cvSaveImage("3.PNG",pImg,0);

	//cvShowImage( "win", img );
	cvWaitKey(0);

 img=cvLoadImage("11.bmp");
draw_subdiv_d( img, subdiv, delaunay_color, voronoi_color); 
cvSaveImage("4.PNG",img,0);

img=cvLoadImage("2.bmp");
draw_subdiv_v( img, subdiv, delaunay_color, voronoi_color); 
cvSaveImage("5.PNG",img,0);
	return 0;
}