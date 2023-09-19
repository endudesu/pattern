/*
 * @Name : imgprocessing.c
 * @Description : Image Processing in C
 * @Date : 2023. 9. 12
 * @Revision : 0.1
 * 0.1 : inverse
 * 0.2 : brightness, contrast
 * @Author : Howoong Lee, Division of Computer Enginnering, Hoseo Univ.
 */

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

/*
 * @Function Name : InverseImage
 * @Descriotion : Pixel 단위로 밝기값을 Inverse
 * @Input : *Input, nWidth, nHeight
 * @Output : *Output
 */
void InverseImage(BYTE* Input, BYTE* Output, int nWidth, int nHeight)
{
	int nImgSize = nWidth * nHeight;

	// convert
	for (int i = 0; i < nImgSize; i++)
		Output[i] = 255 - Input[i];

	return;

}

/*
 * @Function Name : AdjustBrightness
 * @Descriotion : nBrightness에 설정된 값을 Pixel 단위로 +. - 를 통한 밝기값을 조정
 * @Input : *Input, nWidth, nHeight, nBrightness
 * @Output : *Output
 */
void AdjustBrightness(BYTE* Input, BYTE* Output, int nWidth, int nHeight, int nBrightness)
{
	int nImgSize = nWidth * nHeight;

	for (int i = 0; i < nImgSize; i++)
		if (Input[i] + nBrightness > 255)
			Output[i] = 255;
		else if (Input[i] + nBrightness < 0)
			Output[i] = 0;
		else
			Output[i] = Input[i] + nBrightness;

	return;
}

/*
 * @Function Name : AdjustContrast
 * @Descriotion : dContrast에 설정된 값을 Pixel 단위로 *를 통한 대비값을 조정(기준 값 1)
 * @Input : *Input, nWidth, nHeight, dContrast
 * @Output : *Output
 */
void AdjustContrast(BYTE* Input, BYTE* Output, int nWidth, int nHeight, double dContrast)
{
	int nImgSize = nWidth * nHeight;

	for (int i = 0; i < nImgSize; i++)
		if (Input[i] * dContrast > 255)
			Output[i] = 255;
		else
			Output[i] = (BYTE)(Input[i] * dContrast);

	return;
}


void main()
{
	// BMT Header 구조체 선언
	BITMAPFILEHEADER hf; // BMP 파일헤더 14Bytes
	BITMAPINFOHEADER hInfo; // BMP 인포헤더 40Bytes
	RGBQUAD hRGB[256] = { 0, };// 파레트 (256 * 4Bytes)

	// 변수 선언
	FILE* fp = NULL;				// 파일 포인터
	errno_t nErr = 0;				// Error	
	int nImgSize = 0;				// 이미지 크기

	// ver 0.2 변수 추가
	int nBrigntness = 0;	// 밝기 값
	double dContrast = 0;		// 대비 값

	int nMode = 0;					// 기능 선택
	CHAR PATH[256] = { 0, };		// 파일 경로

	// 사용자 입력
	printf("=================================\n\n");
	printf("Image Processing Program\n\n");
	printf("1.  Inverse Image\n");
	printf("2.  Adjust Brightness\n");
	printf("3.  Adjust Contrast\n\n");
	printf("=================================\n\n");

	printf("원하는 기능의 번호를 입력하세요 : ");
	scanf_s("%d", &nMode);

	printf("원본 이미지 파일의 경로를 입력하세요 : ");
	scanf_s("%s", PATH, sizeof(PATH));

	// 이미지 파일 오픈
	nErr = fopen_s(&fp, PATH, "rb");

	if (NULL == fp) {
		printf("Error : file open error = %d\n", nErr);
		return;
	}

	// BITMAPFILEHEADER
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);

	// BITMAPINFOHEADER
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);

	// RGBQUAD
	fread(hRGB, sizeof(RGBQUAD), 256, fp);


	// 이미지 크기 계산(가로 X 세로)
	nImgSize = hInfo.biWidth * hInfo.biHeight;

	// 원본 이미지와 출력 이미지를 저장할 버퍼 할당
	BYTE* Input = (BYTE*)malloc(nImgSize);
	BYTE* Output = (BYTE*)malloc(nImgSize);

	if (NULL == Input || NULL == Output) {
		printf("Error : memory allocation error\n");
		return;
	}

	// 버퍼 초기화
	memset(Input, 0, nImgSize);
	memset(Output, 0, nImgSize);

	// 
	fread(Input, sizeof(BYTE), nImgSize, fp);
	fclose(fp);

	// nMode에 따라 기능을 계속 추가하면서 진행할 예정임
	switch (nMode) {

	case 1:
		// Inverse
		InverseImage(Input, Output, hInfo.biWidth, hInfo.biHeight);

		nErr = fopen_s(&fp, "../inverse.bmp", "wb");
		if (NULL == fp) {
			printf("Error : file open error = %d\n", nErr);
			return;
		}

		break;

	case 2:
		// brightness
		printf("밝기 조절 값(정수)을 입력하세요 : ");
		scanf_s("%d", &nBrigntness);

		AdjustBrightness(Input, Output, hInfo.biWidth, hInfo.biHeight, nBrigntness);

		nErr = fopen_s(&fp, "../brigntness.bmp", "wb");
		if (NULL == fp) {
			printf("Error : file open error = %d\n", nErr);
			return;
		}

		break;

	case 3:
		// contrast
		printf("대비 조절 값(실수)을 입력하세요 : ");
		scanf_s("%lf", &dContrast);

		AdjustContrast(Input, Output, hInfo.biWidth, hInfo.biHeight, dContrast);

		nErr = fopen_s(&fp, "../contrast.bmp", "wb");
		if (NULL == fp) {
			printf("Error : file open error = %d\n", nErr);
			return;
		}

		break;

	default:
		printf("입력 값이 잘못되었습니다.\n");
		return;

	}

	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output, sizeof(BYTE), nImgSize, fp);
	fclose(fp);

	free(Input);
	free(Output);	

	return;
}