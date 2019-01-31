#ifndef __RecvBuf_H__
#define __RecvBuf_H__

#include "CircuitQueue.h"

class RecvBuffer
{
public:
    RecvBuffer()
    { 
        m_pQueue = NULL; 
    }

    virtual ~RecvBuffer()
    { 
        if( m_pQueue ) 
            delete m_pQueue; 
    }

    inline void	Create(int nBufferSize, unsigned int nExtraBufferSize, unsigned int nRecvOffset)
    {
        if (m_pQueue)
            delete m_pQueue;

        m_pQueue = new CircuitQueue<char>;
        m_pQueue->Create(nBufferSize, nExtraBufferSize, nRecvOffset);
        m_bComplete = true;
    }

    inline void	Completion(int nBytesRecvd) 
    {
        m_pQueue->Enqueue(NULL, nBytesRecvd);
        m_bComplete = true;
    }

    inline void	Clear()
    { 
        m_pQueue->Clear(); 
        m_bComplete = true; 
    }

    inline void	GetRecvParam(char **ppRecvPtr, int &nLength)
    {
        if( !IsReadyToRecv() ) 
        { 
            nLength = 0;
            return;
        }

        *ppRecvPtr	= m_pQueue->GetWritePtr();		
        nLength		= m_pQueue->GetWritableLen();

        m_bComplete = false;
    }

    inline char* GetReadDataPtr(unsigned int& nDataLen)
    {
        int nLen = m_pQueue->GetLength();
        int nBackDataLen = m_pQueue->GetBackDataCount();

        nDataLen = std::min(nLen, nBackDataLen);

        if (nLen > nBackDataLen 
            && nBackDataLen < m_pQueue->m_nExtraSize)
        {
            int nCopyDataLen = nLen - nBackDataLen;
            if (nCopyDataLen > m_pQueue->m_nExtraSize)
            {
                nCopyDataLen = m_pQueue->m_nExtraSize;
            }
            
            nDataLen += nCopyDataLen;
            m_pQueue->CopyHeadDataToExtraBuffer(nCopyDataLen);
        }

        return  m_pQueue->GetReadPtr();
    }

    inline void	RemoveReadData( unsigned int nSize ) 
    { 
        m_pQueue->Dequeue( NULL, nSize ); 
    }

public:
    inline bool			IsReadyToRecv() 
    { 
        return ( m_bComplete && m_pQueue->GetSpace() > 0 ) ? true : false; 
    }

public:
    bool				m_bComplete;
    CircuitQueue<char>	*m_pQueue;
};

#endif // __RecvBuf_H__
