#ifndef _CCL_PNTCUDA_OPERATION
#define _CCL_PNTCUDA_OPERATION

class PntsSetBody;

class PntsSetOperation
{
public:
	PntsSetOperation(void);
	~PntsSetOperation(void);

	static void MakeCenter(PntsSetBody *pntsBody);

//	static void OrthogonalNormalOrientation(PntsSetBody *pntsBody, int voxRes, ortPnts_type type = pntsOrtFBLR);
//	static void PCANormalEvaluation(PntsSetBody *pntsBody, int hashingRes, float supportSize);
//	static void CompVDFieldByPointSet(PntsSetBody *pntsBody, int vdRes, VoronoiDiagramCudaBody *vdFieldBody);
//	static void CompMedialAxisByPointSet(PntsSetBody *pntsBody, int vdRes, float angleThreshold, int downSampleRatio, bool bInsideOrOutside, bool bByPntsOrientation);
};

#endif