#include "Model.h"
#include <Windows.h>
Model* loadModel(char* filepath){
	FILE *f;
	fopen_s(&f, filepath, "rb");
	if (!f)
		throw std::exception("Can't find file", 0);

	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	char *pdata = new char[fsize + 1];
	fread(pdata, fsize, 1, f);
	fclose(f);
	pdata[fsize] = 0;

	if (memcmp((char*)pdata, "ply", 3))
		throw std::exception("Bad model file format", 1);

	int vsize = atoi(strstr(((char*)pdata), "element vertex") + strlen("element vertex"));
	int fasize = atoi(strstr(((char*)pdata), "element face") + strlen("element face"));
	unsigned vcount = 0;

	float* v = new float[vsize * 3];
	float* n = new float[vsize * 3];
	float* t = new float[vsize * 2];

	int i = strlen("end_header");
	char* nd = strstr((char*)pdata, "end_header");
	for (int j = 0; j < vsize; ++j){
		v[j * 3] = (float)atof(&nd[++i]);
		for (; nd[i] >= '-' && nd[i] <= '9'; ++i);
		v[j * 3 + 1] = (float)atof(&nd[++i]);
		for (; nd[i] >= '-' && nd[i] <= '9'; ++i);
		v[j * 3 + 2] = (float)atof(&nd[++i]);
		for (; nd[i] >= '-' && nd[i] <= '9'; ++i);
		n[j * 3] = (float)atof(&nd[++i]);
		for (; nd[i] >= '-' && nd[i] <= '9'; ++i);
		n[j * 3 + 1] = (float)atof(&nd[++i]);
		for (; nd[i] >= '-' && nd[i] <= '9'; ++i);
		n[j * 3 + 2] = (float)atof(&nd[++i]);
		for (; nd[i] >= '-' && nd[i] <= '9'; ++i);
		t[j * 2] = (float)atof(&nd[++i]);
		for (; nd[i] >= '-' && nd[i] <= '9'; ++i);
		t[j * 2 + 1] = (float)atof(&nd[++i]);
		for (; nd[i] >= '-' && nd[i] <= '9'; ++i);
	}

	int fs = i;
	for (int j = 0; j < fasize; ++j){
		++i;
		if (atoi(&nd[i]) == 4)
			vcount += 6;
		else
			vcount += 3;
		for (; nd[i] != '\n' && nd[i] != 0; ++i);
	}

	unsigned* fa = new unsigned[vcount];
	i = fs;
	for (unsigned j = 0; j < vcount;){
		int a = atoi(&nd[++i]);
		for (; nd[i] >= '0' && nd[i] <= '9'; ++i);
		fa[j] = atoi(&nd[++i]);
		for (; nd[i] >= '0' && nd[i] <= '9'; ++i);
		fa[j + 1] = atoi(&nd[++i]);
		for (; nd[i] >= '0' && nd[i] <= '9'; ++i);
		fa[j + 2] = atoi(&nd[++i]);
		for (; nd[i] >= '0' && nd[i] <= '9'; ++i);
		if (a == 4){
			fa[j + 3] = fa[j];
			fa[j + 4] = fa[j + 2];
			fa[j + 5] = atoi(&nd[++i]);
			for (; nd[i] >= '0' && nd[i] <= '9'; ++i);
			j += 6;
		}
		else
			j += 3;
	}

	Model* model = new Model;
	model->vcount = vcount;
	model->VBOs = new GLuint[4];

	glGenBuffers(4, model->VBOs);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->VBOs[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vcount * 4, fa, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, model->VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, vsize * 3 * 4, v, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, model->VBOs[2]);
	glBufferData(GL_ARRAY_BUFFER, vsize * 3 * 4, n, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, model->VBOs[3]);
	glBufferData(GL_ARRAY_BUFFER, vsize * 2 * 4, t, GL_STATIC_DRAW);

	delete[] fa;
	delete[] v;
	delete[] n;
	delete[] t;

	return model;
}
Model* loadHeightMap(float x, float y, float z, char* path){
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned char* data;

	FILE* f;
	fopen_s(&f, path, "rb");
	if (!f){
		MessageBoxA(0, "Could not found bitmap file", "Resource", 0);
		exit(-1);
	}
	fread(header, 1, 54, f);
	if (header[0] != 'B' || header[1] != 'M'){
		MessageBoxA(0, "Not a correct BMP file", "Resource", 0);
		exit(-1);
	}
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	unsigned width = *(int*)&(header[0x12]);
	unsigned height = *(int*)&(header[0x16]);
	if (imageSize == 0)    imageSize = width*height * 3;
	if (dataPos == 0)      dataPos = 54;
	data = new unsigned char[imageSize];
	fread(data, 1, imageSize, f);
	fclose(f);

	unsigned vcount = (width - 1)*(height - 1) * 6;
	float* v = new float[width*height * 3];
	float* n = new float[width*height * 3];
	float* t = new float[width*height * 2];
	int* fa = new int[vcount];

	for (unsigned j = 0, k = 0; j < height; ++j)
		for (unsigned i = 0; i < width; ++i){
			v[j*width * 3 + i * 3] = float(i) / float(width)*x - x / 2;
			v[j*width * 3 + i * 3 + 1] = float(data[j*(width * 3 + (width % 4)) + i * 3]) / 255.f * z;
			v[j*width * 3 + i * 3 + 2] = float(j) / float(height)*y - y / 2;

			t[j*width * 2 + i * 2] = float(i) / float(width);
			t[j*width * 2 + i * 2 + 1] = float(j) / float(height);
			
			n[j*width * 3 + i * 3] = 0;
			n[j*width * 3 + i * 3 + 1] = 1;
			n[j*width * 3 + i * 3 + 2] = 0;

			if (j*width + i < height*(width - 1) - 1 &&
				((j*width + i + 1) % width) != 0){
				fa[k * 6] = j*width + i;
				fa[k * 6 + 1] = j*width + i + width;
				fa[k * 6 + 2] = j*width + i + 1;
				fa[k * 6 + 3] = j*width + i + 1;
				fa[k * 6 + 4] = j*width + i + width;
				fa[k * 6 + 5] = j*width + i + width + 1;
				++k;
			}
		}

	Model* model = new Model;
	model->vcount = vcount;
	model->VBOs = new GLuint[4];

	glGenBuffers(4, model->VBOs);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->VBOs[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vcount * 4, fa, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, model->VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, height*width * 3 * 4, v, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, model->VBOs[2]);
	glBufferData(GL_ARRAY_BUFFER, height*width * 3 * 4, n, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, model->VBOs[3]);
	glBufferData(GL_ARRAY_BUFFER, height*width * 2 * 4, t, GL_STATIC_DRAW);

	delete[] v;
	delete[] t;
	delete[] n;
	delete[] fa;
	delete[] data;

	return model;
}


Model::~Model(){
	delete[] VBOs;
	glDeleteBuffers(4, VBOs);
}

