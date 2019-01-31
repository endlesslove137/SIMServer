#include "ByteArray.h"
#include <assert.h>
#include <memory.h>

/************************************************************************/
/* ��ȡ�ֽ�������                                                        */
/************************************************************************/

#define READ_TYPE(typeName, pos, totalSize, ptr)\
{\
	size_t typeSize = sizeof(typeName);\
	if (pos + typeSize <= totalSize)\
	{\
		typeName* pT = (typeName*)((long)ptr + pos);\
		pos += typeSize;\
		return *pT;\
	}\
}

ByteArray::ByteArray(void):capacity(DEFAULT_SIZE), size(0), position(0)
{
	pBytes = new char[DEFAULT_SIZE];
	memset(pBytes, 0, DEFAULT_SIZE);
}

ByteArray::ByteArray( unsigned int length ):capacity(length), size(DEFAULT_SIZE), position(0)
{
	if (length > 0)
	{
		pBytes = new char[length];
		memset(pBytes, 0, length);
	}
}

ByteArray::ByteArray( void* bytes, unsigned int length ):capacity(length), size(length), position(0)
{
	pBytes = new char[length];
	memcpy(pBytes, bytes, length);
}

ByteArray::~ByteArray(void)
{
	delete pBytes;
	pBytes = NULL;
}

bool ByteArray::readBoolean()
{
 	READ_TYPE(bool, position, capacity, pBytes)
	return false;
}

char ByteArray::readByte()
{
	READ_TYPE(char, position, capacity, pBytes)
	return 0;
}

unsigned char ByteArray::readUnsignedByte()
{
	READ_TYPE(unsigned char, position, capacity, pBytes)
	return 0;
}

short ByteArray::readShort()
{
	READ_TYPE(short, position, capacity, pBytes)
	return 0;
}

unsigned short ByteArray::readUnsignedShort()
{
	READ_TYPE(unsigned short, position, capacity, pBytes)
	return 0;
}

int ByteArray::readInt()
{
 	READ_TYPE(int, position, capacity, pBytes)
	return 0;
}

unsigned int ByteArray::readUnsignedInt()
{
	READ_TYPE(unsigned int, position, capacity, pBytes)
	return 0;
}

float ByteArray::readFloat()
{
	READ_TYPE(float, position, capacity, pBytes)
	return 0.0f;
}

double ByteArray::readDouble()
{
	READ_TYPE(double, position, capacity, pBytes)
	return 0.0;
}

// std::string ByteArray::readUTF8()
// {
// 	unsigned short length = readUnsignedShort();
// 	return readMutiByte(length, "utf-8");
// }
// 
// std::string ByteArray::readMutiByte( unsigned int length, char* charset )
// {
// 	if (length > 0)
// 	{
// 		if (length > size - position)
// 		{
// 			length = size - position;
// 		}
// 		char* pChar = (char*)((unsigned int)pBytes + position);
// 		return std::string(pChar, length);
// 	}
// 	return std::string("");
// }

void ByteArray::readBytes( ByteArray& ba, unsigned int offset /*= 0*/, unsigned int length /*= 0*/ )
{
	if (length == 0 || length > capacity - position)	//��ȡ���������
	{
		length = capacity - position;
	}
	if (length > ba.capacity - offset)		//д�����������
	{
		length = ba.capacity - offset;
	}
	if (length > 0)
	{
		void* pDst = (void*)((long)ba.pBytes + offset);
		void* pSrc = (void*)((long)pBytes + position);
		memcpy(pDst, pSrc, length);
		position += length;
	}
}

void ByteArray::readObject(void* pBuffer, unsigned int length)
{
	if (pBuffer == NULL)
	{
		return;
	}
	if (length == 0 || length > capacity - position)
	{
		length = capacity - position;
	}
	if (length > 0)
	{
		memcpy(pBuffer, (void*)((long)pBytes + position), length);
		position += length;
	}
}

void ByteArray::resize(unsigned int newSize)
{
	if (newSize > capacity)
	{
		pBytes = realloc(pBytes, newSize);
		assert(pBytes != NULL);
		capacity = newSize;
	}
	else
	{
		capacity = newSize;
		if (position > capacity)
		{
			position = capacity;
		}
	}
}


void ByteArray::checkSize(int needSize)
{
	needSize -= capacity - position;
	int increasedSize(0);
	while (needSize > 0)
	{
		increasedSize += STEP_SIZE;
		needSize -= STEP_SIZE;
	}
	if (increasedSize > 0)
	{
		pBytes = realloc(pBytes, capacity + increasedSize);
		assert(pBytes != NULL);
		capacity += increasedSize;
	}
}
/************************************************************************/
/* write functions                                                      */
/************************************************************************/

template <typename T>
void ByteArray::writeType(const T& val)
{
	checkSize(sizeof(T));
	memcpy((void*)((unsigned long)pBytes + position), &val, sizeof(T));
	position += sizeof(T);
	if (size < position)
	{
		size = position;
	}
}

void ByteArray::writeBoolean( bool b )
{
	writeType(b);
}

void ByteArray::writeByte( char b )
{
	writeType(b);
}

void ByteArray::writeUnsignedByte( unsigned char b )
{
	writeType(b);
}

void ByteArray::writeShort( short s )
{
	writeType(s);
}

void ByteArray::writeUnsignedShort( unsigned short s )
{
	writeType(s);
}

void ByteArray::writeInt( int i )
{
	writeType(i);
}

void ByteArray::writeUnsignedInt( unsigned int i )
{
	writeType(i);
}

void ByteArray::writeFloat( float f )
{
	writeType(f);
}

void ByteArray::writeDouble( double d )
{
	writeType(d);
}

void ByteArray::writeString( const std::string& str )
{
	writeObject(str.c_str(), str.length());
}

void ByteArray::writeBytes(const ByteArray& ba, unsigned int offset /*= 0*/, unsigned int length /*= 0*/ )
{
	if (offset >= ba.capacity)
	{
		return;
	}
	if (length == 0 || offset + length > ba.capacity)
	{
		length = ba.capacity - offset;
	}
	if (length > 0)
	{
		if (position >= capacity - length)
		{
			realloc(pBytes, position + length + 1);
		}
		void* pDst = (void*)((long)pBytes + position);
		void* pSrc = (void*)((long)ba.pBytes + offset);
		memcpy(pDst, pSrc, length);
		position += length;
		if (size < position)
		{
			size = position;
		}
	}
}

void ByteArray::writeObject( const void* obj, unsigned int length )
{
	if (length > 0)
	{
		checkSize(length);
		memcpy((void*)((unsigned long)pBytes + position), obj, length);
		position += length;
		if (size < position)
		{
			size = position;
		}
	}
}

void ByteArray::truncate()
{
	
}
