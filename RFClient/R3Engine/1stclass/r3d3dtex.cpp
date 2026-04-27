#include <stdio.h>
#include "d3d8.h"
#include "ddraw.h"
#include "d3dx8.h"


#include "core.h"
#include "Jmalloc.h"
#include "JError.h"
#include "d3dutil.h"
#include "dxutil.h"
#include "CommonUtil.h"
#include "mergefile.h"
#include "R3d3d8.h"
#include "R3lightmap.h"
#include "R3d3dtex.h"
#include "R3material.h"
#include "r3enginekernel.h"


//#define _NONE_CACHE	//≈ÿΩ∫√ƒ ƒ≥Ω¨∏¶ æ»æ≤∞Ì Ω«Ω√∞£ ∑Œµ˘«“∂ß


typedef struct{
	DWORD dwMagic;// (0x20534444, or "DDS")	//dds∆ƒ¿œ ∆˜∏À¿« æ’¿« 128πŸ¿Ã∆Æ.... ^^
	DDSURFACEDESC2 ddsd;
}_DDS_Header;

static DWORD stNowTexMem=0;
DWORD GetNowTexMemSize()
{
	return stNowTexMem;
}
void ResetTexMemSize()
{
	stNowTexMem=0;
}

int GetMipMapSkipSize(DDSURFACEDESC2 *ddsd,DWORD mip_level,DWORD max_tex_xl,DWORD max_tex_yl)
{
	int skip_size=0;

	DWORD width=ddsd->dwWidth;
	DWORD height=ddsd->dwHeight;

	if( max_tex_xl < 256 || max_tex_yl <256 )
		Error("¿Ã∑∏∞‘ ≈ÿΩ∫√ƒ ¡¶«—¿Ã ¿€¿∫ƒ´µÂ∞°æ¯¥Ÿ...","");

	DWORD real_mip_level=0;
	while( 1 )
	{
		if( real_mip_level >= mip_level && ddsd->dwWidth <= max_tex_xl && ddsd->dwHeight <=max_tex_yl)
			break;
		if( real_mip_level >= ddsd->dwMipMapCount )
			break;

		skip_size += ddsd->lPitch;
		if( ddsd->dwHeight > 4 )
			ddsd->dwHeight/=2;
		else
			break;
		if( ddsd->dwWidth > 4 )
			ddsd->dwWidth/=2;
		else
			break;
		ddsd->lPitch/=4;
		real_mip_level++;
	}
	ddsd->dwMipMapCount-=real_mip_level;

	return skip_size;
}

//DDS ¿–¿ª∂ß «Ï¥ı∞° ¿ﬂ∏¯µ«º≠ ∏¯¿–¥¬ πˆ±◊∏¶ ¿‚¥¬¥Ÿ.
static void FixDDSHeader(BYTE *buf)
{
	_DDS_Header *header=(_DDS_Header *)buf;

	DWORD tempd2 = header->ddsd.ddpfPixelFormat.dwFlags;
	if( tempd2 == 5 )
		header->ddsd.ddpfPixelFormat.dwFlags=4;
	if( tempd2 == 0x41 )
	{
		if( header->ddsd.ddpfPixelFormat.dwRGBBitCount == 16 )
		{
			if( header->ddsd.ddpfPixelFormat.dwRGBAlphaBitMask != 0x00008000)	//1555∆˜∏À¿Ã æ∆¥—∞ÊøÏ.
				header->ddsd.ddpfPixelFormat.dwRBitMask = 0x00000f00;
		}
		else
		if( header->ddsd.ddpfPixelFormat.dwRGBBitCount == 32 )
		{
			header->ddsd.ddpfPixelFormat.dwRBitMask=0x00ff0000;
			header->ddsd.ddpfPixelFormat.dwGBitMask=0x0000ff00;
			header->ddsd.ddpfPixelFormat.dwBBitMask=0x000000ff;
			header->ddsd.ddpfPixelFormat.dwRGBAlphaBitMask=0xff000000;
		}
	}
	if( header->ddsd.ddpfPixelFormat.dwSize == 0 )
	{
		header->ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
		header->ddsd.ddpfPixelFormat.dwFlags = 4;
	}
}

BYTE pass_word[137]=".ÄMv.¯—Ω?ÜÅX,??..go?@?x<?Ò¿•ˆ;ü¡ ?◊»¡ÈÖÜΩÔV?°˚.áÜaL!;N¥xWÆó?.J.?L.DÕ≈_ËÈÏÎΩæª˜l.Ú‰.??óüù≥!πveT?Êˆ∆y€‚≤K..Î”” ´Í«Ìú«Ÿ–eH¥˙5..jõØ~÷∑y";

static void UnLockDDS(DWORD *buf)	//DDS∆ƒ¿œ æœ»£»≠ Ω√≈∞¿⁄.
{
	DWORD i;
	DWORD *d_pass=(DWORD *)pass_word;

	for(i=0; i<32; i++)
		buf[i]^=d_pass[i];
}

LPDIRECT3DTEXTURE8 R3LoadDDSFromFP(FILE *fp,DWORD file_total_size,DWORD mipmap,DWORD max_tex_xl=2048,DWORD max_tex_yl=2048)	//mipmap¿Ã 0¿Œ∞ÊøÏ ø¯∑° ªÁ¿Ã¡Ó∏¶ 1¿Œ∞ÊøÏ ±◊¥„¥‹∞Ë∫Œ≈Õ ¿–æÓø¿¥¬ ∞ÊøÏ
{
	LPDIRECT3DTEXTURE8 lptexture=NULL;
	_DDS_Header *header;

	int spare_size,skip_size;

	BYTE *temp=(BYTE *)malloc(file_total_size);
	fread(temp,sizeof(_DDS_Header),1,fp);
	header = (_DDS_Header *)temp;
	if( header->dwMagic != 0x20534444 )
	{
		UnLockDDS((DWORD *)temp);	//«Ï¥ı æœ»£∏¶ «ÆæÓ∫ª¥Ÿ.
		if( header->dwMagic != 0x20534444 )	//±◊∑°µµ æ∆¥œ∏È DDS∞° æ∆¥œ¥Ÿ.
		{
			//DDS∞° æ∆¥œ¥Ÿ..
			free(temp);
			return NULL;
		}
	}
	
	skip_size=0;
	if( header->ddsd.dwMipMapCount > 0 )	//π”∏ ¿Ã ¡∏¿Á«ÿæﬂ ∞°¥…«œ¥Ÿ.
	{
		skip_size=GetMipMapSkipSize(&header->ddsd,mipmap,max_tex_xl,max_tex_yl);
		if( skip_size )
			fseek(fp,skip_size,SEEK_CUR);	//æ» ¿–¥¬∫Œ∫–¿ª ∞«≥  ∂⁄¥Ÿ.
	}
	spare_size = file_total_size - sizeof(_DDS_Header) - skip_size;

	fread((temp+sizeof(_DDS_Header)),spare_size,1,fp);//« ø‰«—∫Œ∫–¿ª ±∏«—¥Ÿ.

	FixDDSHeader(temp);

	HRESULT hr = D3DXCreateTextureFromFileInMemory( GetD3dDevice(),
		temp, spare_size+sizeof(_DDS_Header),&lptexture);
/*
	if( hr == D3DXERR_INVALIDDATA )
	{
		BYTE *temp_p=(BYTE *)malloc(file_total_size);

		FILE *fp = fopen("error.dds","wb");
		fwrite(temp,file_total_size,1,fp);
		fclose(fp);

		fp = fopen("error_f.dds","rb");
		fread(temp_p,file_total_size,1,fp);
		fclose(fp);
		memcpy(header,temp_p,sizeof(_DDS_Header));		
		hr = D3DXCreateTextureFromFileInMemory( GetD3dDevice(),
		temp, spare_size+sizeof(_DDS_Header),&lptexture);
	}
*/
	free(temp);
	if( FAILED( hr ) )
	{
		return NULL;
	}
	stNowTexMem += spare_size;
	return lptexture;
}
LPDIRECT3DTEXTURE8 R3LoadDDSAndTextureMem(char *name);

