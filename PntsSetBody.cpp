#define _CRT_SECURE_NO_DEPRECATE

#if defined (__linux__)
#include <GL/glew.h>
#include <GL/glut.h>
#include <sys/uio.h>
#include <dirent.h>
#else
#include <GL/glew.h>
#include <GL/glaux.h>
#include <GL/glut.h>
#include <io.h>
#endif

#include <string.h>
#include <time.h>
#include <math.h>

#include "PntsSetBody.h"

#include "utils/SparseGrid.h"

extern GLK _pGLK;

//----------------------------------------------------------------------------------------------------------------------
PntsSetBody::PntsSetBody(void)
{
	m_range=1.0;		m_pntsNum=0;		
	m_Lighting = false; 
	m_withNormal = false;
	m_drawListID_Points = m_drawListID_NormalArrow = -1;
}

PntsSetBody::~PntsSetBody(void)
{
	ClearAll();
	DeleteGLList();
}

void PntsSetBody::ClearAll()
{
	if (m_pntsNum>0) {
		free(m_pntPosArray);	free(m_normalArray);	
		m_pntsNum=0;
	}
	m_range=1.0;
}
	
void PntsSetBody::BuildGLList(bool bWithArrow)
{
	DeleteGLList();
	m_drawListID_Points = glGenLists(1);
	if (bWithArrow)	m_drawListID_NormalArrow = glGenLists(1);

	//--------------------------------------------------------------------------------------
	//	Build the GL List for points
	glNewList(m_drawListID_Points, GL_COMPILE);
	glBegin(GL_POINTS);
	for (int i = 0; i<m_pntsNum; i++) {
		glNormal3f(m_normalArray[i * 3], m_normalArray[i * 3 + 1], m_normalArray[i * 3 + 2]);
		glVertex3f(m_pntPosArray[i * 3], m_pntPosArray[i * 3 + 1], m_pntPosArray[i * 3 + 2]);
	}
	glEnd();
	//--------------------------------------------------------------------------------------
	glEndList();

	//--------------------------------------------------------------------------------------
	//	Build the GL List of drawProfile
	if (bWithArrow) {
		glNewList(m_drawListID_NormalArrow, GL_COMPILE);
		float xx,yy,zz,arrowLength=1.0f;
		glBegin(GL_LINES);
		glColor3f(.5f,.5f,.5f);
		//--------------------------------------------------------------------------------------
		for(int i=0;i<m_pntsNum;i++) {
			xx=m_pntPosArray[i*3];		yy=m_pntPosArray[i*3+1];	zz=m_pntPosArray[i*3+2];
			glVertex3f(xx,yy,zz);
			xx+=(m_normalArray[i*3]*arrowLength);
			yy+=(m_normalArray[i*3+1]*arrowLength);
			zz+=(m_normalArray[i*3+2]*arrowLength);
			glVertex3f(xx,yy,zz);
		}
		//--------------------------------------------------------------------------------------
		glEnd();
		glEndList();
		printf("CORRET: %d\n", m_drawListID_NormalArrow);
	}
}

void PntsSetBody::DeleteGLList()
{
	if (m_drawListID_Points != -1) { glDeleteLists(m_drawListID_Points, 1);	m_drawListID_Points = -1; }
	if (m_drawListID_NormalArrow != -1) { glDeleteLists(m_drawListID_NormalArrow, 1);	m_drawListID_NormalArrow = -1; }
}

void PntsSetBody::CompRange()
{
	if (m_pntsNum==0) {m_range=1.0; return;}
	float d2;

	m_range=m_range*m_range;
	for(int i=0;i<m_pntsNum;i++) {
		d2=m_pntPosArray[i*3]*m_pntPosArray[i*3]
			+m_pntPosArray[i*3+1]*m_pntPosArray[i*3+1]
			+m_pntPosArray[i*3+2]*m_pntPosArray[i*3+2];
		if (d2>m_range) m_range=d2;
	}
	m_range=sqrt(m_range);
}

void PntsSetBody::drawShade()
{
	float gwidth,range,width,scale;	int sx,sy;
	_pGLK.GetScale(scale);
	range=_pGLK.GetRange();
	_pGLK.GetSize(sx,sy);	width=(sx>sy)?sx:sy;

	gwidth=2.0f;
	if (m_Lighting) {
		glPointSize((float)(gwidth*.1*width*0.5/range*scale)*0.866f);
		glEnable(GL_POINT_SMOOTH);	// without this, the rectangule will be displayed for point
	}
	else {
		glDisable(GL_LIGHTING);
		glDisable(GL_POINT_SMOOTH);	// without this, the rectangule will be displayed for point
		glPointSize((float)(gwidth*.01*width*0.5/range*scale)*0.866f);
	}

	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.0);
	glColor3f(174.0f / 255.0f, 198.0f / 255.0f, 188.0f / 255.0f);
	glEnable(GL_POINT_SMOOTH);	// without this, the rectangule will be displayed for point
	glCallList(m_drawListID_Points);
}

void PntsSetBody::drawProfile()
{
	if (m_drawListID_NormalArrow < 0) return;

	glDisable(GL_LIGHTING);
	glCallList(m_drawListID_NormalArrow);
}
	
