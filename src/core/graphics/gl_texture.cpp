/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright (c) 2006-2018 Mikhail Kurinnoi, Viewizard


	AstroMenace is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	AstroMenace is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with AstroMenace. If not, see <http://www.gnu.org/licenses/>.


	Web Site: http://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

// TODO translate comments

// NOTE GL_EXT_direct_state_access (since OpenGL 4.5)
//      glTextureStorage2DEXT() + glTextureSubImage2DEXT() + glGenerateTextureMipmapEXT()
//      could be used with texture's name directly, without bind target

// NOTE GL_EXT_direct_state_access (since OpenGL 4.5)
//      glBindTextureUnitEXT()
//      could be used to replace glActiveTexture() + glBindTexture()

#include "../texture/texture.h"
#include "graphics.h"

extern PFNGLACTIVETEXTUREARBPROC glActiveTexture_ARB;
extern PFNGLGENERATEMIPMAPPROC glGenerateMipmapEXT;
extern PFNGLTEXSTORAGE2DPROC glTexStorage2DEXT;


//------------------------------------------------------------------------------------
// Создание текстуры
//------------------------------------------------------------------------------------
GLtexture vw_BuildTexture(uint8_t *ustDIB, int Width, int Height, bool MipMap, int Bytes, int CompressionType)
{
	if (!ustDIB)
		return 0;

	GLtexture TextureID{0};

	glGenTextures(1, &TextureID);
	vw_BindTexture(0, TextureID);

	int Format;
	int InternalFormat;

	if (vw_GetDevCaps()->TexturesCompression && (CompressionType > 0)) {
		if (Bytes == 4) {
			Format = GL_RGBA;
			if (vw_GetDevCaps()->TexturesCompressionBPTC && (CompressionType > 1))
				InternalFormat = GL_COMPRESSED_RGBA_BPTC_UNORM_ARB;
			else
				InternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		} else {
			Format = GL_RGB;
			if (vw_GetDevCaps()->TexturesCompressionBPTC && (CompressionType > 1))
				InternalFormat = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB;
			else
				InternalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
		}
	} else {
		if (Bytes == 4) {
			Format = GL_RGBA;
			InternalFormat = GL_RGBA8;
		} else {
			// считаем 4 слоя (фактически их задействуем)
			Format = GL_RGB;
			InternalFormat = GL_RGB8;
		}
	}

	if (MipMap) {
		// используем по порядку наиболее новые решения при генерации мипмепов
		if (glGenerateMipmapEXT && glTexStorage2DEXT) {
			// считаем сколько нужно создавать мипмапов
			int NeedMipMapLvls = floor(log2(Width>Height?Width:Height)) + 1;
			glTexStorage2DEXT(GL_TEXTURE_2D, NeedMipMapLvls, InternalFormat, Width, Height);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Width, Height, Format, GL_UNSIGNED_BYTE, ustDIB);
			glGenerateMipmapEXT(GL_TEXTURE_2D);
		} else if (glGenerateMipmapEXT) {
			glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, Width, Height, 0, Format, GL_UNSIGNED_BYTE, ustDIB);
			glGenerateMipmapEXT(GL_TEXTURE_2D);
		} else if (vw_GetDevCaps()->HardwareMipMapGeneration) {
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
			glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, Width, Height, 0, Format, GL_UNSIGNED_BYTE, ustDIB);
		} else
			gluBuild2DMipmaps(GL_TEXTURE_2D, InternalFormat, Width, Height, Format, GL_UNSIGNED_BYTE, ustDIB);
	} else { // без мипмепов
		if (glTexStorage2DEXT) {
			glTexStorage2DEXT(GL_TEXTURE_2D, 1, InternalFormat, Width, Height);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Width, Height, Format, GL_UNSIGNED_BYTE, ustDIB);
		} else
			glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, Width, Height, 0, Format, GL_UNSIGNED_BYTE, ustDIB);
	}

	return TextureID;
}