LPDIRECT3DTEXTURE8 R3LoadDDS(char *name,DWORD mipmap,DWORD max_tex_xl=2048,DWORD max_tex_yl=2048)	//mipmap¿Ã 0¿Œ∞ÊøÏ ø¯∑° ªÁ¿Ã¡Ó∏¶ 1¿Œ∞ÊøÏ ±◊¥„¥‹∞Ë∫Œ≈Õ ¿–æÓø¿¥¬ ∞ÊøÏ
{
	LPDIRECT3DTEXTURE8 lptexture=NULL;

	int total_size = GetFileSizeAndMergeFile(name);		//∆ƒ¿œ ªÁ¿Ã¡Ó∏¶ æÀæ∆≥Ω¥Ÿ.
	FILE *fp = fopenMFM(name,"rb");
	if( fp == NULL )
		return NULL;

	lptexture=R3LoadDDSFromFP(fp, total_size, mipmap, max_tex_xl, max_tex_yl);

	if( lptexture== NULL )	//dds∞° æ∆¥—∞©¥Ÿ...
	{
		lptexture=R3LoadDDSAndTextureMem(name);
	}

	fclose(fp);
	return lptexture;
}


LPDIRECT3DTEXTURE8 GetD3DTextureFromBuffer(BYTE *buf,DWORD size,DWORD mipmap)	//mipmap¿Ã 0¿Œ∞ÊøÏ ø¯∑° ªÁ¿Ã¡Ó∏¶ 1¿Œ∞ÊøÏ ±◊¥„¥‹∞Ë∫Œ≈Õ ¿–æÓø¿¥¬ ∞ÊøÏ
{
	LPDIRECT3DTEXTURE8 lptexture=NULL;
	HRESULT hr = D3DXCreateTextureFromFileInMemory( GetD3dDevice(),
		buf, size,&lptexture);

	return lptexture;
}


//-----------------------------------------------------------------
LPDIRECT3DTEXTURE8 R3LoadDDSAndTextureMem(char *name)
{
	LPDIRECT3DTEXTURE8 lptexture=NULL;
	HRESULT hr;


	_R3ENGINE_STATE *state = GetR3State();
	if( state->mBestTextureFormat == D3DFMT_DXT1 )	//∏∏æ‡ dxt1¿ª ¡ˆø¯«œ∏È.
	{
		D3DXIMAGE_INFO       SrcInfo;
		hr = D3DXCreateTextureFromFileEx( GetD3dDevice(), name, 
					D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, 
					D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
					D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 0, &SrcInfo, NULL, &lptexture );
/*
		int size = GetFileSize(name);		//∆ƒ¿œ ªÁ¿Ã¡Ó∏¶ æÀæ∆≥Ω¥Ÿ.
		BYTE *temp=(BYTE *)Dmalloc(size);

		FILE *fp = fopen(name,"rb");
		fread(temp,size,1,fp);
		fclose(fp);

		hr = D3DXCreateTextureFromFileInMemory( GetD3dDevice(),
			temp, size,&lptexture);
		Dfree(temp);
*/
	}
	else
	{
		hr = D3DXCreateTextureFromFileEx( GetD3dDevice(), name, 
					D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, state->mBestTextureFormat, 
					D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
					D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 0, NULL, NULL, &lptexture );
	}

	if( FAILED( hr ) )
	{
		return NULL;
	}

	return lptexture;
}

void RestoreAllSpriteTexMem();
void ReleaseAllSpriteTexMem();

//--------------------------------- ≈ÿΩ∫√ƒ∑Œµ˘ ∞¸∑√ -------------------------------------
typedef struct{
	LPDIRECT3DTEXTURE8 m_pTexture;
	DWORD mFlag;
}_R3D3DTEXTURE;


/*
typedef struct{
	char mName[128];
	LPDIRECT3DTEXTURE8 m_pTexture;
}_R3Texture;
*/
static _R3D3DTEXTURE *stR3D3DTexture=NULL;
static int stR3D3DTexCnt=1,stR3D3DTexMaxCnt=64;

static R3Texture *stR3Texture=NULL;
static int stR3TexCnt=1,stR3TexMaxCnt=64;

//#define	_R3T_VERSION	1.0

//int GetNowR3D3DTexCnt()	//«ˆ¿Á ≈ÿΩ∫√ƒ ƒ´øÓ∆Æ
int GetNowR3D3DTexCnt()	//«ˆ¿Á ≈ÿΩ∫√ƒ ƒ´øÓ∆Æ
{
	return stR3D3DTexCnt;
}
void SetNowR3D3DTexCnt(int cnt) //π´¿∞‘ ¡÷¿«% ∏∏£∞Ì æ≤∏È √•¿”∏¯¡¸...
{
	for(int i=cnt; i<stR3D3DTexCnt;i++)
	{
		memset(&stR3D3DTexture[i],0,sizeof(_R3D3DTEXTURE));
	}
	stR3D3DTexCnt=cnt;
}

int GetNowR3TexCnt()	//«ˆ¿Á r3≈ÿΩ∫√ƒ ƒ´øÓ∆Æ
{
	return stR3TexCnt;
}
void SetNowR3TexCnt(int cnt) //r3≈ÿΩ∫√ƒ ¡÷¿«, ∏∏£∞Ì æ≤∏È √•¿”∏¯¡¸...
{
	for(int i=cnt; i<stR3TexCnt; i++)
	{
		memset(&stR3Texture[i],0,sizeof(_R3D3DTEXTURE));
	}
	stR3TexCnt=cnt;
}

LPDIRECT3DTEXTURE8 GetD3DTexture(WORD r_xl,WORD r_yl,D3DFORMAT r_format
	,WORD d_xl,WORD d_yl,D3DFORMAT d_format,BYTE *buf,DWORD mipcnt)
	// d_XX¥¬ ø¯«œ¥¬ ≈ÿΩ∫√ƒªÁ¿Ã¡Ó≥™ ∆˜∏À¿∏∑Œ πŸ≤ŸæÓº≠ æÒæÓæﬂ µ…∞ÊøÏ...
{
	LPDIRECT3DTEXTURE8 ptex;
	int i,j;
	D3DLOCKED_RECT rect;
	DWORD one_trans;

	HRESULT hr=D3DXCreateTexture(
		GetD3dDevice(),
		d_xl,		d_yl,		mipcnt,		0,
		d_format,
		D3DPOOL_MANAGED,
		&ptex);

	if( hr != D3D_OK )
	{
		Error("ø¯«œ¥¬ ≈ÿΩ∫√ƒ ∆˜∏À¿Ãª˝º∫µ«¡ˆ æ Ω¿¥œ¥Ÿ.","");
	}

	if( d_format == r_format )	//ø¯«œ¥¬ ∆˜∏À∞˙ Ω«¡¶∆˜∏À¿Ã ∏¬¥¬∞ÊøÏ.
	{
		ptex->LockRect(0,&rect,0,0);

		if( r_format == D3DFMT_DXT1 )
			one_trans = r_xl/2;
		else
		if( r_format == D3DFMT_R5G6B5 )
			one_trans = r_xl*2;
		else
			one_trans = r_xl;

		for( i=0; i<r_yl; i++)
		{
			memcpy(&((BYTE*)rect.pBits)[i*rect.Pitch],&buf[i*one_trans],one_trans );
		}
		ptex->UnlockRect(0);
	}
	else
	{
		if( r_format == D3DFMT_R8G8B8 && d_format == D3DFMT_A8R8G8B8 )
		{
			ptex->LockRect(0,&rect,0,0);
			for( i=0; i<r_yl; i++)
			{
				for( j=0; j<r_xl; j++)
				{
					((BYTE*)rect.pBits)[i*rect.Pitch+j*4+3]=0xff;	//A
					((BYTE*)rect.pBits)[i*rect.Pitch+j*4+2]=buf[(i*r_xl+j)*4+0];	//R
					((BYTE*)rect.pBits)[i*rect.Pitch+j*4+1]=buf[(i*r_xl+j)*4+1];	//G
					((BYTE*)rect.pBits)[i*rect.Pitch+j*4+0]=buf[(i*r_xl+j)*4+2];	//B
				}
			}
			ptex->UnlockRect(0);
		}
		else
			Error("æ∆¡˜ ¿Ã ≈ÿΩ∫√ƒ∆˜∏À ¡ˆø¯¿Ã µ«¡ˆ æ ¿∏π«∑Œ ¿Âæ¿œø°∞‘ πÆ¿««—¥Ÿ.","");
	}
	return ptex;
}

