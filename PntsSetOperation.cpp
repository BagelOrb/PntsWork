#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <memory.h>
#include <math.h>
#include <time.h>

#if defined (__linux__)
#include <sys/uio.h>
#include <dirent.h>
#else
#include <io.h>
#endif

#include "PntsSetBody.h"
#include "PntsSetOperation.h"

//----------------------------------------------------------------------------------------------------------------------
PntsSetOperation::PntsSetOperation(void)
{
}

PntsSetOperation::~PntsSetOperation(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
void PntsSetOperation::MakeCenter(PntsSetBody *pntsBody)
{
	int pntsNum = pntsBody->GetPntsNum();
	float *pntsPosArrayPtr = pntsBody->GetPntPosArrayPtr();
	float *pntsNvArrayPtr = pntsBody->GetNormalArrayPtr();

	float minX, maxX, minY, maxY, minZ, maxZ, cx,cy,cz;
	maxX = maxY = maxZ = -1.0e+10f;
	minX = minY = minZ = 1.0e+10f;
	for (int i = 0; i < pntsNum; i++) {
		if (pntsPosArrayPtr[i*3] < minX) minX = pntsPosArrayPtr[i*3];
		if (pntsPosArrayPtr[i*3] > maxX) maxX = pntsPosArrayPtr[i*3];
		if (pntsPosArrayPtr[i*3 + 1] < minY) minY = pntsPosArrayPtr[i*3 + 1];
		if (pntsPosArrayPtr[i*3 + 1] > maxY) maxY = pntsPosArrayPtr[i*3 + 1];
		if (pntsPosArrayPtr[i*3 + 2] < minZ) minZ = pntsPosArrayPtr[i*3 + 2];
		if (pntsPosArrayPtr[i*3 + 2] > maxZ) maxZ = pntsPosArrayPtr[i*3 + 2];
	}

	cx = (minX + maxX)*0.5f;	cy = (minY + maxY)*0.5f;	cz = (minZ + maxZ)*0.5f;
	for (int i = 0; i < pntsNum; i++) {
		pntsPosArrayPtr[i * 3] = pntsPosArrayPtr[i * 3] - cx;
		pntsPosArrayPtr[i * 3 + 1] = pntsPosArrayPtr[i * 3 + 1] - cy;
		pntsPosArrayPtr[i * 3 + 2] = pntsPosArrayPtr[i * 3 + 2] - cz;
	}
}
