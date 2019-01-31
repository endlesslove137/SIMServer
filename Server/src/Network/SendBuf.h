#ifndef __SendBuf_H__
#define __SendBuf_H__

#include "CircuitQueue.h"

class SendBuffer
{
public:
	SendBuffer()
	{
		m_pQueue = NULL;
	}
	virtual ~SendBuffer()
	{
		if (m_pQueue) delete m_pQueue;
	}

	inline void Create(int nBufferSize, unsigned int nExtraBuffeSize, unsigned int nRecvOffset)
	{
		if (m_pQueue) delete m_pQueue;
		m_pQueue = new CircuitQueue<char>;
		m_pQueue->Create(nBufferSize, nExtraBuffeSize, nRecvOffset);
		m_bComplete = true;
	}
	inline void Clear()
	{
		m_pQueue->Clear();
		m_bComplete = true;
	}

	inline void Completion(int nBytesSend)
	{
		m_pQueue->Dequeue(NULL, nBytesSend);
		m_bComplete = true;
	}

	inline bool	GetSendParam(char **ppSendPtr, int &nLength)
	{
		if (!IsReadyToSend())
		{
			nLength = 0;
			return false;
		}
		*ppSendPtr	= m_pQueue->GetReadPtr();
		nLength		= m_pQueue->GetReadableLen();
		m_bComplete = false;
		return true;
	}

	inline bool Write(char *pMsg, unsigned int nSize)
	{
		if (!m_pQueue->Enqueue(pMsg, nSize)) return false;
		return true;
	}

	inline unsigned int GetLength()
	{
		return m_pQueue->GetLength();
	}
	inline unsigned int GetSpace()
	{
		return m_pQueue->GetSpace();
	}

public:
	inline bool			IsReadyToSend()
	{
		return (m_bComplete && m_pQueue->GetLength() > 0) ? true : false;
	}

public:
	bool				m_bComplete;
	CircuitQueue<char>	*m_pQueue;
};

#endif // __SendBuf_H__