/*	//≥™¡ﬂø° ¿€æ˜.
void LoadR3T(R3Texture *tex,D3DFORMAT desire_format,DWORD mip_level)
{
	float version;
	DWORD format,cnt,flag,size;

	BYTE *temp_buf=(BYTE *)Dmalloc(2048*2048*2);
	FILE *fp = fopen(tex->mName,"rb");
	fread(&version,4,1,fp);	//πˆ¿¸
	fread(&format,4,1,fp);		//∆˜∏À
	fread(&cnt,4,1,fp);	//≈ÿΩ∫√ƒ ∞≥ºˆ 

	for(DWORD j=0; j<cnt; j++)
	{
		WORD xl,yl;
		fread(&flag,4,1,fp);	//«√∑°±◊ 
		fread(&xl,2,1,fp);		//∆˜∏À
		fread(&yl,2,1,fp);		//∆˜∏À
		fread(&size,4,1,fp);		//∆˜∏À
//		stR3D3DTexture[tex->mStartID-1+j].m_pTexture = GetD3DTexture(format,desire_format,buf);
	}
	fclose(fp);
	Dfree(temp_buf);
}
*/
static DWORD GetOneStepMipMap(WORD size)
{
	if( size > 4 )
		size>>=1;
	return (DWORD)size;
}

void MakeMipMap(WORD xl,WORD yl,WORD *d_buf,WORD *r_buf)
{
	int i,j,r_xl,r_yl;
	DWORD r,g,b;

	r_xl=xl<<1;
	r_yl=yl<<1;

	for(i=0; i<yl; i++)
	{
		for(j=0; j<xl; j++)
		{
			int index = (i*2)*r_xl+(j<<1);
			r = (r_buf[index]>>11);
			g = (r_buf[index]>>5)&63;
			b = (r_buf[index])&31;
			index = (i*2)*r_xl+(j<<1)+1;
			r += (r_buf[index]>>11);
			g += (r_buf[index]>>5)&63;
			b += (r_buf[index])&31;
			index = (i*2+1)*r_xl+(j<<1);
			r += (r_buf[index]>>11);
			g += (r_buf[index]>>5)&63;
			b += (r_buf[index])&31;
			index = (i*2+1)*r_xl+(j<<1)+1;
			r += (r_buf[index]>>11);
			g += (r_buf[index]>>5)&63;
			b += (r_buf[index])&31;
			r>>=2;
			g>>=2;
			b>>=2;
			d_buf[i*xl+j] = (WORD)((r<<11)|(g<<5)|b);
		}
	}
}

void MakeMipMap(WORD xl,WORD yl,WORD *d_buf,BYTE *r_buf)
{
	int i,j,r_xl,r_yl;
	DWORD r,g,b;

	r_xl=xl<<1;
	r_yl=yl<<1;

	for(i=0; i<yl; i++)
	{
		for(j=0; j<xl; j++)
		{
			int index = (i*2)*r_xl+(j<<1);
			r = (r_buf[index*4+0]);
			g = (r_buf[index*4+1]);
			b = (r_buf[index*4+2]);
			index = (i*2)*r_xl+(j<<1)+1;
			r += (r_buf[index*4+0]);
			g += (r_buf[index*4+1]);
			b += (r_buf[index*4+2]);
			index = (i*2+1)*r_xl+(j<<1);
			r += (r_buf[index*4+0]);
			g += (r_buf[index*4+1]);
			b += (r_buf[index*4+2]);
			index = (i*2+1)*r_xl+(j<<1)+1;
			r += (r_buf[index*4+0]);
			g += (r_buf[index*4+1]);
			b += (r_buf[index*4+2]);
			r>>=5;
			g>>=4;
			b>>=5;
			d_buf[i*xl+j] = (WORD)((b<<11)|(g<<5)|r);
		}
	}
}

static DWORD LightMapTexSize;
DWORD GetLightMapTexSize()
{
	return LightMapTexSize;
}

_LIGHTMAP **LoadR3TLightMap(R3Texture *tex,D3DFORMAT desire_format)
//∂Û¿Ã∆Æ∏ ¿ª ¿–±‚¿ß«—«‘ºˆ.... πˆ∆€ ∏Æ≈œ∞™¿∫ ∂Û¿Ã∆Æ∏  ∞πºˆ.
//¿Ã¿¸ø° ≈ÿΩ∫√ƒ ¡§∫∏(tex)¥¬ πÃ∏Æ ≥÷æÓ¡÷æÓ«—¥Ÿ.
{
	float version;
	DWORD format,cnt=0,size,mip_xl,mip_yl;
	_LIGHTMAP **Lightmap;

	LightMapTexSize=0;
	if( tex == NULL )
		return NULL;

	FILE *fp = fopen(tex->mName,"rb");
	if( fp == NULL )
		return NULL;
	fread(&version,4,1,fp);	//πˆ¿¸
	fread(&cnt,4,1,fp);	//≈ÿΩ∫√ƒ ∞≥ºˆ 

	if( version != 1.1f && version != 1.2f )
	{
		if( _R3T_VERSION != version )
		{
			Warning(tex->mName,"<-¿Ã ∆ƒ¿œ¿Ã ¿Ã¿¸πˆ¿¸¿‘¥œ¥Ÿ.");
			return NULL;
		}
	}
	
	Lightmap=(_LIGHTMAP **)Dmalloc(cnt*sizeof(_LIGHTMAP *));

	for(DWORD j=0; j<cnt; j++)
	{
		fread(&size,sizeof(DWORD),1,fp);

		BYTE *temp_buf=(BYTE *)Dmalloc(size);
		BYTE *real_buf=temp_buf+sizeof(_DDS_Header);
		fread(temp_buf,size,1,fp);
		_DDS_Header *dds_h = (_DDS_Header *)temp_buf;
		if( dds_h->dwMagic != 0x20534444)
		{
			Error(tex->mName,"¿Ã R3T∆ƒ¿œæ»ø° <- DDS∆ƒ¿œ¿Ã æ∆¥—∞Õ¿Ã µÈæÓ ¿÷Ω¿¥œ¥Ÿ.");
			Dfree(temp_buf);
			return NULL;
		}

		WORD xl,yl;
		xl=(WORD)dds_h->ddsd.dwWidth;
		yl=(WORD)dds_h->ddsd.dwHeight;
		format = dds_h->ddsd.ddpfPixelFormat.dwFourCC;
		if( format != 0 )
		{
			Error(tex->mName,"<- ¿Ã ∂Û¿Ã∆Æ∏  ≈ÿΩ∫√ƒ¥¬ ¡ˆø¯∏¯«‘...");
			return NULL;
		}
		if( dds_h->ddsd.ddpfPixelFormat.dwRGBBitCount == 16)
			format=D3DFMT_R5G6B5;
		else 
		if( dds_h->ddsd.ddpfPixelFormat.dwRGBBitCount == 32)
			format=D3DFMT_A8R8G8B8;
		else
		{
			Error(tex->mName,"<- ¿Ã ∂Û¿Ã∆Æ∏  ≈ÿΩ∫√ƒ¥¬ ¡ˆø¯∏¯«‘...");
			return NULL;
		}
//		if( xl > 1024 || yl > 2048 )
		if( xl > 2048 || yl > 2048 )
			Error("∂Û¿Ã∆Æ∏ ¿Ã ≥ π´≈´∞…...","");

		LightMapTexSize += xl*yl*(dds_h->ddsd.ddpfPixelFormat.dwRGBBitCount/8);

		mip_xl=GetOneStepMipMap(xl);
		mip_yl=GetOneStepMipMap(yl);
		//∞≠µµ ∞ËªÍøÎ.. Ω«¡¶ ∂Û¿Ã∆Æ∏ ¿ª ¥Ÿø√∏Æ∏È ≥—≥∂∫Òø©º≠ «—π¯ mipmap¿ª «ÿ¡ÿ∞Õ¿ª ø√∏∞¥Ÿ.
		Lightmap[j]=(_LIGHTMAP *)Dmalloc(sizeof(_LIGHTMAP));
		Lightmap[j]->xl = (WORD)mip_xl;
		Lightmap[j]->yl = (WORD)mip_yl;
		Lightmap[j]->buf = (WORD *)Dmalloc(mip_xl*mip_yl*sizeof(WORD));

		stNowTexMem += size;
		if( format == D3DFMT_A8R8G8B8 )
		{
			MakeMipMap(Lightmap[j]->xl,Lightmap[j]->yl,Lightmap[j]->buf,(BYTE *)(temp_buf+sizeof(_DDS_Header)));
			//∂Û¿Ã∆Æ∏ ¿∫ 32∫Ò∆Æ∑Œ...
			stR3D3DTexture[tex->mStartID+j].m_pTexture = GetD3DTextureFromBuffer(temp_buf,size,0);
			//stR3D3DTexture[tex->mStartID+j].m_pTexture = GetD3DTexture(xl,yl
			//	,(D3DFORMAT)format,xl,yl,D3DFMT_A8R8G8B8,temp_buf,1);
		}
		else
		if( format == D3DFMT_R5G6B5 )
		{
			MakeMipMap(Lightmap[j]->xl,Lightmap[j]->yl,Lightmap[j]->buf,(WORD *)(temp_buf+sizeof(_DDS_Header)));
			//∂Û¿Ã∆Æ∏ ¿∫ 16∫Ò∆Æ∑Œ...
			stR3D3DTexture[tex->mStartID+j].m_pTexture = GetD3DTextureFromBuffer(temp_buf,size,0);
			//stR3D3DTexture[tex->mStartID+j].m_pTexture = GetD3DTexture(xl,yl
			//	,(D3DFORMAT)format,xl,yl,D3DFMT_R5G6B5,temp_buf,1);
		}
		else
		{
			Dfree(temp_buf);
			Error("æ∆¡˜ ¡ˆø¯µ«¡ˆ æ ¿∫ R3t∆ƒ¿œ¿‘¥œ¥Ÿ.","");
			return NULL;
		}
		Dfree(temp_buf);
	}
	fclose(fp);
	return Lightmap;
}


