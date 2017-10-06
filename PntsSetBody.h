#ifndef	_CCL_PNTSSET_BODY
#define	_CCL_PNTSSET_BODY

#include "GLKLib/GLK.h"

#define MAX(a,b)		(((a)>(b))?(a):(b))
#define MIN(a,b)		(((a)<(b))?(a):(b))

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

private:
	bool m_Lighting;	float m_range;
	int m_drawListID_Points, m_drawListID_NormalArrow;

	bool m_withNormal;
	int m_pntsNum;
	float* m_pntPosArray;		float* m_normalArray;
};

#endif
