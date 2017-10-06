#ifndef _CCL_PNTS_DATABOARD
#define _CCL_PNTS_DATABOARD

class PntsSetBody;

class PntsDataBoard
{
public:
	PntsDataBoard(void) {
		m_pntsSetBody = NULL;
		m_bPntNormalDisplay=false; 
	};
	virtual ~PntsDataBoard(void) {};

	PntsSetBody *m_pntsSetBody;

	bool m_bPntNormalDisplay;
};

#endif