DWORD stR3TexManageFlag=0;
void SetR3TexManageFlag(DWORD mFlag)
{
	stR3TexManageFlag=mFlag;
}
DWORD GetR3TexManageFlag()
{
	return stR3TexManageFlag;
}

R3Texture *R3GetTexInfoR3T(char *name,DWORD flag)	//R3T≈ÿΩ∫√ƒ ¡§∫∏∏¶ ∏Æ≈œ«—¥Ÿ.
{
	if(stR3Texture==NULL)	//√≥¿Ωø°¥¬
	{
		stR3Texture = (R3Texture *)Dmalloc(sizeof(R3Texture)*stR3TexMaxCnt);
		memset( stR3Texture, 0, sizeof(R3Texture)*stR3TexMaxCnt );

		stR3D3DTexture = (_R3D3DTEXTURE *)Dmalloc(sizeof(_R3D3DTEXTURE)*stR3D3DTexMaxCnt);
		memset( stR3D3DTexture, 0, sizeof(_R3D3DTEXTURE)*stR3D3DTexMaxCnt );
	}
	if( stR3TexCnt >= stR3TexMaxCnt )	//√≥¿Ω 64∞≥¿‚¿∫∞Õ ∫∏¥Ÿ ∏π¿ª ∞ÊøÏ ¿ÁπËƒ°.
	{
		stR3Texture=(R3Texture *)ReAlloc(stR3Texture,sizeof(R3Texture)*stR3TexMaxCnt,sizeof(R3Texture)*(stR3TexMaxCnt+16));
		memset( (stR3Texture+stR3TexMaxCnt), 0, sizeof(R3Texture)*16 );
		stR3TexMaxCnt += 16;
	}
	if( !(stR3TexManageFlag & _R3TEX_MANAGE_FLAG_NO_SAME_CHECK) )
	{
		for(int i=1; i<stR3TexCnt; i++)
		{
			if(!strcmp(stR3Texture[i].mName,name))	// ∞∞¿∫¿Ã∏ß¿∏∑Œ ≈ÿΩ∫√ƒ∞° ¿÷æ˙¥Ÿ..
			{
				stR3Texture[i].mSameCnt++;
				return &stR3Texture[i];
			}
		}
	}
	if(strlen(name)>=_TEX_NAME_SIZE-1)
	{
		Warning("¿¢ ≈ÿΩ∫√ƒ¿Ã∏ß¿Ã ¿Ã∑∏∞‘ ±‚≥ƒ...","");
		return NULL;
	}

	float version;
//	DWORD format;
	int cnt;

	FILE *fp = fopenMFM(name,"rb");
	if(fp==NULL)
	{
		char buf[256];
		strcpy( buf, name );
		strlwr(buf);
		StripEXT(buf);
		if( buf[strlen(buf)-3]=='l' && buf[strlen(buf)-2]=='g' && buf[strlen(buf)-1]=='t' )
		{
		}
		else
			Warning(name,"<-¿Ã∆ƒ¿œ¿Ã æ¯¥¬µ• r3m∆ƒ¿œ¿∫ ¡∏¿Á«’¥œ¥Ÿ.. æ∆∏∂ ∑Œ∫øø°º≠π˜≥µ¿ª∞Ã¥œ¥Ÿ...");
		return NULL;
	}
	fread(&version,4,1,fp);	//πˆ¿¸
//	fread(&format,4,1,fp);		//∆˜∏À
	fread(&cnt,4,1,fp);	//≈ÿΩ∫√ƒ ∞≥ºˆ 
	fclose(fp);
	
	stR3Texture[stR3TexCnt].mFlag = flag;
	stR3Texture[stR3TexCnt].mStartID = stR3D3DTexCnt;
	stR3Texture[stR3TexCnt].mTexNum = cnt;
	strcpy(stR3Texture[stR3TexCnt].mName,name);

	if( cnt+stR3D3DTexCnt >= stR3D3DTexMaxCnt )	//≈ÿΩ∫√ƒ ≥÷¿ª ∞¯∞£»Æ∫∏...
	{
		stR3D3DTexture=(_R3D3DTEXTURE *)ReAlloc(stR3D3DTexture,sizeof(_R3D3DTEXTURE)*stR3D3DTexMaxCnt,sizeof(_R3D3DTEXTURE)*(stR3D3DTexMaxCnt+cnt));
		memset( (stR3D3DTexture+stR3D3DTexMaxCnt), 0, sizeof(_R3D3DTEXTURE)*cnt );
		stR3D3DTexMaxCnt += cnt;
	}

	stR3D3DTexCnt+=cnt;
	stR3TexCnt++;
	return &stR3Texture[stR3TexCnt-1];
}



char *R3GetTexName(int id)	//æ∆¿Ãµ∏¶ ≥÷æÓ¡÷∏È ≈ÿΩ∫√ƒ ¿Ã∏ß¿ª æÀ∑¡¡ÿ¥Ÿ.
{
	if( id >= stR3TexCnt || id < 0 )	//æ¯¥Ÿ.
		return NULL;
	return stR3Texture[id].mName;
}