//------------------------------------------------------------------------------------
// Bind
//------------------------------------------------------------------------------------
void vw_BindTexture(uint32_t Stage, GLtexture TextureID)
{
	if (glActiveTexture_ARB)
		glActiveTexture_ARB(GL_TEXTURE0 + Stage);

	if (TextureID) {
		glBindTexture(GL_TEXTURE_2D, TextureID);
		glEnable(GL_TEXTURE_2D);
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
	}
}

//------------------------------------------------------------------------------------
// Delete
//------------------------------------------------------------------------------------
void vw_DeleteTexture(GLtexture TextureID)
{
	glDeleteTextures(1, &TextureID);
}

//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_SetTextureBlendMode(int pname, int param)
{
	GLenum  cmd = GL_COMBINE_RGB;
	GLenum  arg = GL_REPLACE;

	switch (pname) {
	case RI_TBLEND_COLOROP:
		cmd = GL_COMBINE_RGB;
		break;
	case RI_TBLEND_ALPHAOP:
		cmd = GL_COMBINE_ALPHA;
		break;
	case RI_TBLEND_ALPHAARG1:
		cmd = GL_OPERAND0_ALPHA;
		break;
	case RI_TBLEND_ALPHAARG2:
		cmd = GL_OPERAND1_ALPHA;
		break;
	case RI_TBLEND_ALPHAARG3:
		cmd = GL_OPERAND2_ALPHA;
		break;
	case RI_TBLEND_COLORARG1:
		cmd = GL_SOURCE0_RGB;
		break;
	case RI_TBLEND_COLORARG2:
		cmd = GL_SOURCE1_RGB;
		break;
	case RI_TBLEND_COLORARG3:
		cmd = GL_SOURCE2_RGB;
		break;
	default:
		std::cerr << __func__ << "(): " << "wrong pname.\n";
		return;
	}

	switch (param) {
	case RI_TBLEND_SOURCE1:
		arg = GL_REPLACE;
		break;
	case RI_TBLEND_SOURCE2:
		arg = GL_REPLACE;
		break;
	case RI_TBLEND_MODULATE:
		arg = GL_MODULATE;
		break;
	case RI_TBLEND_MODULATE2X:
		arg = GL_MODULATE;
		break;
	case RI_TBLEND_MODULATE4X:
		arg = GL_MODULATE;
		break;
	case RI_TBLEND_ADD:
		arg = GL_ADD;
		break;
	case RI_TBLEND_ADDSMOOTH:
		arg = GL_ADD;
		break;
	case RI_TBLEND_ADD_SIGNED:
		arg = GL_ADD_SIGNED;
		break;
	case RI_TBLEND_SUBTRACT:
		arg = GL_SUBTRACT;
		break;
	case RI_TBLEND_DIFFUSE_ALPHA:
	case RI_TBLEND_TEXTURE_ALPHA:
	case RI_TBLEND_CURRENT_ALPHA:
		arg = GL_INTERPOLATE;
		break;
	case RI_TBLEND_DOTPRODUCT:
		arg = GL_DOT3_RGB;
		break;
	case RI_TBLEND_CURRENT:
		arg = GL_PREVIOUS;
		break;
	case RI_TBLEND_TEXTURE:
		arg = GL_TEXTURE;
		break;
	case RI_TBLEND_CONSTANT:
		arg = GL_CONSTANT;
		break;
	case RI_TBLEND_DIFFUSE:
		arg = GL_PRIMARY_COLOR;
		break;
	case RI_TBLEND_SPECULAR:
		arg = 0;
		break;
	default:
		std::cerr << __func__ << "(): " << "wrong param.\n";
		return;
	}

	// работаем с MODULATE
	switch (param) {
	case RI_TBLEND_MODULATE2X:
		if (pname == RI_TBLEND_COLOROP) glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 2);
		else glTexEnvi(GL_TEXTURE_ENV, 	GL_ALPHA_SCALE, 2);
		break;
	case RI_TBLEND_MODULATE4X:
		if (pname == RI_TBLEND_COLOROP) glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 4);
		else glTexEnvi(GL_TEXTURE_ENV, 	GL_ALPHA_SCALE, 4);
		break;
	default:
		if (pname == RI_TBLEND_COLOROP) glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 1);
		else glTexEnvi(GL_TEXTURE_ENV, 	GL_ALPHA_SCALE, 1);
		break;
	}

	glTexEnvi(GL_TEXTURE_ENV, cmd, arg);
}

