/*
 * @Name : imgprocessing.c
 * @Description : Image Processing in C
 * @Date : 2023. 9. 12
 * @Revision : 0.4
 * 0.1 : inverse
 * 0.2 : brightness, contrast
 * 0.3 : histogram, gonzales method, binalization
 * 0.4 : histogram stretching, histogram equlization
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

/*
 * @Function Name : GenerateHistogram
 * @Descriotion : 입력 이미지에 대한 히스토그램을 버퍼에 출력
 * @Input : *Input, nWidth, nHeight, dContrast
 * @Output : *Histogram
 */
void GenerateHistogram(BYTE* Input, int* Histogram, int nWidth, int nHeight)
{
	int nImgSize = nWidth * nHeight;

	for (int i = 0; i < nImgSize; i++)
		Histogram[Input[i]]++;

	return;
}

/*
 * @Function Name : GenerateBinarization
 * @Descriotion : bThreshold 값을 임계값으로 하여 이진화를 진행
 * @Input : *Input, nWidth, nHeight, bThreshold
 * @Output : *Output
 */
void GenerateBinarization(BYTE* Input, BYTE* Output, int nWidth, int nHeight, BYTE bThreshold)
{
	int nImgSize = nWidth * nHeight;

	for (int i = 0; i < nImgSize; i++)
		if (Input[i] < bThreshold)
			Output[i] = 0;
		else
			Output[i] = 255;

	return;
}

/*
 * @Function Name : GonzalezMethod
 * @Descriotion : Gonzalez, Wodds Method에 따라 최적의 이진화 임계값을 계산
 * @Input : *Histogram
 * @Output : bThreshold
 */
BYTE GonzalezMethod(int* Histogram)
{

	BYTE bLow = 0, bHigh = 0;
	BYTE bThreshold, bNewThreshold;
	BYTE e = 2; // 오차값 설정

	int nG1 = 0, nG2 = 0, nCntG1 = 0, nCntG2 = 0, nMeanG1, nMeanG2;

	// 초기 Threshold를 영상에서 어두운 값으로 설정
	for (int i = 0; i < 256; i++) {
		if (Histogram[i] != 0) {
			bLow = i;
			break;
		}
	}

	// 초기 Threshold를 영상에서 가장 밝은 값
	for (int i = 255; i >= 0; i--) {
		if (Histogram[i] != 0) {
			bHigh = i;
			break;
		}
	}

	// 1. Threshold의 초기값을 추정: 최소값 + 최대값 / 2
	bThreshold = (bLow + bHigh) / 2;
	printf("---------------------------\n");
	printf("Initial Threshold = %d\n", bThreshold);


	// 2~4번을 e보다 작을때까지 반복 : e = 2로 설정
	while (1) {

		// 2. Threshold를 기준으로 영상을 분할
		// Threshold 보다 작은 값들을 G1에 추가
		for (int i = bLow; i <= bThreshold; i++) {
			nG1 += (Histogram[i] * i);
			nCntG1 += Histogram[i];
		} // Threshold 를 G1에 설정

		// Threshold 보다 큰 값들을 G2에 추가
		for (int i = bThreshold + 1; i <= bHigh; i++) {
			nG2 += (Histogram[i] * i);
			nCntG2 += Histogram[i];
		}

		// 오류처리 : 0으로 나누기 오류
		if (0 == nCntG1) nCntG1 = 1;
		if (0 == nCntG2) nCntG2 = 1;

		// 3. G1, G2의 밝기 평균값 계산
		nMeanG1 = nG1 / nCntG1;
		nMeanG2 = nG2 / nCntG2;

		// 4.  새로운 임계값을 계산
		bNewThreshold = (nMeanG1 + nMeanG2) / 2;

		// 5. 오차가 3보다 작은지 검사 
		if (abs(bNewThreshold - bThreshold) < e)
		{
			// 오차 범위내 Threshold 최종 결정
			bThreshold = bNewThreshold;
			break;
		}
		else {
			bThreshold = bNewThreshold;
			printf("New Threshold = %d\n", bNewThreshold);

		}

		// 반복을 위해 변수를 0으로 초기화
		nG1 = nG2 = nCntG1 = nCntG2 = 0;
	}

	printf("Last Threshold = %d\n", bThreshold);
	return bThreshold;
}

/*
 * @Function Name : HistogramStretching
 * @Descriotion : 히스토그램 스트래칭을 수행
 * @Input : *Input, *Histigrnam, nWidth, nHeight
 * @Output : *Output
 */
void HistogramStretching(BYTE* Input, BYTE* Output, int* Histogram, int nWidth, int nHeight)
{
	int ImgSize = nWidth * nHeight;
	BYTE Low, High;
	for (int i = 0; i < 256; i++) {
		if (Histogram[i] != 0) {
			Low = i;
			break;
		}
	}
	for (int i = 255; i >= 0; i--) {
		if (Histogram[i] != 0) {
			High = i;
			break;
		}
	}
	for (int i = 0; i < ImgSize; i++) {
		Output[i] = (BYTE)((Input[i] - Low) / (double)(High - Low) * 255.0);
	}

	return;
}