int R3GetPreTextureId(char *name)//√≥¿Ωø° ∑Œµ˘«“ ≈ÿΩ∫√ƒ æ∆¿Ãµ∏¶ æÚæÓ≥Ω¥Ÿ.Ω«¡¶∑Œ ≈ÿΩ∫√ƒ ∏ﬁ∏∏Æø° æÒ¡ˆ¥¬ æ ¥¬¥Ÿ.
{
	int i;

	if(stR3Texture==NULL)
	{
		stR3Texture = (R3Texture *)Dmalloc(sizeof(R3Texture)*stR3TexMaxCnt);
		memset( stR3Texture, 0, sizeof(R3Texture)*stR3TexMaxCnt );

		stR3D3DTexture = (_R3D3DTEXTURE *)Dmalloc(sizeof(_R3D3DTEXTURE)*stR3D3DTexMaxCnt);
		memset( stR3D3DTexture, 0, sizeof(_R3D3DTEXTURE)*stR3D3DTexMaxCnt );
	}
	if( stR3TexCnt >= stR3TexMaxCnt )	//√≥¿Ω 64∞≥¿‚¿∫∞Õ ∫∏¥Ÿ ∏π¿ª ∞ÊøÏ ¿ÁπËƒ°.
	{
		stR3Texture=(R3Texture *)ReAlloc(stR3Texture,sizeof(R3Texture)*stR3TexMaxCnt,sizeof(R3Texture)*(stR3TexMaxCnt+16));
		memset( (stR3Texture+stR3TexMaxCnt), 0, sizeof(R3Texture)*16 );
		stR3TexMaxCnt += 16;
	}
	if(strlen(name)<_TEX_NAME_SIZE-1)
	{
		// ∞∞¿∫¿Ã∏ß¿∏∑Œ ≈ÿΩ∫√ƒ∏¶ π´Ω√«“∂ß∞«≥  ∂⁄¥Ÿ.
		if( !(stR3TexManageFlag & _R3TEX_MANAGE_FLAG_NO_SAME_CHECK) )
		{
			for(int i=1; i<stR3TexCnt; i++)
			{
				if(!strcmp(stR3Texture[i].mName,name))	// ∞∞¿∫¿Ã∏ß¿∏∑Œ ≈ÿΩ∫√ƒ∞° ¿÷æ˙¥Ÿ..
				{
					stR3Texture[i].mSameCnt++;
					return stR3Texture[i].mStartID;
				}
			}
		}
		strcpy(stR3Texture[stR3TexCnt].mName,name);
	}
	else
	{
		Warning("¿¢ ≈ÿΩ∫√ƒ¿Ã∏ß¿Ã ¿Ã∑∏∞‘ ±‚≥ƒ...","");
		return 0;
	}

	//∫ÛΩΩ∑‘¿Ã ¿÷¿∏∏È √£æ∆º≠ æ¥¥Ÿ.
	BOOL is_empty_slut=FALSE;
	if( !(stR3TexManageFlag & _R3TEX_MANAGE_FLAG_NO_SAME_CHECK) )
	{
		for( i=1; i<stR3TexCnt; i++)
		{
			if( stR3Texture[i].mTexNum ==0 && stR3Texture[i].mName[0] == NULL )
			{
				is_empty_slut=TRUE;
				break;
			}
		}
	}
	if( is_empty_slut )
	{
		stR3Texture[i].mFlag = _R3TEXFLAG_NONE_R3T|_R3TEXFLAG_MIPMAP;
		stR3Texture[i].mStartID = i;
		stR3Texture[i].mTexNum=1;
		return i;
	}
	else
	{
		stR3Texture[stR3TexCnt].mFlag = _R3TEXFLAG_NONE_R3T|_R3TEXFLAG_MIPMAP;
		stR3Texture[stR3TexCnt].mStartID = stR3D3DTexCnt;
		stR3Texture[stR3TexCnt].mTexNum=1;
		stR3D3DTexCnt++;	//Ω«¡¶ ≈ÿΩ∫√ƒ∞πºˆ∏¶ ø√∏∞¥Ÿ.
		stR3TexCnt++;

		if( stR3D3DTexCnt >= stR3D3DTexMaxCnt )	//Ω«¡¶ ≈ÿΩ∫√ƒ ≥÷¿ª ∞¯∞£»Æ∫∏...
		{
			stR3D3DTexture=(_R3D3DTEXTURE *)ReAlloc(stR3D3DTexture,sizeof(_R3D3DTEXTURE)*stR3D3DTexMaxCnt,sizeof(_R3D3DTEXTURE)*(stR3D3DTexMaxCnt+16));
			memset( (stR3D3DTexture+stR3D3DTexMaxCnt), 0, sizeof(_R3D3DTEXTURE)*16 );
			stR3D3DTexMaxCnt += 16;
		}
		return stR3D3DTexCnt-1;
	}
}
BOOL IsAniTex(char *hole)	//æ÷¥œ∏ﬁ¿Ãº« ≈ÿΩ∫√ƒ¿Œ¡ˆ æÀæ∆≥Ω¥Ÿ.
{
	DWORD i;
	if( hole == NULL )
		return FALSE;
	for(i=0; i<strlen(hole); i++)
	{
		if( hole[i] == '$')
			return TRUE;
	}
	return FALSE;
}
static int GetAniTexNum(char *path,char *pre_name,char *ext)
{
	int i;
	char name[256];
	FILE *fp;

	for(i=1; i<_MAX_ANI_TEX_NUM+1; i++)
	{
		strcpy(name,path);
		sprintf(name,"%s%s%04d%s",path,pre_name,i,ext);
		if( (fp = fopen(name,"rb"))==NULL )
		{
			return i-1;
		}
		else
			fclose(fp);
	}
	return _MAX_ANI_TEX_NUM+1;
}

void R3GetPreAniTextureId(char *path,char *name,LONG *start_id,LONG *num) //∑Œµ˘«“ æ÷¥œ ≈ÿΩ∫√ƒ Ω√¿€ æ∆¿Ãµπ◊ ∞≥ºˆ∏¶ æÚæÓ≥Ω¥Ÿ.Ω«¡¶∑Œ ≈ÿΩ∫√ƒ ∏ﬁ∏∏Æø° æÒ¡ˆ¥¬ æ ¥¬¥Ÿ.
{
	int i;
	char pre_name[128],ext[32],r_name[256];

	//---¿Ã∏ß∞˙ »Æ¿Â¿⁄∏¶ æÀæ∆≥Ω¥Ÿ.
	for(i=0; i<128;i++)	
	{
		if(name[i] == '.')
			break;
		if(name[i] != '$')
			pre_name[i] = name[i];
		else
			pre_name[i] = NULL;
	}
	ext[0] = '.';
	ext[1] = name[i+1];
	ext[2] = name[i+2];
	ext[3] = name[i+3];
	ext[4] = NULL;

	*num = GetAniTexNum(path,pre_name,ext);

	for(i=1; i<*num+1; i++)
	{
		strcpy(r_name,path);
		sprintf(r_name,"%s%s%04d%s",path,pre_name,i,ext);
		int id = R3GetPreTextureId(r_name);
		if( i==1 )
			*start_id = id;
	}
}

void R3ReleasePreTextures()	//∑Œµ˘«“ ≈ÿΩ∫√ƒµÈ¿ª ¿˙¿Â«œ¥¬ _R3Texture∏¶ æ¯æÿ¥Ÿ.
{
	if(stR3Texture)
	{
		Dfree(stR3Texture);
		stR3Texture = NULL;
	}
	stR3TexCnt=1;

	if(stR3D3DTexture)
	{
		Dfree(stR3D3DTexture);
		stR3D3DTexture = NULL;
	}
	stR3D3DTexCnt=1;
}

void R3ReleaseAllTextures()	//≈ÿΩ∫√ƒ∏ﬁ∏∏Æ∏¶ ¿¸∫Œ ≈¨∏ÆæÓ«—¥Ÿ.
{
	
	for(int i=1; i<stR3D3DTexCnt; i++)
	{
	    SAFE_RELEASE( stR3D3DTexture[i].m_pTexture);
	}
	ReleaseSystemTexture();	//Ω√Ω∫≈€ ≈ÿΩ∫√ƒ∏¶ ≥ª∏∞¥Ÿ.
//	ReleaseAllSpriteTexMem();	//Ω∫«¡∂Û¿Ã∆Æ∏¶ ¥Ÿ≥ª∏∞¥Ÿ.
}

void R3LoadTextureMem(int id)	//∆Ø¡§ ≈ÿΩ∫√ƒ∏¶ ≈ÿΩ∫√ƒ∏ﬁ∏∏Æø° æÒ¥¬¥Ÿ. R3T∆ƒ¿œ¿Ã µ«∏È æ»µ»¥Ÿ.
{
	int i;

	if( stR3D3DTexture[id].m_pTexture || stR3TexCnt<=1 )
		return;	

	for(i=1; i<stR3TexCnt; i++)
	{
		if( (int)stR3Texture[i].mStartID > id )
		{
			break;
		}
	}
	if( !(stR3Texture[i-1].mFlag & _R3TEXFLAG_NONE_R3T) )
	{
		Warning("R3T∆ƒ¿œ¿ª ∏ﬁ∏∏Æø° ø√∏±ºˆ æ¯Ω¿¥œ¥Ÿ.","");
		return;
	}
	if( stR3Texture[i-1].mSameCnt )	//∞∞¿∫∞≈¥¬ ≈ÿΩ∫√ƒ∑Œ ø√∂Û∞£¥Ÿ.
		return;
//	stR3D3DTexture[id].m_pTexture = R3LoadDDSAndTextureMem(stR3Texture[i-1].mName);
	if( stR3D3DTexture[id].mFlag & _R3D3DTEXTURE_FLAG_NO_LOD )	//lod∏‘¿Ã∏È æ»µ«¥¬∞ÊøÏ.
		stR3D3DTexture[id].m_pTexture = R3LoadDDS(stR3Texture[i-1].mName,0,2048,2048);
	else
		stR3D3DTexture[id].m_pTexture = R3LoadDDS(stR3Texture[i-1].mName,GetR3State()->mSceneTextureDetail,2048,2048);
}