//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_SetTextureFiltering(int nFiltering)
{
	// перебираем по мип-меп фильтру
	switch (nFiltering & RI_MIPFILTER) {
	case RI_MIPFILTER_NONE:
		if ((nFiltering & RI_MINFILTER) == RI_MINFILTER_POINT)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		if ((nFiltering & RI_MINFILTER) == RI_MINFILTER_LINEAR)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		break;
	case RI_MIPFILTER_POINT:
		if ((nFiltering & RI_MINFILTER) == RI_MINFILTER_POINT)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		if ((nFiltering & RI_MINFILTER) == RI_MINFILTER_LINEAR)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		break;
	case RI_MIPFILTER_LINEAR:
		if ((nFiltering & RI_MINFILTER) == RI_MINFILTER_POINT)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		if ((nFiltering & RI_MINFILTER) == RI_MINFILTER_LINEAR)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		break;
	default:
		std::cerr << __func__ << "(): " << "wrong nFiltering.\n";
		return;
	}

	// ставим MAG фильтр
	if ((nFiltering & RI_MAGFILTER) == RI_MAGFILTER_POINT)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	if ((nFiltering & RI_MAGFILTER) == RI_MAGFILTER_LINEAR)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_SetTextureAnisotropy(int AnisotropyLevel)
{
	// ставим ANISOTROPY
	if ((AnisotropyLevel > 1) &&
	    (vw_GetDevCaps()->MaxAnisotropyLevel > 1)) {
		if (AnisotropyLevel > vw_GetDevCaps()->MaxAnisotropyLevel)
			AnisotropyLevel = vw_GetDevCaps()->MaxAnisotropyLevel;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, AnisotropyLevel);
	}
}

//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_SetTextureAddressMode(int nAddressMode)
{
	if ((nAddressMode & RI_WRAP_U) == RI_WRAP_U)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // не ставить просто GL_CLAMP, нвидия все равно ставит GL_CLAMP_TO_EDGE

	if ((nAddressMode & RI_WRAP_V) == RI_WRAP_V)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // не ставить просто GL_CLAMP, нвидия все равно ставит GL_CLAMP_TO_EDGE
}

//------------------------------------------------------------------------------------
// Ставим-убираем альфа тест
//------------------------------------------------------------------------------------
void vw_SetTextureAlphaTest(bool Flag, float Value)
{
	if (Flag) {
		glAlphaFunc(GL_GREATER, Value);
		glEnable(GL_ALPHA_TEST);
	} else {
		glAlphaFunc(GL_GREATER, 1.0f);
		glDisable(GL_ALPHA_TEST);
	}
}

