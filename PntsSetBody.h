#ifndef	_CCL_PNTSSET_BODY
#define	_CCL_PNTSSET_BODY

#include "GLKLib/GLK.h"

#include <vector>

#define MAX(a,b)		(((a)>(b))?(a):(b))
#define MIN(a,b)		(((a)<(b))?(a):(b))

namespace rs {
class float3; // forward declaration from rs::point3
}

class PntsSetBody : public GLKEntity
{
public:
	PntsSetBody(void);
	virtual ~PntsSetBody(void);

	void BuildGLList(bool bWithArrow);
	void DeleteGLList();

	void ClearAll();

	void CompRange();

	bool ImportOBJFile(char *filename);
	bool ExportOBJFile(char *filename);
	bool ImportPWNFile(char *filename);
	bool ExportPWNFile(char *filename);

	virtual void drawShade();
	virtual void drawProfile();
	virtual void drawMesh() {};
	virtual void drawPreMesh() {};
	virtual void drawHighLight() {};
	virtual float getRange() {return m_range;}

	void SetLighting(bool bLight) {m_Lighting=bLight;};
	bool GetLighting() {return m_Lighting;};

	int GetPntsNum() {return m_pntsNum;};
	float* GetPntPosArrayPtr() {return m_pntPosArray;};
	float* GetNormalArrayPtr() {return m_normalArray;};
	void SetPntsNum(int num) {m_pntsNum=num;};
	void SetPntPosArrayPtr(float *ptr) {m_pntPosArray=ptr;};
	void SetNormalArrayPtr(float *ptr) {m_normalArray=ptr;};

    void calculateNormals(bool show_progress = false);

    /*!
     * Flip normals to align them with a given point
     */
    void alignNormals(float camera_normal_x, float camera_normal_y, float camera_normal_z);
    
    void setData(const std::vector<rs::float3>& points);
private:
	bool m_Lighting;	float m_range;
	int m_drawListID_Points, m_drawListID_NormalArrow;

	bool m_withNormal;
	int m_pntsNum;
	float* m_pntPosArray;		float* m_normalArray;
};

#endif