void R3ReleaseTextureMem(int id)	//∆Ø¡§ ≈ÿΩ∫√ƒ∏¶ ≈ÿΩ∫√ƒ∏ﬁ∏∏Æø° ≥ª∏∞¥Ÿ.
{
	int i;

	if( stR3D3DTexture == NULL || id == -1 )
		return;
	if( stR3D3DTexture[id].m_pTexture==NULL )
		return;
	for(i=1; i<stR3TexCnt; i++)
	{
		if( (int)stR3Texture[i].mStartID > id )
		{
			break;
		}
	}
	if( stR3Texture[i-1].mSameCnt )	//æÓµº±∞° æ≤∞Ì ¿÷æÓº≠ ¥Á¿Â ≥ª∏Æ¡ˆ ∏¯«—¥Ÿ.ƒ´øÓ∆Æ∏¶ ¿œ¥‹ ¡¶∞≈«—¥Ÿ.
	{
		stR3Texture[i-1].mSameCnt--;
		return;
	}
	if( !(stR3Texture[i-1].mFlag & _R3TEXFLAG_NONE_R3T) )
	{
		Warning("R3T∆ƒ¿œ¿ª ∏ﬁ∏∏Æø°º≠ ≥ª∏±ºˆæ¯Ω¿¥œ¥Ÿ.","");
	}
	stR3Texture[i-1].mName[0]=NULL;
    SAFE_RELEASE( stR3D3DTexture[id].m_pTexture);
}


static CTextureRender stShadowTexture;	//Ω¶µµøÏ≈ÿΩ∫√ƒ∑ª¥ıøÎ.

CTextureRender *GetShadowRenderTexture()
{
	return &stShadowTexture;	//Ω¶µµøÏ≈ÿΩ∫√ƒ∑ª¥ıøÎ.
}
void RestoreSystemTexture()	//Ω√Ω∫≈€ ≈ÿΩ∫√ƒ∏¶ ø√∏∞¥Ÿ.
{
	_R3ENGINE_STATE *state = GetR3State();

	if( state->mDLightTexture )
		state->mDLightTexture->Release();			//Ω√Ω∫≈€ ¥Ÿ¿Ã≥™πÕ ∂Û¿Ã∆Æ≈ÿΩ∫√ƒ
//--------------------------- ∫º∑˝ ∆˜±◊øÎ Ω√Ω∫≈€ ≈ÿΩ∫√ƒ....
	stShadowTexture.CreateTexture(GetD3dDevice(),256,256,FALSE);
/*
	if( state->mVFogTexture )
		state->mVFogTexture->Release();			//Ω√Ω∫≈€ ∆˜±◊≈ÿΩ∫√ƒ
//	state->mVFogTexture = R3LoadDDSAndTextureMem(".\\system\\r3fog.tga");
	HRESULT hr=D3DXCreateTexture(
			GetD3dDevice(),
			32,
			256,
			1,
			0,
			D3DFMT_A8R8G8B8,
			D3DPOOL_MANAGED,
			&state->mVFogTexture);
	if( hr != D3D_OK )
		Error("Ω√Ω∫≈€ ≈ÿΩ∫√ƒ∏¶ ª˝º∫«“ºˆ æ¯Ω¿¥œ¥Ÿ.!","");
	D3DLOCKED_RECT rect;
	state->mVFogTexture->LockRect(0,&rect,0,0);

	float start = 15;
	float max_i=(float)pow(start,2);
	float div_f = (1)/256.0f;

	for(int i=0; i<256; i++)
	{
		float p_pow= (float)pow(start, 2-(float)i*div_f);
		BYTE gop=255-(BYTE)(((p_pow-start)/(max_i-start))*255);  
//		gop = rand()%255;
		for(int j=0; j<32; j++)
		{
			((BYTE*)rect.pBits)[(i)*rect.Pitch+j*4]=gop;
			((BYTE*)rect.pBits)[(i)*rect.Pitch+j*4+1]=gop;
			((BYTE*)rect.pBits)[(i)*rect.Pitch+j*4+2]=gop;
			((BYTE*)rect.pBits)[(i)*rect.Pitch+j*4+3]=gop;
		}
	}
	state->mVFogTexture->UnlockRect(0);
	if( state->mVFogTexture == NULL )
		Error("Ω√Ω∫≈€ ≈ÿΩ∫√ƒ∏¶ ø√∏Æ¡ˆ∏¶ ∏¯«‘.","");
*/

	//∑Œ∞Ì ≈ÿΩ∫√ƒ
	if( state->mLogoTexture )
		state->mLogoTexture = R3LoadDDS(".\\system\\logo.dds",2);
	if( state->mDLightTexture )
		state->mDLightTexture = R3LoadDDS(".\\system\\dlight.dds",2);

//---------------------------------------------------------------

}

void ReleaseSystemTexture()	//Ω√Ω∫≈€ ≈ÿΩ∫√ƒ∏¶ ≥ª∏∞¥Ÿ.
{
	_R3ENGINE_STATE *state = GetR3State();
	stShadowTexture.ReleaseTexture();
	if( state->mDLightTexture )
	{
		state->mDLightTexture->Release();			//Ω√Ω∫≈€ ∆˜±◊≈ÿΩ∫√ƒ
		state->mDLightTexture=NULL;
	}
	if( state->mLogoTexture )
	{
		state->mLogoTexture->Release();			//Ω√Ω∫≈€ ∑Œ∞Ì≈ÿΩ∫√ƒ
		state->mLogoTexture=NULL;
	}
}


void R3RestoreAllTextures()	//_R3Texture∏¶ ¬¸¡∂«ÿº≠ ∑Œµ˘«“ ≈ÿΩ∫√ƒ∏¶ ≈ÿΩ∫√ƒ∏ﬁ∏∏Æø° æÒ¥¬¥Ÿ.
{
	for(int i=1; i<stR3TexCnt; i++)
	{
		if( stR3Texture[i].mFlag&_R3TEXFLAG_NONE_R3T )
		{
			if( stR3Texture[i].mTexNum != 1 )
				Error("≈ÿΩ∫√ƒ ∑Œµ˘ ø°∑Ø...","");
			if( stR3D3DTexture[stR3Texture[i].mStartID].m_pTexture==NULL )
			{
				if( stR3D3DTexture[stR3Texture[i].mStartID].mFlag & _R3D3DTEXTURE_FLAG_NO_LOD )
					stR3D3DTexture[stR3Texture[i].mStartID].m_pTexture = R3LoadDDS(stR3Texture[i].mName,0,2048,2048);
				else
					stR3D3DTexture[stR3Texture[i].mStartID].m_pTexture = R3LoadDDS(stR3Texture[i].mName,GetR3State()->mSceneTextureDetail,2048,2048);
			}
//				stR3D3DTexture[stR3Texture[i].mStartID].m_pTexture = R3LoadDDSAndTextureMem(stR3Texture[i].mName);
		}
//		else
//			LoadR3T(&stR3Texture[i]);
	}
//	RestoreAllSpriteTexMem();	//Ω∫«¡∂Û¿Ã∆Æ∏¶ ø√∏∞¥Ÿ.
	RestoreSystemTexture();	//Ω√Ω∫≈€ ≈ÿΩ∫√ƒ∏¶ ø√∏∞¥Ÿ.
}

LPDIRECT3DTEXTURE8 R3GetSurface(int id)	//id∏¶ ≥÷æÓ¡÷∏È «•∏È¿ª ∏Æ≈œ«ÿ¡ÿ¥Ÿ.
{
	if( id <= 0 )
		return NULL;
	if( id <= stR3D3DTexCnt)
		return stR3D3DTexture[id].m_pTexture;
	return NULL;
}

static DWORD stDebugSize;
static char stDebugName[256];

void SetDDSTexSizeDebug(DWORD size)
{
	stDebugSize = size;
}
void SetDDSTexNameDebug(char *name)
{
	strncpy( stDebugName, name ,128);
}

