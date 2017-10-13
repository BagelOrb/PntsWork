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

#include "utils/SparsePointGrid.h"
using namespace cura;

#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Eigenvalues>
using namespace Eigen;

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

void PntsSetBody::calculateNormals()
{
    int avg_cells_per_dimension = 20;
    int k = 20;
    
    
    
    FPoint3 min = FPoint3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    FPoint3 max = FPoint3(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
    
    for (int i = 0; i < m_pntsNum; i++)
    {
        float& x = m_pntPosArray[i * 3];
        float& y = m_pntPosArray[i * 3 + 1];
        float& z = m_pntPosArray[i * 3 + 2];
        min.x = std::min(min.x, x);
        min.y = std::min(min.y, y);
        min.z = std::min(min.z, z);
        max.x = std::max(max.x, x);
        max.y = std::max(max.y, y);
        max.z = std::max(max.z, z);
    }
    
    FPoint3 size = max - min;
    float avg_size = (size.x + size.y + size.z) / 3.0;
    float cell_size = avg_size / avg_cells_per_dimension;
    
    
    struct Locator
    {
        FPoint3 operator()(const FPoint3& p) const
        { 
            return p;
        }
    };
    SparsePointGrid<FPoint3, Locator> grid(cell_size);
    
    for (int i = 0; i < m_pntsNum; i++)
    {
        float& x = m_pntPosArray[i * 3];
        float& y = m_pntPosArray[i * 3 + 1];
        float& z = m_pntPosArray[i * 3 + 2];
        grid.insert(FPoint3(x, y, z));
    }
    
    
    for (int i = 0; i < m_pntsNum; i++)
    {
        FPoint3 p(m_pntPosArray[i * 3], m_pntPosArray[i * 3 + 1], m_pntPosArray[i * 3 + 2]);
        std::vector<FPoint3> knn = grid.getKnn(p, k, cell_size);
        Eigen::MatrixXf mat(knn.size(), 3);
        for (int nn_idx = 0; nn_idx < knn.size(); nn_idx++)
        {
            const FPoint3& nn = knn[nn_idx];
            mat(nn_idx, 0) = nn.x;
            mat(nn_idx, 1) = nn.y;
            mat(nn_idx, 2) = nn.z;
        }
        MatrixXf centered = mat.rowwise() - mat.colwise().mean();
        MatrixXf cov = (centered.adjoint() * centered) / double(mat.rows() - 1);
        SelfAdjointEigenSolver<MatrixXf> es;
        es.compute(cov);
        Vector3f eigenvalues = es.eigenvalues();
        int lowest_eigenvalue_idx = 0;
        float lowest_eigenvalue = eigenvalues[0];
        for (int eigenvalue_idx = 1; eigenvalue_idx < 3; eigenvalue_idx++)
        {
            if (eigenvalues[eigenvalue_idx] < lowest_eigenvalue)
            {
                lowest_eigenvalue = eigenvalues[eigenvalue_idx];
                lowest_eigenvalue_idx = eigenvalue_idx;
            }
        }
        Vector3f last_component = es.eigenvectors().col(lowest_eigenvalue_idx);
        if (last_component[1] < 0)
        {
            last_component *= -1.0;
        }
        float& normal_x = m_normalArray[i * 3];
        float& normal_y = m_normalArray[i * 3 + 1];
        float& normal_z = m_normalArray[i * 3 + 2];
        normal_x = last_component[0];
        normal_y = last_component[1];
        normal_z = last_component[2];
        
    }
}