/*
 * @Function Name : HistogramEqualization
 * @Descriotion : 히스토그램 평활화를 수행
 * @Input : *Input, *Histigrnam, nWidth, nHeight
 * @Output : *Output
 */
void HistogramEqualization(BYTE* Input, BYTE* Output, int* Histogram, int nWidth, int nHeight)
{
	int ImgSize = nWidth * nHeight;

	int Nt = ImgSize;
	int Gmax = 255;

	double Ratio = Gmax / (double)Nt;
	BYTE NormSum[256];

	int AHistogram[256] = { 0, };

	for (int i = 0; i < 256; i++) {
		for (int j = 0; j <= i; j++) {
			AHistogram[i] += Histogram[j];
		}
	}

	for (int i = 0; i < 256; i++) {
		NormSum[i] = (BYTE)(Ratio * AHistogram[i]);
	}
	for (int i = 0; i < ImgSize; i++)
	{
		Output[i] = NormSum[Input[i]];
	}

	return;
}

/*
 * @Function Name : main
 * @Descriotion : Image Processing main 함수로 switch 문에 따라 함수를 호출하여 기능을 수행
 * @Input :
 * @Output :
 */
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

	// ver 0.3 변수 추가
	int nHisto[256] = { 0, };
	BYTE bThreshold;
	int nThreshold = 0;			// threshold를 입력

	int nMode = 0;					// 기능 선택
	CHAR PATH[256] = { 0, };		// 파일 경로

	// 사용자 입력
	printf("=================================\n\n");
	printf("Image Processing Program\n\n");
	printf("1.  Inverse Image\n");
	printf("2.  Adjust Brightness\n");
	printf("3.  Adjust Contrast\n");
	printf("4.  Generate Histogram\n");
	printf("5.  Generate Binarization - Gonzalez Method\n");
	printf("6.  Generate Binarization\n");
	printf("7.  Histogram Stretching\n");
	printf("8.  Histogram Equalization\n\n");
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
			free(Input);
			free(Output);
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
			free(Input);
			free(Output);
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
			free(Input);
			free(Output);
			return;
		}

		break;

	case 4:
		// Histogram 생성
		GenerateHistogram(Input, nHisto, hInfo.biWidth, hInfo.biHeight);

		// 히스토그램 값을 화면에 출력
		for (int i = 0; i < 256; i++)
			printf("%d, %d\n", i, nHisto[i]);

		free(Input);
		free(Output);
		return;

	case 5:
		// Histogram 생성
		GenerateHistogram(Input, nHisto, hInfo.biWidth, hInfo.biHeight);

		// Gonzales Method로 threshold를 결정
		bThreshold = GonzalezMethod(nHisto);

		// 이진화 진행
		GenerateBinarization(Input, Output, hInfo.biWidth, hInfo.biHeight, bThreshold);

		nErr = fopen_s(&fp, "../gonzalez_binarization.bmp", "wb");
		if (NULL == fp) {
			printf("Error : file open error = %d\n", nErr);
			free(Input);
			free(Output);
			return;
		}

		break;
			
	case 6:
		printf("이진화 임계값(Threshold)를 입력하세요 : ");
		scanf_s("%d", &nThreshold);

		GenerateBinarization(Input, Output, hInfo.biWidth, hInfo.biHeight, (BYTE)nThreshold);

		nErr = fopen_s(&fp, "../binarization.bmp", "wb");
		if (NULL == fp) {
			printf("Error : file open error = %d\n", nErr);
			free(Input);
			free(Output);
			return;
		}

		break;

	case 7:
		// Histogram 생성
		GenerateHistogram(Input, nHisto, hInfo.biWidth, hInfo.biHeight);

		// 히스토그램 스트래칭 진행
		HistogramStretching(Input, Output, nHisto, hInfo.biWidth, hInfo.biHeight);

		nErr = fopen_s(&fp, "../stretching.bmp", "wb");
		if (NULL == fp) {
			printf("Error : file open error = %d\n", nErr);
			free(Input);
			free(Output);
			return;
		}

		break;

	case 8:
		// Histogram 생성
		GenerateHistogram(Input, nHisto, hInfo.biWidth, hInfo.biHeight);

		// 히스토그램 평활화 진행
		HistogramEqualization(Input, Output, nHisto, hInfo.biWidth, hInfo.biHeight);

		nErr = fopen_s(&fp, "../equalization.bmp", "wb");
		if (NULL == fp) {
			printf("Error : file open error = %d\n", nErr);
			free(Input);
			free(Output);
			return;
		}

		break;


	default:
		printf("입력 값이 잘못되었습니다.\n");
		free(Input);
		free(Output);
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