//--------2d Ω∫«¡∂Û¿Ã∆Æ ∞¸∑√.
void *GetDDSTexFromBuffer(WORD xl,WORD yl,DWORD format,BYTE *buf)
{
	_R3ENGINE_STATE *state = GetR3State();

	LPDIRECT3DTEXTURE8 ptex;
	D3DLOCKED_RECT rect,src_rect;
	DWORD one_trans;
	DWORD line_cnt;
	DWORD i;
	HRESULT hr;

//	state->mBestSpriteTextureFormat = D3DFMT_R5G6B5;

	if( format == D3DFMT_DXT1 || format == D3DFMT_DXT3 || format == D3DFMT_DXT5 )
	{
		hr=D3DXCreateTexture(
			GetD3dDevice(),
			xl,		yl,		1,		0,
			(_D3DFORMAT)format,
			D3DPOOL_MANAGED,
			&ptex);

		if( hr != D3D_OK )
			Error("Sprite texture format not supported.","");

		ptex->LockRect(0,&rect,0,0);

		if( format == D3DFMT_DXT1 )
			one_trans = (((DWORD)xl + 3) / 4) * 8;
		else
			one_trans = (((DWORD)xl + 3) / 4) * 16;
		line_cnt = ((DWORD)yl + 3) / 4;
		if( line_cnt == 0 )
			line_cnt = 1;

		for(i=0; i<line_cnt; i++)
		{
			memcpy(&((BYTE*)rect.pBits)[i*rect.Pitch],&buf[i*one_trans],one_trans );
		}
		ptex->UnlockRect(0);
	}
	else
	{
		hr=D3DXCreateTexture(
			GetD3dDevice(),
			xl,		yl,		1,		0,
			(_D3DFORMAT)format,
			D3DPOOL_MANAGED,
			&ptex);

		if( hr != D3D_OK )
			Error("Sprite texture format not supported.","");

		LPDIRECT3DSURFACE8 psurf,ptar;

		GetD3dDevice()->CreateImageSurface(xl,yl,(_D3DFORMAT)format,&psurf);
		if( format == D3DFMT_A8R8G8B8 )
		{
			one_trans = xl*4;
		}
		else
		if( format == D3DFMT_A1R5G5B5 || format == D3DFMT_R5G6B5 )
		{
			one_trans = xl*2;
		}
		else
		{
			Warning("This sprite texture format is not supported by R3Engine yet.","");
			psurf->Release();
			ptex->Release();
			return NULL;
		}
		line_cnt = yl;

		psurf->LockRect(&rect,0,0);
		for(i=0; i<line_cnt; i++)
		{
			memcpy(&((BYTE*)rect.pBits)[i*rect.Pitch],&buf[i*one_trans],one_trans );
		}
		psurf->UnlockRect();
		hr = GetD3dDevice()->CreateImageSurface(xl,yl,(_D3DFORMAT)format,&ptar);
		
        hr = D3DXLoadSurfaceFromSurface(ptar, NULL, NULL, psurf, NULL, NULL,
            D3DX_FILTER_TRIANGLE, 0);
		psurf->Release();
/*
		if( state->mBestSpriteTextureFormat == D3DFMT_A8R8G8B8 )
			one_trans = xl*4;
		else
			one_trans = xl*2;
*/
		if( format == D3DFMT_A8R8G8B8 )
			one_trans = xl*4;
		else
			one_trans = xl*2;

		ptar->LockRect(&src_rect,0,0);
		ptex->LockRect(0,&rect,0,0);	//????? lock

		for( i=0; i<line_cnt; i++)
		{
			if( i*rect.Pitch > stDebugSize )
			{
				char temp[256];
				sprintf(temp,"%s,best 0x%x,sprite 0x%x,alpha 0x%x",stDebugName
					,state->mBestTextureFormat,state->mBestSpriteTextureFormat,state->mBestAlphaTextureFormat);
				Warning(temp,"");
				break;
			}
			memcpy(&((BYTE*)rect.pBits)[i*rect.Pitch],&((BYTE*)src_rect.pBits)[i*src_rect.Pitch],one_trans );
		}
		ptex->UnlockRect(0);
		ptar->UnlockRect();
		ptar->Release();
	}
	return (void *)ptex;
}
#include "2dsprite.h"

typedef struct{
	char mName[128];
	CSprite *m_pSprite;
}_R3Sprite;

static _R3Sprite *stSprite=NULL;
static int stSpTex_cnt=0,stSpmax_cnt=64;

void InitSpriteManager()	//ø£¡¯ √ ±‚»≠Ω√ »£√‚.
{
	stSpmax_cnt=64;
	stSprite = (_R3Sprite *)Dmalloc(sizeof(_R3Sprite)*(stSpmax_cnt));
}
void ReleaseSpriteManager()
{
	if( stSprite==NULL )
		return;
	for(int i=0; i<stSpTex_cnt;i++)
	{
		stSprite[i].m_pSprite->ReleaseTexMemSprite();
	}
	if( stSprite )
	{
		Dfree(stSprite);
		stSprite=NULL;
	}
}
void RestoreAllSpriteTexMem()
{
	if( stSprite==NULL )
		return;
	for(int i=0; i<stSpTex_cnt;i++)
	{
		stSprite[i].m_pSprite->RestoreTexMemSprite();
	}
}
void ReleaseAllSpriteTexMem()
{
	if( stSprite==NULL )
		return;
	for(int i=0; i<stSpTex_cnt;i++)
	{
		stSprite[i].m_pSprite->ReleaseTexMemSprite();
	}
//	stSpTex_cnt=0;
}

void RestoreSpriteManager(CSprite *spr)
{
	if( stSprite==NULL )
		return;
	strcpy(stSprite[stSpTex_cnt].mName,spr->GetFileName());
	stSprite[stSpTex_cnt].m_pSprite = spr;
	stSpTex_cnt++;
	if( stSpTex_cnt >= stSpmax_cnt )
	{
		stSprite = (_R3Sprite *)ReAlloc(stSprite,sizeof(_R3Sprite)*(stSpmax_cnt),sizeof(_R3Sprite)*(stSpmax_cnt+16));
		stSpmax_cnt+=16;
	}
}
void ReleaseSpriteManager(CSprite *spr)
{
	BOOL is_true=FALSE;

	if( stSprite==NULL )
		return;
	for(int i=0; i<stSpTex_cnt;i++)
	{
		if( stSprite[i].m_pSprite == spr )
		{
			is_true =TRUE;
			break;
		}
	}
	if( is_true )
	{
		memcpy(&stSprite[i],&stSprite[i+1],sizeof(_R3Sprite)*(stSpTex_cnt-1-i));
		stSpTex_cnt--;
	}
}

void SetR3D3DTexture(DWORD id,DWORD flag)
{
	if( id <= 0 || (DWORD)stR3D3DTexCnt <= id )
		return;
	stR3D3DTexture[id].mFlag = flag;
}


//------
void LoadR3T(R3Texture *tex)	//r3t∆ƒ¿œ¿« ∞ÊøÏ 
//void LoadR3TLightMap(R3Texture *tex,D3DFORMAT desire_format)
{
	float version;
	DWORD tex_num,i,size;

	if( tex == NULL )
		return;
	FILE *fp = fopenMFM(tex->mName,"rb");
	if( fp == NULL ) 
	{
		Warning(tex->mName,"<-¿Ã∆ƒ¿œ¿Ã æ¯Ω¥");
		return;
	}
	fread(&version,4,1,fp);	//πˆ¿¸
	fread(&tex_num,sizeof(DWORD),1,fp);

	if( version < 1.1f)
//	if( _R3T_VERSION != version )
	{
		Warning(tex->mName,"<-¿Ã ∆ƒ¿œ¿Ã ¿Ã¿¸πˆ¿¸¿‘¥œ¥Ÿ.");
		return;
	}

	typedef struct{
		char name[128];
	}_tex_name;

//	_tex_name *tex_name = (_tex_name*)Dmalloc(tex_num*sizeof(_tex_name));
//	fread(tex_name,tex_num*sizeof(_tex_name),1,fp);
//	Dfree( tex_name );	//¿–¥‹ ¿–∞Ì πˆ∏Æ¥¬µ• 

	fseek(fp,tex_num*sizeof(_tex_name),SEEK_CUR);	//≈ÿΩ∫√ƒ ¿Ã∏ß ¡§∫∏µÈ¿Ã¥Ÿ. «ˆ¿Á æ»æ≤¥œ±Ó.. ∞«≥  ∂⁄¥Ÿ.

	//---¿⁄ ≈ÿΩ∫√ƒµÈ¿ª ¡√¡√ ¿–¿⁄.....
	for(i=0; i<tex_num; i++)
	{
		fread(&size,sizeof(DWORD),1,fp);
/*
		BYTE *temp=(BYTE *)Dmalloc(size);
		fread(temp,size,1,fp);
		DWORD *dwmagic = (DWORD *)temp;
		if( dwmagic[0] != 0x20534444)
		{
			Error(tex->mName,"¿Ã R3T∆ƒ¿œæ»ø° <- DDS∆ƒ¿œ¿Ã æ∆¥—∞Õ¿Ã µÈæÓ ¿÷Ω¿¥œ¥Ÿ.");
		}
		//ø©±‚ø°º≠ DDS∏¶ ∏ﬁ∏∏Æ ∫Ì∑œ π”∏ ¿« «¸≈¬∑Œ ¿–æÓµÈ¿Œ¥Ÿ.
		stR3D3DTexture[tex->mStartID+i].m_pTexture = GetD3DTextureFromBuffer(temp,size,0);
		Dfree( temp );
		*/

		if( stR3D3DTexture[tex->mStartID+i].mFlag & _R3D3DTEXTURE_FLAG_NO_LOD )
			stR3D3DTexture[tex->mStartID+i].m_pTexture =R3LoadDDSFromFP(fp,size,0);
		else
			stR3D3DTexture[tex->mStartID+i].m_pTexture =R3LoadDDSFromFP(fp,size,GetR3State()->mSceneTextureDetail);
	}

	fclose(fp);
}


