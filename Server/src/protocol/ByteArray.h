#ifndef __BYTE_ARRAY_H__
#define __BYTE_ARRAY_H__
#include <string>

#pragma pack(push)
#pragma pack(1)
class ByteArray
{
public:
	ByteArray(void);
	virtual ~ByteArray(void);

	ByteArray(unsigned int length);
	ByteArray(void* bytes, unsigned int length);
	ByteArray(const ByteArray& ba);

	unsigned int		position;
	inline unsigned int	getSize() { return size; }
	void				truncate();
	void				resize(unsigned int newSize);

	bool				readBoolean();
	char				readByte();
	unsigned char		readUnsignedByte();
	short				readShort();
	unsigned short		readUnsignedShort();
	int					readInt();
	unsigned int		readUnsignedInt();
	float				readFloat();
	double				readDouble();
	std::string			readString();
	void				readBytes(ByteArray& ba, unsigned int offset = 0, unsigned int length = 0);
	void				readObject(void* pBuffer, unsigned int size = 0);

	void				writeBoolean(bool b);
	void				writeByte(char b);
	void				writeUnsignedByte(unsigned char b);
	void				writeShort(short s);
	void				writeUnsignedShort(unsigned short s);
	void				writeInt(int i);
	void				writeUnsignedInt(unsigned int i);
	void				writeFloat(float f);
	void				writeDouble(double d);
 	void				writeString(const std::string& str);
	void				writeBytes(const ByteArray& ba, unsigned int offset = 0, unsigned int length = 0);
	void				writeObject(const void* obj, unsigned int length);

protected:
	static const unsigned int DEFAULT_SIZE = 100;
	static const unsigned int STEP_SIZE = 100;

	void*				pBytes;		//字节数据
	unsigned int		capacity;	//占用内存大小
	unsigned int		size;	//实际内容大小
	void				checkSize(int needSize);
	template <typename T> void writeType(const T& val);
};
#pragma pack(pop)

#endif