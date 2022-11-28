#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include "Timer.h"
#include <zstd.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include <chrono>
#include <memory>
#include "timer.h"
#include <windows.h>
//#include <QDebug>
//#include <QDir>
//#include "QDateTime"
//#include <QTextStream>
//#include <QDirIterator>
//#include <QStorageInfo>
//#include <QString>
//#include <lz4file.h>
//#pragma comment(lib, "QT6Core.lib")
using namespace std;

#define TEST_FILE_NAME_1 "data0000000049_144919_872_2.bin"
#define HEADER_SIZE 16
#define DATA_SIZE 25011200

using namespace std;

struct _wgsdata
{
	uint8_t* data;
};

int main()
{
	// simple compress : 1.5s
	//// zstd 압축해보기 // 900ms
	Timer timer;
	timer.start();
    const char* oname = "data0000000049_144919_872_2.zst";
    cout << "TEST_FILE_NAME_1 : " << TEST_FILE_NAME_1;
    size_t fSize;
    void* const fBuff = mallocAndLoadFile_orDie(TEST_FILE_NAME_1, &fSize);
    size_t const cBuffSize = ZSTD_compressBound(fSize);
    void* const cBuff = malloc_orDie(cBuffSize);
	timer.stop();
	cout << "걸린시간 : " << timer.elapsedMilliseconds() << endl;
	timer.start();
    /* Compress.
     * If you are doing many compressions, you may want to reuse the context.
     * See the multiple_simple_compression.c example.
     */
    size_t const cSize = ZSTD_compress(cBuff, cBuffSize, fBuff, fSize, 1);
    CHECK_ZSTD(cSize);
    saveFile_orDie(oname, cBuff, cSize);
	timer.stop();
	cout << "걸린시간 : " << timer.elapsedMilliseconds() << endl;
	timer.start();
    /* success */
    printf("%25s : %6u -> %7u - %s \n", TEST_FILE_NAME_1, (unsigned)fSize, (unsigned)cSize, oname);
	free(fBuff);
	free(cBuff);
	timer.stop();
	cout << "걸린시간 : "<< timer.elapsedMilliseconds() << endl;

	// 압축해제 // 500ms
	timer.stop();
	timer.start();
	const char* zname = "data0000000049_144919_872_1.zst";
	const char* rname = "data0000000049_144919_872_1.bin";
	cout << "압축해제 시작 : " << "zname" << zname << endl;
	size_t csSize;
	void* const csBuff = mallocAndLoadFile_orDie(zname, &csSize);
	cout << "압축해제 : " << "csBuff : " << csBuff << endl;
	unsigned long long const rSize = ZSTD_getFrameContentSize(csBuff, csSize);
	cout << "압축해제 : " << "rSize : " << rSize << endl;
	CHECK(rSize != ZSTD_CONTENTSIZE_ERROR, "%s: not compressed by zstd!", zname);
	CHECK(rSize != ZSTD_CONTENTSIZE_UNKNOWN, "%s: original size unknown!", zname);

	void* const rsBuff = malloc_orDie((size_t)rSize);
	cout << "압축해제 : " << "rsBuff : " << rsBuff << endl;
	/* Decompress.
	 * If you are doing many decompressions, you may want to reuse the context
	 * and use ZSTD_decompressDCtx(). If you want to set advanced parameters,
	 * use ZSTD_DCtx_setParameter().
	 */
	size_t const dSize = ZSTD_decompress(rsBuff, rSize, csBuff, csSize);
	cout << "압축해제 : " << "dSize : " << dSize << endl;
	CHECK_ZSTD(dSize);
	/* When zstd knows the content size, it will error if it doesn't match. */
	CHECK(dSize == rSize, "Impossible because zstd will check this condition!");
	/* success */
	printf("%25s : %6u -> %7u \n", zname, (unsigned)csSize, (unsigned)dSize);
	saveFile_orDie(rname, rsBuff, dSize);
	free(rsBuff);
	free(csBuff);
	timer.stop();
	cout << "걸린시간 : " << timer.elapsedMilliseconds() << endl;
	cout << "압축해제 끝 : " << "oname : " << zname << endl;

    return 0;
}