bool PntsSetBody::ImportPWNFile(char *filename)
{
	FILE *fp;	int i,pntsNum;	float xx,yy,zz;

	fp = fopen(filename, "r");
    if(!fp) {
	    printf("===============================================\n");
	    printf("Can not open the data file - PWN File Import!\n");
	    printf("===============================================\n");
	    return false;
	}

	ClearAll();	
	//--------------------------------------------------------------------------------------------------------
	fscanf(fp,"%d\n",&pntsNum);
	//--------------------------------------------------------------------------------------------------------
	m_pntPosArray=(float*)malloc(sizeof(float)*pntsNum*3);
	m_normalArray=(float*)malloc(sizeof(float)*pntsNum*3);
	for(i=0;i<pntsNum;i++) {
		fscanf(fp,"%f %f %f\n",&xx,&yy,&zz);
		//xx=xx*100.0f;	yy=yy*100.0f; zz=zz*100.0f;
		m_pntPosArray[i*3]=xx;	m_pntPosArray[i*3+1]=yy;	m_pntPosArray[i*3+2]=zz;
	}
	for(i=0;i<pntsNum;i++) {
		fscanf(fp,"%f %f %f\n",&xx,&yy,&zz);
		m_normalArray[i*3]=xx;	m_normalArray[i*3+1]=yy;	m_normalArray[i*3+2]=zz;
	}
	fclose(fp);
	//--------------------------------------------------------------------------------------------------------
	m_pntsNum=pntsNum;

	printf("Pnt number: %d\n",pntsNum);

	return true;
}

bool PntsSetBody::ExportPWNFile(char *filename)
{
	FILE *fp;
	int i,pntsNum;	float *pntsPosArray,*pntsNvArray;

	fp = fopen(filename, "w");
    if(!fp) {
	    printf("===============================================\n");
	    printf("Can not open the data file - PWN File Import!\n");
	    printf("===============================================\n");
	    return false;
	}
	
	pntsNum=m_pntsNum;	pntsPosArray=m_pntPosArray;	pntsNvArray=m_normalArray;

	fprintf(fp,"%d\n",pntsNum);
	for(i=0;i<pntsNum;i++) {fprintf(fp,"%f %f %f\n",pntsPosArray[i*3],pntsPosArray[i*3+1],pntsPosArray[i*3+2]);}
	for(i=0;i<pntsNum;i++) {fprintf(fp,"%f %f %f\n",pntsNvArray[i*3],pntsNvArray[i*3+1],pntsNvArray[i*3+2]);}
	fclose(fp);

	return true;
}

bool PntsSetBody::ImportOBJFile(char *filename)
{
	FILE *fp;
	char linebuf[256],buf[100];
	int pntsNum,nvNum;
	float xx,yy,zz;

	fp = fopen(filename, "r");
    if(!fp) {
	    printf("===============================================\n");
	    printf("Can not open the data file - OBJ File Import!\n");
	    printf("===============================================\n");
	    return false;
	}

	//--------------------------------------------------------------------------------------------------------
	//	Analysis of OBJ file
	pntsNum=nvNum=0;
	while(!feof(fp)) {
// 		sprintf(buf,"");
// 		sprintf(linebuf,"");
		fgets(linebuf, 255, fp);
		sscanf(linebuf,"%s",buf);
	
		if ( (strlen(buf)==1) && (buf[0]=='v') ) pntsNum++;
		if ( (strlen(buf)==2) && (buf[0]=='v') && (buf[1]=='n') ) nvNum++;
	}
	fclose(fp);
	if (pntsNum==0) return false;

	//--------------------------------------------------------------------------------------------------------
	//	Data import
	printf("Pnt number: %d\nNormal vector number: %d\n",pntsNum,nvNum);
	ClearAll();	
	//--------------------------------------------------------------------------------------------------------
	m_pntPosArray=(float*)malloc(sizeof(float)*pntsNum*3);
	m_normalArray=(float*)malloc(sizeof(float)*pntsNum*3);
	//--------------------------------------------------------------------------------------------------------
	fp = fopen(filename, "r");	
	int pntIndex=0,nvIndex=0;
	while(!feof(fp)) {
// 		sprintf(buf,"");
// 		sprintf(linebuf,"");
		fgets(linebuf, 255, fp);
		sscanf(linebuf,"%s",buf);
	
		if ( (strlen(buf)==1) && (buf[0]=='v') ) {
			sscanf(linebuf, "%s %f %f %f \n", buf, &xx, &yy, &zz);
			m_pntPosArray[pntIndex*3]=xx;	
			m_pntPosArray[pntIndex*3+1]=yy;
			m_pntPosArray[pntIndex*3+2]=zz;
			pntIndex++;
		}	
		if ( (strlen(buf)==2) && (buf[0]=='v') && (buf[1]=='n') ) {
			if (nvIndex<pntsNum) {
				sscanf(linebuf, "%s %f %f %f \n", buf, &xx, &yy, &zz);
				m_normalArray[nvIndex*3]=xx;	
				m_normalArray[nvIndex*3+1]=yy;
				m_normalArray[nvIndex*3+2]=zz;
				nvIndex++;
			}
		}
	}
	fclose(fp);
	//--------------------------------------------------------------------------------------------------------
	m_pntsNum=pntsNum;

	return true;
}

bool PntsSetBody::ExportOBJFile(char *filename)
{
	FILE *fp;
	int i,pntsNum;	float *pntsPosArray,*pntsNvArray;

	fp = fopen(filename, "w");
    if(!fp) {
	    printf("===============================================\n");
	    printf("Can not open the data file - OBJ File Import!\n");
	    printf("===============================================\n");
	    return false;
	}
	
	pntsNum=m_pntsNum;	pntsPosArray=m_pntPosArray;	pntsNvArray=m_normalArray;

	fprintf(fp,"\n\n");
	for(i=0;i<pntsNum;i++) {fprintf(fp,"v %f %f %f\n",pntsPosArray[i*3],pntsPosArray[i*3+1],pntsPosArray[i*3+2]);}
	fprintf(fp,"\n\n");
	for(i=0;i<pntsNum;i++) {fprintf(fp,"vn %f %f %f\n",pntsNvArray[i*3],pntsNvArray[i*3+1],pntsNvArray[i*3+2]);}
	fclose(fp);

	return true;
}
