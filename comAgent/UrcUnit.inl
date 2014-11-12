#ifndef _URC_UNIT_INL_
#define _URC_UNIT_INL_
//////////////////////////////////////////////////////////////////////////
/*
template <class _Tx>
class CUrcType : public CUrcValue
*/
template <class _Tx>
BOOL CUrcType<_Tx>::SetValue(const _Tx& newValue)
{
	if (m_bDefault)
	{
		m_bDefault = FALSE;
		m_value = newValue;
		return TRUE;
	}
	else if (newValue == m_value)
	{
		return FALSE;
	}
	else
	{
		m_value = newValue;
		return TRUE;
	}
}

#ifndef WIN32
template<>
inline BOOL CUrcType<BOOL>::Default()
{
	return FALSE;
}
#endif

template<>
inline int CUrcType<int>::Default()
{
	return 0;
}

template <class _Tx>
BOOL CUrcType<_Tx>::Reset()
{
	if (!m_bDefault)
	{
		m_bDefault = TRUE;
		
		const _Tx val0 = Default();
		
		if (!(m_value == val0))
		{
			m_value = val0;
			return TRUE;
		}
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

#define IMPLEMENT_CONCRETE_URC_BEGIN(_code, _type, _event)				\
	template<>															\
	class CUrcConcreteWorker<_code>: public CUrcWorker					\
	{																	\
	private:															\
		typedef _type MyType;											\
	public:																\
		virtual CUrcValue *Alloc(const CUrcValue *pRef) const			\
		{																\
			if (NULL != pRef)											\
				return new MyType(*static_cast<const MyType*>(pRef));	\
			else														\
				return new MyType;										\
		}																\
		virtual BOOL IsEvent() const									\
		{																\
			return _event;												\
		}

#define IMPLEMENT_CONCRETE_URC_END	};


IMPLEMENT_CONCRETE_URC_BEGIN(URC_SYS, CUrcSys, FALSE)
	/*
	return value:
		-1: AT result is not of the URC
		 0: AT result is wanted but value not changed
		 1: AT result is wanted and value is changed
	*/
	virtual int Decode(const ATResult *pRes, CUrcValue *pVal) const
	{
		ASSERT(pRes != NULL);
		ASSERT(pVal != NULL);

		MyType *p = static_cast<MyType*>(pVal);

#ifdef _DEBUG
		WATCH_POINTER(p);
		p->retain();
#endif
		// New value is partly decided by old value

		if (getEind(pRes) == 128)
		{
			if (p->Value().alpha < SYS_ATREADY)
			{
				return (p->SetValue(DualWord(SYS_ATREADY, 0))) ? 1 : 0;
			}

			return 0;	// ignore this URC
		}
		else if (pRes->check_key("+CREG"))
		{
			if (p->Value().alpha >= SYS_PINREADY)
			{
				WORD creg = convCreg(
					pRes->get_integer(0, 1), 
					p->Value().alpha);

				string _s; int cid;
				pRes->get_string(_s, 0, 3);
				sscanf(_s.c_str(), "%x", &cid);

				return (p->SetValue(DualWord(creg, cid))) ? 1 : 0;
			}
			else
			{
				return 0;
			}
		}
		
		return -1;	// not my type
	}
IMPLEMENT_CONCRETE_URC_END


IMPLEMENT_CONCRETE_URC_BEGIN(URC_SMS, CUrcFlag, FALSE)
	virtual int Decode(const ATResult *pRes, CUrcValue *pVal) const
	{
		ASSERT(pRes != NULL);
		ASSERT(pVal != NULL);

		MyType *p = static_cast<MyType*>(pVal);

		if (getEind(pRes) != 1)
			return -1;	// not my type
		else if (p->SetValue(TRUE))
			return 1;
		else
			return 0;
	}
IMPLEMENT_CONCRETE_URC_END


IMPLEMENT_CONCRETE_URC_BEGIN(URC_PBK, CUrcFlag, FALSE)
	virtual int Decode(const ATResult *pRes, CUrcValue *pVal) const
	{
		ASSERT(pRes != NULL);
		ASSERT(pVal != NULL);

		MyType *p = static_cast<MyType*>(pVal);

		if (getEind(pRes) != 2)
			return -1;
		else if (p->SetValue(TRUE))
			return 1;
		else
			return 0;
	}
IMPLEMENT_CONCRETE_URC_END

IMPLEMENT_CONCRETE_URC_BEGIN(URC_CCCM, CUrcInt, TRUE)
	virtual int Decode(const ATResult *pRes, CUrcValue *pVal) const
	{
		ASSERT(pRes != NULL);
		ASSERT(pVal != NULL);

		MyType *p = static_cast<MyType*>(pVal);

		if (!pRes->check_key("+CCCM"))
			return -1;
		
		p->SetValue(TRUE);
		
		return 1;
	}
IMPLEMENT_CONCRETE_URC_END


IMPLEMENT_CONCRETE_URC_BEGIN(URC_CCWV, CUrcFlag, TRUE)
	virtual int Decode(const ATResult *pRes, CUrcValue *pVal) const
	{
		ASSERT(pRes != NULL);
		ASSERT(pVal != NULL);

		MyType *p = static_cast<MyType*>(pVal);

		if (!pRes->check_key("+CCWV"))
			return -1;
		
		p->SetValue(TRUE);

		return 1;
	}
IMPLEMENT_CONCRETE_URC_END

IMPLEMENT_CONCRETE_URC_BEGIN(URC_CSSI, CUrcCssn, TRUE)
	virtual int Decode(const ATResult *pRes, CUrcValue *pVal) const
	{
		ASSERT(pRes != NULL);
		ASSERT(pVal != NULL);

		MyType *p = static_cast<MyType*>(pVal);

		if (!pRes->check_key("+CSSI"))
		{
			return -1;
		}
		
		CSsnCode ssnCode(
			pRes->get_integer(0, 1),
			pRes->get_integer(0, 2),
			0);

		p->SetValue(ssnCode);

		return 1;
	}
IMPLEMENT_CONCRETE_URC_END

IMPLEMENT_CONCRETE_URC_BEGIN(URC_CSSU, CUrcCssn, TRUE)
	virtual int Decode(const ATResult *pRes, CUrcValue *pVal) const
	{
		ASSERT(pRes != NULL);
		ASSERT(pVal != NULL);

		MyType *p = static_cast<MyType*>(pVal);

		if (!pRes->check_key("+CSSU"))
		{
			return -1;
		}
		
		CSsnCode ssnCode(
			pRes->get_integer(0, 1),
			pRes->get_integer(0, 2),
			pRes->get_integer(0, 4)
			);

		pRes->get_string(ssnCode.number, 0, 3);
		
		p->SetValue(ssnCode);

		return 1;
	}
IMPLEMENT_CONCRETE_URC_END

IMPLEMENT_CONCRETE_URC_BEGIN(URC_CUSD, CUrcUssd, TRUE)
	virtual int Decode(const ATResult *pRes, CUrcValue *pVal) const
	{
		ASSERT(pRes != NULL);
		ASSERT(pVal != NULL);

		MyType *p = static_cast<MyType*>(pVal);

		if (!pRes->check_key("+CUSD"))
		{
			return -1;
		}
		
		CUssdCode	ussdCode;
		string		strTmp;

		ussdCode.n = pRes->get_integer(0, 1);
		ussdCode.dcs = pRes->get_integer(0, 3);
		
		pRes->get_string(strTmp, 0, 2);
		ussdCode.Decode(strTmp, CBS_ALPHA_UCS2/*pValue->dcs*/);

		p->SetValue(ussdCode);

		return 1;
	}
IMPLEMENT_CONCRETE_URC_END

IMPLEMENT_CONCRETE_URC_BEGIN(URC_ERROR, CUrcInt, TRUE)
	virtual int Decode(const ATResult *pRes, CUrcValue *pVal) const
	{
		ASSERT(pRes != NULL);
		ASSERT(pVal != NULL);

		MyType *p = static_cast<MyType*>(pVal);

		int err = -1;

		if (pRes->check_key("+CME ERROR"))
			err = pRes->get_integer(0, 1);
		else if (pRes->check_key("ERROR"))
			err = ER_UNKNOWN;
		
		if (err >= 0)
		{
			p->SetValue(err);
			return 1;
		}
		
		return -1;
	}
IMPLEMENT_CONCRETE_URC_END

IMPLEMENT_CONCRETE_URC_BEGIN(URC_CCWA, CUrcEmpty, TRUE)
	virtual int Decode(const ATResult *pRes, CUrcValue *pVal) const
	{
		ASSERT(pRes != NULL);
		ASSERT(pVal != NULL);

		return (pRes->check_key("+CCWA")) ? 1 : -1;
	}
IMPLEMENT_CONCRETE_URC_END

IMPLEMENT_CONCRETE_URC_BEGIN(URC_RING, CUrcEmpty, TRUE)
	virtual int Decode(const ATResult *pRes, CUrcValue *pVal) const
	{
		ASSERT(pRes != NULL);
		ASSERT(pVal != NULL);

		return (pRes->check_key("RING")) ? 1 : -1;
	}
IMPLEMENT_CONCRETE_URC_END

IMPLEMENT_CONCRETE_URC_BEGIN(URC_NOCA, CUrcEmpty, TRUE)
	virtual int Decode(const ATResult *pRes, CUrcValue *pVal) const
	{
		ASSERT(pRes != NULL);
		ASSERT(pVal != NULL);

		return (pRes->check_key("NO CARRIER")) ? 1 : -1;
	}
IMPLEMENT_CONCRETE_URC_END

IMPLEMENT_CONCRETE_URC_BEGIN(URC_ECSQ, CUrcInt, FALSE)
	virtual int Decode(const ATResult *pRes, CUrcValue *pVal) const
	{
		ASSERT(pRes != NULL);
		ASSERT(pVal != NULL);
		
		MyType *p = static_cast<MyType*>(pVal);
		
		if (!pRes->check_key("+ECSQ"))
			return -1;

		int rssi = pRes->get_integer(0, 1);

		// rssi: 0, 1~31, 99
		if (rssi < 1 || rssi > 31)
			rssi = 0;
		 else if(rssi>=13 && rssi<=31)
            rssi=5;
           else if (rssi>=11 && rssi<13)                 //rssi = (rssi * 5 + 15) / 31; zhang shao hua
             rssi=4;
             else if(rssi>=7 && rssi<11)
                 rssi=3;
                 else if(rssi>=3 && rssi<7)
                   rssi=2;
                   else if (rssi>=1 && rssi<3)
                     rssi=1;   

		if (p->SetValue(rssi))
			return 1;
		else
			return 0;
	}
IMPLEMENT_CONCRETE_URC_END


IMPLEMENT_CONCRETE_URC_BEGIN(URC_CIEV, CUrcMsgStatus, TRUE)
	virtual int Decode(const ATResult *pRes, CUrcValue *pVal) const
	{
		ASSERT(pRes != NULL);
		ASSERT(pVal != NULL);

		MyType *p = static_cast<MyType*>(pVal);

		if(!pRes->check_key("+CIEV"))
			return -1;

		CMsgStatusInd statusInd;

		statusInd.ind = pRes->get_integer(0, 1);
		pRes->get_string(statusInd.value, 0, 2);

		p->SetValue(statusInd);

		return 1;
	}
IMPLEMENT_CONCRETE_URC_END

IMPLEMENT_CONCRETE_URC_BEGIN(URC_CMTI, CUrcMsg, TRUE)
	virtual int Decode(const ATResult *pRes, CUrcValue *pVal) const
	{
		ASSERT(pRes != NULL);
		ASSERT(pVal != NULL);

		MyType *p = static_cast<MyType*>(pVal);

		if (!pRes->check_key("+CMTI"))
			return -1;
		
		CMsgInd msgInd;

		msgInd.index = pRes->get_integer(0, 2);	// index
	
		pRes->get_string(msgInd.mem, 0, 1);		// mem

		p->SetValue(msgInd);

		return 1;
	}
IMPLEMENT_CONCRETE_URC_END

IMPLEMENT_CONCRETE_URC_BEGIN(URC_CBM, CUrcCbMsg, TRUE)
	virtual int Decode(const ATResult *pRes, CUrcValue *pVal) const
	{
		ASSERT(pRes != NULL);
		ASSERT(pVal != NULL);
		
		MyType *p = static_cast<MyType*>(pVal);
		
		if (!pRes->check_key("+CBM"))
			return -1;
		
		CCbMsgInd msgInd;
		
		msgInd.length = pRes->get_integer(0, 1);
		
		pRes->get_string(msgInd.pdu, 0, 2);
		
		p->SetValue(msgInd);
		
		return 1;
	}
IMPLEMENT_CONCRETE_URC_END

IMPLEMENT_CONCRETE_URC_BEGIN(URC_ETWS, CUrcEtwsMsg, TRUE)
	virtual int Decode(const ATResult *pRes, CUrcValue *pVal) const
	{
		ASSERT(pRes != NULL);
		ASSERT(pVal != NULL);
		
		MyType *p = static_cast<MyType*>(pVal);
		
		if (!pRes->check_key("+ETWS"))
			return -1;
		
		CEtwsMsgInd msgInd;
		
		msgInd.warningType = pRes->get_integer(0, 1);
		msgInd.msgId = pRes->get_integer(0, 2);
		msgInd.serialNum = pRes->get_integer(0, 3);

		pRes->get_string(msgInd.plmnId, 0, 4);
		pRes->get_string(msgInd.securityInfo, 0, 5);
		
		p->SetValue(msgInd);
		
		return 1;
	}
IMPLEMENT_CONCRETE_URC_END

IMPLEMENT_CONCRETE_URC_BEGIN(URC_CGREG, CUrcUssd, FALSE)
	virtual int Decode(const ATResult *pRes, CUrcValue *pVal) const
	{
		ASSERT(pRes != NULL);
		ASSERT(pVal != NULL);

		MyType *p = static_cast<MyType*>(pVal);

		if (!pRes->check_key("+CGREG"))
			return -1;
		
		CUssdCode gregCode;

		gregCode.n = pRes->get_integer(0, 1);		//	<stat>
		gregCode.dcs = pRes->get_integer(0, 4);	//	<AcT>
		pRes->get_string(gregCode.str_urc, 0, 3);	//	<ci>

		if (p->SetValue(gregCode))
			return 1;
		else
			return 0;
	}
IMPLEMENT_CONCRETE_URC_END

IMPLEMENT_CONCRETE_URC_BEGIN(URC_STKPCI, CUrcMsg, TRUE)
	virtual int Decode(const ATResult *pRes, CUrcValue *pVal) const
	{
		ASSERT(pRes != NULL);
		ASSERT(pVal != NULL);

		MyType *p = static_cast<MyType*>(pVal);

		if (!pRes->check_key("+STKPCI"))
			return -1;
		
		CMsgInd msgInd;

		msgInd.index = pRes->get_integer(0, 1);

		pRes->get_string(msgInd.mem, 0, 2);

		p->SetValue(msgInd);

		return 1;
	}
IMPLEMENT_CONCRETE_URC_END

IMPLEMENT_CONCRETE_URC_BEGIN(URC_CLCC, CUrcCalls, FALSE)
	virtual int Decode(const ATResult *pRes, CUrcValue *pVal) const
	{
		return -1;
	}
IMPLEMENT_CONCRETE_URC_END

IMPLEMENT_CONCRETE_URC_BEGIN(URC_PSBEARER, CUrcBearer, FALSE)
	virtual int Decode(const ATResult *pRes, CUrcValue *pVal) const
	{
		ASSERT(pRes != NULL);
		ASSERT(pVal != NULL);

		MyType *p = static_cast<MyType*>(pVal);

		if (!pRes->check_key("+PSBEARER"))
			return -1;

		DualWord bearer(
			pRes->get_integer(0, 1),
			pRes->get_integer(0, 2)
			);

		if (p->SetValue(bearer))
			return 1;
		else
			return 0;
	}
IMPLEMENT_CONCRETE_URC_END

#endif // _URC_UNIT_INL_