//----------------------------- ≈ÿΩ∫√ƒ ∑ª¥ı ≈¨∑°Ω∫ 
CTextureRender::CTextureRender()
{
    m_pRenderToSurface=0;
    m_pCausticTex=0;
    m_pCausticSurf=0;
	m_IsLoaded=0;
    m_pZBuffer=0;
	m_pd3dDevice=0;
	m_dwXsize=0,
	m_dwYsize=0;
	m_bUseZbuffer=0;
	m_ClearAtOnce=0;
}

CTextureRender::~CTextureRender()
{
}
void CTextureRender::ReleaseTexture()
{
	if( !m_IsLoaded )
		return;
//    SAFE_RELEASE(m_pRenderToSurface);
//    SAFE_RELEASE(m_pCausticSurf);
	if( m_bUseZbuffer )
	{
	    SAFE_RELEASE(m_pCausticTex);
		SAFE_RELEASE(m_pZBuffer);
	}
	else
	{
	    SAFE_RELEASE(m_pRenderToSurface);
	    SAFE_RELEASE(m_pCausticTex);
	    SAFE_RELEASE(m_pCausticSurf);
	}
    m_pRenderToSurface=0;
    m_pCausticTex=0;
    m_pCausticSurf=0;
	m_IsLoaded=0;
    m_pZBuffer=0;
	m_pd3dDevice=0;
	m_dwXsize=0,
	m_dwYsize=0;
	m_bUseZbuffer=0;
}
BOOL CTextureRender::CreateTexture(LPDIRECT3DDEVICE8 pd3dDevice,DWORD x_size,DWORD y_size,BOOL use_zbuffer)
{
	if( m_IsLoaded )
		return FALSE;
    // Create caustic texture
    D3DDISPLAYMODE mode;
	m_pd3dDevice = pd3dDevice;
    m_pd3dDevice->GetDisplayMode(&mode);

	HRESULT hr;
	m_dwXsize = x_size;
	m_dwYsize = y_size;

	m_ClearAtOnce=FALSE;
	m_bUseZbuffer=use_zbuffer;
	if( use_zbuffer )
	{
		if( FAILED( hr = m_pd3dDevice->CreateDepthStencilSurface( x_size,y_size,
			D3DFMT_D16, D3DMULTISAMPLE_NONE, &m_pZBuffer ) ) )
		{
			return FALSE;
		}

		if(FAILED(hr = D3DXCreateTexture(m_pd3dDevice, x_size, y_size, 1, D3DUSAGE_RENDERTARGET, mode.Format, D3DPOOL_DEFAULT, &m_pCausticTex)) )
		{
			return FALSE;
		}
	}
	else
	{
		if(FAILED(hr = D3DXCreateTexture(m_pd3dDevice, x_size, y_size, 1, D3DUSAGE_RENDERTARGET, mode.Format, D3DPOOL_DEFAULT, &m_pCausticTex)) )
		{
			return FALSE;
		}
	    D3DSURFACE_DESC desc;
		m_pCausticTex->GetSurfaceLevel(0, &m_pCausticSurf);
		m_pCausticSurf->GetDesc(&desc);
		if(FAILED(hr = D3DXCreateRenderToSurface(m_pd3dDevice, desc.Width, desc.Height, 
			desc.Format, FALSE, D3DFMT_UNKNOWN, &m_pRenderToSurface)))
		{
			return FALSE;
		}
	}
	m_IsLoaded=1;
	/*
	BeginScene();
	ClearFrameBuffer(0xff000000);
//	ClearFrameBuffer();
	EndScene();
*/
    return TRUE;
}
BOOL CTextureRender::BeginScene(DWORD sub_view_port)
{
	if(!m_IsLoaded)
		return FALSE;

	if( m_bUseZbuffer )
	{
		if( !m_pCausticTex )
			return FALSE;
		HRESULT hr =  m_pCausticTex->GetSurfaceLevel(0, &m_pCausticSurf);

		if( hr != S_OK )
			return FALSE;
		//first, render the scene with the depth encoding
		hr =  m_pd3dDevice->SetRenderTarget(m_pCausticSurf, m_pZBuffer);
		if( hr != S_OK )
		{
			m_pCausticSurf->Release();
			m_pCausticSurf=0;
			return FALSE;
		}
		if(!SUCCEEDED( m_pd3dDevice->BeginScene() ))
		{
			m_pCausticSurf->Release();
			m_pCausticSurf=0;
			return FALSE;
		}
	}
	else
	if( !m_pRenderToSurface ||  !SUCCEEDED(m_pRenderToSurface->BeginScene(m_pCausticSurf, NULL)) )
		return FALSE;
	SetTextureRenderTargetFrameBuffer(FALSE);	//≈∏∞Ÿ«¡∑π¿”πˆ∆€∞° æ∆¥œ∂Ûº≠...

	PushViewPortArea();
	D3DVIEWPORT8 ViewPort;

	if( !m_ClearAtOnce )
	{
		m_ClearAtOnce=TRUE;
		ViewPort.X = 0;
		ViewPort.Y = 0;
		ViewPort.Height = m_dwXsize;
		ViewPort.Width =  m_dwYsize;
		ViewPort.MaxZ = 1;
		ViewPort.MinZ = 0;
		m_pd3dDevice->SetViewport(&ViewPort);
		ClearFrameBuffer(0xff000000);
//		ClearFrameBuffer(0xffff008f);
	}
/*
	ViewPort.X = sub_view_port;
	ViewPort.Y = sub_view_port;
	ViewPort.Height = m_dwXsize-2*sub_view_port;
	ViewPort.Width =  m_dwYsize-2*sub_view_port;
	ViewPort.MaxZ = 1;
	ViewPort.MinZ = 0;
	m_pd3dDevice->SetViewport(&ViewPort);
*/
	SetViewPortArea(sub_view_port,sub_view_port,(LONG)(m_dwXsize-2*sub_view_port),(LONG)(m_dwYsize-2*sub_view_port));
	_R3ENGINE_STATE *state = GetR3State();
	state->mScreenXsize = (float)m_dwXsize;
	state->mScreenYsize = (float)m_dwYsize;
/*
	_R3ENGINE_STATE *state = GetR3State();
	state->mViewSx = 0;
	state->mViewSy = 0;
	state->mViewXL = m_dwXsize;
	state->mViewYL = m_dwYsize;
	state->mScreenXsize = m_dwXsize;
	state->mScreenYsize = m_dwYsize;
	SetViewPortArea(0,0,(LONG)m_dwXsize,(LONG)m_dwYsize);
*/

	return TRUE;
}

void CTextureRender::ClearFrameBuffer( DWORD color )
{
	DWORD argb;

	if(!m_IsLoaded )
		return;
	if( color == 0 )
		argb = ::GetFogColor();
	else
		argb = color;

	if( m_bUseZbuffer )
	{
	    m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER
			,argb
//			,0xff000000
			, 1.0f, 0L );
	}
	else
	{
	    m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET
			,argb
			, 1.0f, 0L );
	}
}

extern LPDIRECT3DSURFACE8 stOldRenderTarget;
extern LPDIRECT3DSURFACE8 stOldStencilZ;

void CTextureRender::EndScene()
{
	if(!m_IsLoaded)
		return;
	if( m_bUseZbuffer )
	{
		if( !m_pCausticSurf )
			return;
		m_pCausticSurf->Release();
		m_pd3dDevice->EndScene();
	}
	else
	{
		if( !m_pRenderToSurface )
			return;
		m_pRenderToSurface->EndScene();
	}
	PopViewPortArea();
/*
	_R3ENGINE_STATE *state = GetR3State();
	SetViewPortArea((LONG)state->mViewSx,(LONG)state->mViewSy
		,(LONG)state->mViewXL,(LONG)state->mViewYL);
*/
}