//------------------------------------------------------------------------------------
// Режим прозрачности
//------------------------------------------------------------------------------------
void vw_SetTextureBlend(bool Flag, int Src, int Dst)
{
	if (!Flag) {
		glDisable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ZERO);
		return;
	}

	int tmpSRC,tmpDST;

	switch(Src) {
	case RI_BLEND_ZERO:
		tmpSRC = GL_ZERO;
		break;
	case RI_BLEND_ONE:
		tmpSRC = GL_ONE;
		break;
	case RI_BLEND_SRCCOLOR:
		tmpSRC = GL_SRC_COLOR;
		break;
	case RI_BLEND_INVSRCCOLOR:
		tmpSRC = GL_ONE_MINUS_SRC_COLOR;
		break;
	case RI_BLEND_INVSRCALPHA:
		tmpSRC = GL_ONE_MINUS_SRC_ALPHA;
		break;
	case RI_BLEND_DESTALPHA:
		tmpSRC = GL_DST_ALPHA;
		break;
	case RI_BLEND_INVDESTALPHA:
		tmpSRC = GL_ONE_MINUS_DST_ALPHA;
		break;
	case RI_BLEND_SRCALPHA:
		tmpSRC = GL_SRC_ALPHA;
		break;
	case RI_BLEND_DESTCOLOR:
		tmpSRC = GL_DST_COLOR;
		break;
	case RI_BLEND_INVDESTCOLOR:
		tmpSRC = GL_ONE_MINUS_DST_COLOR;
		break;
	case RI_BLEND_SRCALPHASAT:
		tmpSRC = GL_SRC_ALPHA_SATURATE;
		break;
	default:
		tmpSRC = GL_ONE;
		break;
	}

	switch(Dst) {
	case RI_BLEND_ZERO:
		tmpDST = GL_ZERO;
		break;
	case RI_BLEND_ONE:
		tmpDST = GL_ONE;
		break;
	case RI_BLEND_SRCCOLOR:
		tmpDST = GL_SRC_COLOR;
		break;
	case RI_BLEND_INVSRCCOLOR:
		tmpDST = GL_ONE_MINUS_SRC_COLOR;
		break;
	case RI_BLEND_INVSRCALPHA:
		tmpDST = GL_ONE_MINUS_SRC_ALPHA;
		break;
	case RI_BLEND_DESTALPHA:
		tmpDST = GL_DST_ALPHA;
		break;
	case RI_BLEND_INVDESTALPHA:
		tmpDST = GL_ONE_MINUS_DST_ALPHA;
		break;
	case RI_BLEND_SRCALPHA:
		tmpDST = GL_SRC_ALPHA;
		break;
	case RI_BLEND_DESTCOLOR:
		tmpDST = GL_DST_COLOR;
		break;
	case RI_BLEND_INVDESTCOLOR:
		tmpDST = GL_ONE_MINUS_DST_COLOR;
		break;
	case RI_BLEND_SRCALPHASAT:
		tmpDST = GL_SRC_ALPHA_SATURATE;
		break;
	default:
		tmpDST = GL_ZERO;
		break;
	}

	glEnable(GL_BLEND);
	glBlendFunc(tmpSRC, tmpDST);
}

//------------------------------------------------------------------------------------
// установка режима и функции сравнения
//------------------------------------------------------------------------------------
void vw_SetTextureCompare(int MODE, int FUNC)
{
	switch(MODE) {
	case RI_COMPARE_R_TO_TEXTURE:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		break;
	case RI_COMPARE_NONE:
	default:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		break;
	}

	switch(FUNC) {
	case RI_LESSEQUAL:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		break;
	case RI_GREATEREQUAL:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_GEQUAL);
		break;
	case RI_LESS:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
		break;
	case RI_GREATER:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_GREATER);
		break;
	case RI_EQUAL:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_EQUAL);
		break;
	case RI_NOTEQUAL:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_NOTEQUAL);
		break;
	case RI_ALWAYS:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_ALWAYS);
		break;
	case RI_NEVER:
	default:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_NEVER);
		break;
	}
}

//------------------------------------------------------------------------------------
// установка режима работы с компонентом глубины
//------------------------------------------------------------------------------------
void vw_SetTextureDepthMode(int MODE)
{
	switch(MODE) {
	case RI_DEPTH_TEXTURE_MODE_LUMINANCE:
		glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
		break;
	case RI_DEPTH_TEXTURE_MODE_INTENSITY:
		glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
		break;
	case RI_DEPTH_TEXTURE_MODE_ALPHA:
	default:
		glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_ALPHA);
		break;
	}
}

//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_SetTextureEnvMode(int MODE)
{
	switch (MODE) {
	case RI_TENV_DECAL:
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		break;
	case RI_TENV_BLEND:
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
		break;
	case RI_TENV_REPLACE:
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		break;
	case RI_TENV_ADD:
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
		break;
	case RI_TENV_COMBINE:
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
		break;
	case RI_TENV_MODULATE:
	default:
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		break;
	}
}
