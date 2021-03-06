//-----------------------------------------------------// 
//
//  GLetraUSW 
//  Clase para dibujado de texto en OpenGL.
//  Usa FreeType2 (www.freetype.org).
// 	
//
//  UnSitioWeb.com  para programar juegos 
//  (cc)by   Vicente J. Fdez.
//
//-----------------------------------------------------//

// Hay que incluir los encabezados de FreeType2 
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_BITMAP_H 
#include FT_TRUETYPE_IDS_H
#include FT_MODULE_H
#include FT_SIZES_H

// Tipo estructura para guardar los datos del bounding box 
// de lo que se escribe
typedef struct USWlimites{
	GLfloat posx; GLfloat posy; GLfloat posyMax;
	GLfloat tamx; GLfloat tamy; GLfloat tamyMax; };

// Enumeracion para indicar donde pintamos el texto respecto 
// a las coordenadas que ponemos
enum USWorigen {USWarriba=0,USWrenglon};

// Clase para pintar texto.
class GLetraUSW {
public:
	// Constructor
	GLetraUSW();
	// Destructor
	~GLetraUSW();
	// Indica a esta fuente que pinte contando con el buffer de profundidad.
	//    si pr es true lo usa
	//	  si pr es false no lo usa (por defecto)
	//    af puede ser un valor float entre 0 y 1, lo normal 0.
	void profundidad(bool pr=false,GLfloat af=0);
	// Indica si las letras proyectaran sombra.
	//   si ss es 0, sin sombra
	//   si ss es mayor de 0 indica como es de oscura la sombra (de 1 a 255)
	//   ox y oy marcan la separacion en pixels entre la letra y la sombra.
	//       pueden ser negativos.
	// Si se quiere tener sombra hay que usar esta funcion antes de cargar
	// la fuente.
	void sombra(unsigned char ss=0, GLfloat ox=5, GLfloat oy=5);
	// Indica el color de las letras en RGB (0 a 255)
	//  Componentes del color de la letra 
	//	  lr-rojo  lg-verde  lb-azul
	//  Componentes del color del borde
	//    br-rojo  bg-verde  bb-azul
	// Si las letras tienen borde, hay que poner el color antes de cargar la 
	// fuente, despues no se puede cambiar.
	void color(int lr=255, int lg=255, int lb=255, int br=0, int bg=0, int bb=0);
	// Indica el tamaño de la fuente y del borde.
	//    tama es el tamaño
	//    bord el grosor del borde (0 es que no hay borde)
	// Solo se puede usar antes de cargar la fuente.
	void tamano(int tama=20, int bord=0);
	// Giro en grados de la letra a pintar
	//   gir- grados a girar
	//   sum- si gir es desde 0 o se suma a lo que ya habia
	void gira(int gir=0, bool sum=false);
	// Donde se pintan las letras en relacion a la posicion Y
	//   si or es USWarriba la posicion Y corresponde a la parte de arriba de las letras
	//   si or es USWrenglon la posicion Y corresponde al renglon donde se escribe la letra
	void origen(USWorigen or);
	// Posicion donde pintar el proximo texto (desde la esquina superior izquieda)
	//   posx y posy son las coordenadas en pixels
	//   posz es la profundidad
	//   sum es si las coordenadas se suman a las anteriores o son absolutas
	void posicion(GLfloat posx=0, GLfloat posy=0, GLfloat posz=0, bool sum=false);
	// Modifica la posicion donde se escribira el texto
	void continuacion(GLfloat posx=0, GLfloat posy=0, GLfloat posz=0);
	// Cambia la posicion del proximo texto a la linea siguiente (nueva linea)
	//    ls es al numero de lineas
	void nl(int ls=1);
	// Para saber el bounding box (los limites de texto)
	//    USWlimites es un 
	void limites( USWlimites *lim, char *cadena=NULL,... );
	// Devuelve el tamaño en pixels del alto de la fuente
	int maxtamy(void);
	// Escribe texto
	void escribe(char *cadena=NULL,...);
	// Escribe texto en unas coordenadas
	void escribe(GLfloat x, GLfloat y, char *cadena=NULL,...);
	// Carga fuente de disco o memoria
	int carga(void *fich, unsigned long sz=0); 
	// Almacena el texto de los errores si los hay.
	char Error[200];





private:
	void escrib(char *cadena,GLfloat offx=0, GLfloat offy=0);
	int genera();
	int generachar(FT_ULong  c, bool s=false);

	FT_Library libFT; 
	FT_Face fuente; 
	FT_Bitmap bitmap;
	struct medidas{
		GLint posx; GLint posy;
		GLint tamx;	GLint tamy;
		GLint ttamx; GLint ttamy;
		GLint beary; GLint bearx;
		GLint avance;
	}medir[256];
	
	USWorigen orig;
	bool prof; GLfloat alphf;
	bool cont; int ultchar;
	unsigned char som;
	unsigned int tam, tam2, borde, bitpp;
	GLfloat ff;
	unsigned int tambmp, tamtex,bitx,bity;
	int maxx,maxy,maxbeary,maxbby,difbeary;
	unsigned char borr,borg,borb;
	unsigned char letr,letg,letb;
	GLfloat posx,posy,posz,giro;
	GLfloat oposx, oposy;
	GLfloat soffx, soffy;
	GLfloat bbposY, bbposX;
	GLfloat ultamX,ultamY;
	GLfloat ulpospenX;
	unsigned char *buf; 
	unsigned char *bit; 
	int *kerning;
	GLuint textur, texsom;
	GLuint vboVertices;
	GLuint vboTextura;
};

GLetraUSW::~GLetraUSW()
{
	glDeleteBuffersARB(1, &vboVertices);   
	glDeleteBuffersARB(1, &vboTextura);   
	if(buf!=NULL) {delete []buf; buf=NULL; }
	if(bit!=NULL) {delete []bit; bit=NULL; }
	if(kerning!=NULL) {delete []kerning; kerning=NULL; }
	glDeleteTextures( 1, &textur );
	if(texsom) glDeleteTextures( 1, &texsom );
};

GLetraUSW::GLetraUSW()
{
	int i;
	memset(medir,0,sizeof(medidas)*256);
	orig=USWarriba;
	textur=0; texsom=0;
	posx=0; posy=0; posz=0, giro=0;
	oposx=0; oposy=0;
	ultamX=0; ultamY=0; bbposY=0; bbposX=0;
	borr=0; borg=0; borb=0;
	letr=255; letg=255; letb=255;
	tam=20; tam2=20; ff=1;
	buf=NULL;
	bit=NULL;
	kerning=NULL;
	Error[0]='\0';
	som=0; soffx=5; soffy=5;
	prof=false; alphf=0;
	cont=false; ultchar=32;
};

void GLetraUSW::profundidad(bool pr,GLfloat af)
{	prof=pr; alphf=af; };

void GLetraUSW::sombra(unsigned char ss, GLfloat ox, GLfloat oy) 
{ 
	if(textur && !texsom) return;
	som=ss; soffx=ox; soffy=oy;
};

void GLetraUSW::color(int lr, int lg, int lb, int br, int bg, int bb)
{
	borr=br; borg=bg; borb=bb;
	letr=lr; letg=lg; letb=lb;
};

void GLetraUSW::origen(USWorigen or) { orig=or; };

void GLetraUSW::posicion(GLfloat px, GLfloat py, GLfloat pz, bool sum)
{
	if(Error[0]!='\0') { return; }
	cont=false; ulpospenX=0;
	posx-=ultamX;
	if(sum) { posx+=px; posy+=py; posz+=pz; }
	else { posx=px; posy=py; posz=pz; }
	oposx=posx; oposy=posy;

};

void GLetraUSW::continuacion(GLfloat px, GLfloat py, GLfloat pz)
{
	if(Error[0]!='\0') { return; }
	posx+=ulpospenX+px; posy+=py; posz+=pz; cont=true;
	
};

void GLetraUSW::nl(int ls)
{
	posx=oposx; posy=posy+(float)(maxbby*ls);
}

void GLetraUSW::tamano(int tama, int bord)
{
	if(textur) { return; };
	borde=bord; tam=tama-(borde*2); 
	tam2=tam;
	if(tam>120) tam=120;

};

int GLetraUSW::maxtamy(void) {return (int)((maxbby));};

void GLetraUSW::gira(int gir, bool sum) 
{
	if(Error[0]!='\0') { return; }
	if(sum) giro+=gir;
	else giro=gir;
};

void GLetraUSW::escribe(GLfloat x, GLfloat y, char *cadena,...)
{	if(!textur) return;
	if(Error[0]!='\0') {  return; }
	if(cadena==NULL) return;
	unsigned char ss2;
	va_list	ap;
	char aux[512];	
	va_start(ap, cadena);									
	vsprintf(aux, cadena, ap);						
	va_end(ap);	
	oposx=posx=x; oposy=posy=y;
	
	if(som>0) { escrib(aux,soffx,soffy); };
	ss2=som;som=0; escrib(aux); som=ss2;
	
	continuacion();
	
};

void GLetraUSW::escribe(char *cadena,...)
{	if(!textur) return;
	if(Error[0]!='\0') {  return; }
	if(cadena==NULL) return;
	unsigned char ss2;
	va_list	ap;
	char aux[512];	
	va_start(ap, cadena);									
	vsprintf(aux, cadena, ap);						
	va_end(ap);	
	
	if(som>0) { escrib(aux,soffx,soffy); };
	ss2=som;som=0; escrib(aux); som=ss2;
	
	continuacion();
};

void GLetraUSW::escrib(char *cadena, GLfloat offx, GLfloat offy)
{
	if(cadena[0]=='\0') return;

	int i,maxbyt=0; difbeary=0;
	GLfloat temp1,temp2,tempx=0,tempy=0; 
	GLfloat maxtx=0,maxty=0, primbearx=0;
	ultamX=0; ultamY=0;
	GLfloat vp[4];
	int ker=0;
			
	wchar_t unicode[512];
	//MultiByteToWideChar(CP_ACP, 0, cadena, -1, unicode, 512);
	
	size_t sx;
	const char *cx=cadena;
	sx=mbsrtowcs(NULL,&cx,0,NULL);
	mbsrtowcs(unicode,&cx,sx+1,NULL);

	bool fluz = glIsEnabled(GL_LIGHTING);
	bool ftex = glIsEnabled(GL_TEXTURE_2D);
	bool fpro = glIsEnabled(GL_DEPTH_TEST);
	if(prof) glEnable(GL_DEPTH_TEST);
	else glDisable(GL_DEPTH_TEST);
	if(prof) { glEnable(GL_ALPHA_TEST); glAlphaFunc(GL_GREATER,alphf); }

	glDisable(GL_LIGHTING);
	glMatrixMode(GL_TEXTURE);
	glScalef(1.0/(float)tamtex,1.0/(float)tamtex,1);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glGetFloatv(GL_VIEWPORT,vp);
	glOrtho(0,vp[2],vp[3],0,-4,4);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	
	primbearx-=(float)(medir[unicode[0]].bearx);
	maxtx+=primbearx;
	tempx=posx-oposx+primbearx; 
	if(orig==USWarriba) tempy=posy-oposy;  
	else tempy=posy-oposy-((float)maxbeary);  


	GLfloat corr=0; // para corregir el ulpospenX
	// preparo la posicion segun la anterior cadena
	if(cont){
		if(kerning!=NULL) ker=kerning[(ultchar*255)+unicode[0]]; 
		else ker=0;
		corr=((float)medir[unicode[0]].bearx+(float)ker);
		tempx+=(corr); 
		}
	
	glTranslatef(oposx,oposy,0); 
	if(giro!=0) glRotatef(giro,0,0,1);
	if(som>0) glTranslatef(tempx+soffx,tempy+soffy,posz);
	else glTranslatef(tempx,tempy,posz);
	
	//if(giro!=0) glRotatef(giro,0,0,1);
	
	if(borde==0) glColor3ub(letr,letg,letb);
	else glColor3ub(255,255,255);
	glEnable(GL_BLEND);
	if(som>0) glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_ALPHA);
	else glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboVertices); 
	glVertexPointer(3, GL_FLOAT,sizeof(GLfloat)*3, 0); 
	glActiveTexture(GL_TEXTURE0);
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboTextura); 
	glTexCoordPointer(2, GL_FLOAT,sizeof(GLfloat)*2, 0);
	glEnable(GL_TEXTURE_2D);	
	if(som>0) glBindTexture(GL_TEXTURE_2D, texsom);
	else glBindTexture(GL_TEXTURE_2D, textur);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

	//busca el beary mayor de esta cadena
	for(i=0;unicode[i]!='\0';i++)
	{
		if(unicode[i]==L'€') { unicode[i]=1; }  
		if(unicode[i]>255) { unicode[i]=L'#'; }
		if(medir[unicode[i]].beary>maxbyt) maxbyt=medir[unicode[i]].beary;
		
	}
	difbeary=(maxbeary-maxbyt);

	for(i=0;unicode[i]!='\0';i++)
	{
		if(kerning!=NULL){
			if(unicode[i+1]=='\0') ker=0;
			if(kerning[(unicode[i]*255)+unicode[i+1]]>100 || kerning[(unicode[i]*255)+unicode[i+1]]<-100 ) ker=0;
			else ker=(float)kerning[(unicode[i]*255)+unicode[i+1]]; } 
		else ker=0;
		 
		glPushMatrix();
		temp2=((float)maxbyt)-((float)medir[unicode[i]].beary);
		glTranslatef((float)(medir[unicode[i]].bearx),temp2+((float)difbeary),0.0f);
		glDrawArrays(GL_QUADS, (unicode[i]*4), 4 );
		glPopMatrix();		
		temp1=((float)(medir[unicode[i]].avance)+(float)ker+((float)borde*2)); 
 		glTranslatef(temp1,0.0f,0.0f); 
		
		//calculo tamaño boundinbox de lo que escribo
		temp2+=(float)medir[unicode[i]].tamy;
		if(maxty<temp2) maxty=temp2;
		maxtx+=temp1;
	
	};
	ultchar=unicode[i-1];
	if(som==0) {  cont=false; }

	int zz=((((medir[ultchar].tamx)+(medir[ultchar].bearx)-borde)-(medir[ultchar].avance)));
	ultamX=maxtx+(float)zz;  
	ultamY=maxty; // tamaño del ultimo boundingbox
	ulpospenX=maxtx+(corr);
	bbposX=posx;

	if(orig==USWarriba) bbposY=difbeary;
	else bbposY=-maxbyt;

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0); 
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glColor3ub(255,255,255);

	glPopMatrix();
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_ALPHA_TEST);
	if(fluz) glEnable(GL_LIGHTING);
	if(ftex) glEnable(GL_TEXTURE_2D); else glDisable(GL_TEXTURE_2D);
	if(fpro) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
};

void GLetraUSW::limites(USWlimites *lim, char *cadena,... )
{
	if(Error[0]!='\0') { return; }
	if(cadena==NULL) 
	{
		lim->tamx=ultamX; lim->tamy=ultamY; lim->tamyMax=(float)maxbby;
		lim->posx=bbposX; lim->posy=posy+bbposY; lim->posyMax=posy+bbposY-difbeary;
		return;
	}
	
	int i,maxbyt=0,difbeary=0;
	va_list	ap;
	char aux[512];	
	GLfloat temp1,temp2,tempx=0; 
	GLfloat maxtx=0,maxty=0, primbearx=0;
	int ker=0;

	va_start(ap, cadena);									
	vsprintf(aux, cadena, ap);						
	va_end(ap);					
	wchar_t unicode[512];
	//MultiByteToWideChar(CP_ACP, 0, aux, -1, unicode, 512);

	size_t sx;
	const char *cx=cadena;
	sx=mbsrtowcs(NULL,&cx,0,NULL);
	mbsrtowcs(unicode,&cx,sx+1,NULL);

	primbearx-=(float)(medir[unicode[0]].bearx);
	maxtx+=primbearx;

	//busca el beary mayor de esta cadena
	for(i=0;unicode[i]!='\0';i++){
	//for(i=0;i<loncad;i++){
		if(unicode[i]==L'€') { unicode[i]=1; }  
		if(unicode[i]>255) { unicode[i]=L'#'; }
		if(medir[unicode[i]].beary>maxbyt) maxbyt=medir[unicode[i]].beary;
		}
	difbeary=maxbeary-maxbyt;

	for(i=0;unicode[i]!='\0';i++)
	{
		if(unicode[i]==L'€') { unicode[i]=1; }  
		if(unicode[i]>255) { unicode[i]='#'; }

		if(kerning!=NULL){
			if(unicode[i+1]=='\0') ker=0;
			if(kerning[(unicode[i]*255)+unicode[i+1]]>100 || kerning[(unicode[i]*255)+unicode[i+1]]<-100 ) ker=0;
			else ker=kerning[(unicode[i]*255)+unicode[i+1]]; }
		else ker=0;

		temp2=((float)maxbyt)-((float)medir[unicode[i]].beary);
		temp1=(float)(medir[unicode[i]].avance)+(float)ker+((float)borde);
		temp2+=(float)medir[unicode[i]].tamy;
		if(maxty<temp2) maxty=temp2;
		maxtx+=temp1;	
	};
	int ultchar=(int)unicode[i-1];

	int zz=((((medir[ultchar].tamx)+(medir[ultchar].bearx)-borde)-(medir[ultchar].avance)));
	lim->tamx=maxtx+(GLfloat)zz; 
	lim->tamy=maxty;
	lim->tamyMax=(float)maxbby;
	lim->posx=posx; 
	
	if(orig==USWarriba) { lim->posy=posy+difbeary; lim->posyMax=posy;}
	else { lim->posy=posy-maxbyt; lim->posyMax=posy-maxbyt-difbeary;}
};


int GLetraUSW::generachar(FT_ULong  c, bool s)
{
	FT_Error error;
	FT_UInt glyph_index;
	unsigned int i,j;
	int ir,ig,ib;
	unsigned char i3,it;
	float pc1,pc2;
	int bmrows,bmwidth;
	
	memset(buf, 0, bitpp*tambmp*tambmp);
	memset(bit, 0, bitpp*tambmp*tambmp);

	glyph_index = FT_Get_Char_Index( fuente, c ); 
	if(glyph_index==0) glyph_index = FT_Get_Char_Index( fuente, L'#' );

	error = FT_Load_Glyph( fuente, glyph_index, FT_LOAD_DEFAULT );
	if ( error ) { sprintf(Error,"Error cargando glifo: %c",c); return 1; }
	
	error = FT_Render_Glyph( fuente->glyph, FT_RENDER_MODE_NORMAL );
	if ( error ) { sprintf(Error,"Error renderizando glifo: %c",c); return 1; }
	error = FT_Bitmap_Copy(libFT,&fuente->glyph->bitmap,&bitmap);
	if ( error ) { sprintf(Error,"Error copiando bitmap: %c",c); return 1; }
	bity=bmrows=bitmap.rows; bitx=bmwidth=bitmap.width;
	
	if(borde==0 || s)
	{
		if(borde>0) { FT_Bitmap_Embolden(libFT, &bitmap, borde*2*64, borde*2*64);
					bity=bmrows=bitmap.rows; bitx=bmwidth=bitmap.width; }
		for(i=0;i<(bitmap.rows);i++)
			for(j=0;j<(bitmap.width);j++) {
				it=*((bitmap.buffer)+((i*bitmap.width)+(j)));	
				if(s) if(it>som) it=som; 
				*(buf+((i)*tambmp)+((j)))=it;  }
		FT_Bitmap_Done(libFT,&bitmap);
		return 0;
	}

	for(i=0;i<(bitmap.rows);i++)
		for(j=0;j<(bitmap.width);j++)
			*(bit+((i)*tambmp)+((j)))=*((bitmap.buffer)+((i*bitmap.width)+(j)));			

	FT_Bitmap_Embolden(libFT, &bitmap, borde*2*64, borde*2*64);

	for(i=0;i<(bitmap.rows);i++)
	{
		for(j=0;j<(bitmap.width);j++)
		{
			it=*((bitmap.buffer)+((i*bitmap.width)+(j)));	
			
			if( it > 0 )
			{
				*(buf+(i*tambmp*4)+(j*4)+3)=it;
				*(buf+(i*tambmp*4)+(j*4)+0)=borr;
				*(buf+(i*tambmp*4)+(j*4)+1)=borg;
				*(buf+(i*tambmp*4)+(j*4)+2)=borb;
			}				
		}
	}
	int dr=(bitmap.rows-bmrows)/2;
	int dw=(bitmap.width-bmwidth)/2;

	for(i=0;i<(bmrows);i++)
	{
		for(j=0;j<(bmwidth);j++)
		{
			it=(*((bit)+((i*tambmp)+(j))));					
			if( it > 0 )
			{
				pc1=1.0-((float)it/255.0f);
				ir=letr;ig=letg;ib=letb;
				if(it<255){
					i3=*(buf+((i+dr)*tambmp*4)+((j+dw)*4)+0); ir= ir-((ir-i3)*pc1);   
					i3=*(buf+((i+dr)*tambmp*4)+((j+dw)*4)+1); ig= ig-((ig-i3)*pc1); 
					i3=*(buf+((i+dr)*tambmp*4)+((j+dw)*4)+2); ib= ib-((ib-i3)*pc1);  
					}
				*(buf+((i+dr)*tambmp*4)+((j+dw)*4)+0)=ir;
				*(buf+((i+dr)*tambmp*4)+((j+dw)*4)+1)=ig;
				*(buf+((i+dr)*tambmp*4)+((j+dw)*4)+2)=ib;
			}					
		}
	}

	bitx=bitmap.width; bity=bitmap.rows;
	FT_Bitmap_Done(libFT,&bitmap);
	return 0;
};
int GLetraUSW::carga(void *fich,unsigned long sz)
{	
	if(textur) return 1;
	int i;
	FT_Error error;
	// inicializo FreeType y cargo fuente
	error = FT_Init_FreeType( &libFT ); 
	if ( error ) { sprintf(Error,"Error al inicializar"); return 1; }
	
	//MessageBoxA(NULL,(char*)fich,"a",MB_OK);
	if(sz==0) error = FT_New_Face( libFT, (char*)fich, 0, &fuente ); 
				//if ( error ) sprintf(Error,"Error al cargar fichero"); return 1; }
	else error = FT_New_Memory_Face( libFT, (unsigned char*)fich, sz, 0, &fuente );
	if ( error == FT_Err_Unknown_File_Format ) {sprintf(Error,"Error de formato de fichero"); return 1;}
	else if ( error ) { sprintf(Error,"Error al cargar fichero"); return 1; }
	
	error = FT_Select_Charmap( fuente, FT_ENCODING_UNICODE );
	if(error) { sprintf(Error,"La codificacion no es unicode"); return 1; }
	/*
	FT_CharMap found = 0;
	FT_CharMap charmap;
	 for ( i = 0; i < fuente->num_charmaps; i++ ) 
	 { 
		 charmap = fuente->charmaps[i]; 
		 sprintf(mensaje[i],"pl: %i - en: %i - %d",charmap->platform_id,charmap->encoding_id, charmap->encoding);
		 if ( charmap->platform_id == 3 && charmap->encoding_id == 1 ) 
			{ found = charmap; break; } 
	 } 
	if(!found){ sprintf(Error,"La codificacion no es unicode"); return 1; }
	error = FT_Set_Charmap( fuente, found ); 
	if ( error ) { sprintf(Error,"La codificacion no es unicode"); return 1; }
	*/

	//error = FT_Set_Pixel_Sizes(fuente,0,tam);
	//if ( error ) { sprintf(Error,"La fuente no es escalable"); return 1; }
	
	//genero fuente en textura opengl. Si se sale de la textura maxima (2048x2048) reduzco el tamaño y reintento.
	int err=1;
	while( tam>=6 && err==1)
	{
		error=FT_Set_Pixel_Sizes(fuente,0,tam);
		if ( error ) { sprintf(Error,"La fuente no es escalable"); return 1; }
		err=genera();
		if(err==1) { tam-=5; Error[0]=0; } 
	};
	if(tam<6) sprintf(Error,"Tamaño muy pequeño"); 
	return err;
};

int GLetraUSW::genera()
{	
	FT_ULong  charcode; 
	FT_Glyph gly;
	FT_UInt glyph_index;
	FT_BBox bbox; 
	int i,j;
	int tl=0; int tc=0;  
	FT_UInt indice1,indice2;
	FT_Vector delta;
	maxx=0;maxy=0;maxbeary=0,maxbby=0;

	// busco el tamaño maximo de glifo, X e Y, para el tamaño de fuente pedido
	float temby=0,temmax=0;
	charcode=32; 
	for(i=32;i<=255;i++)
	{	
		glyph_index = FT_Get_Char_Index( fuente, charcode );  
		if(glyph_index==0) glyph_index = FT_Get_Char_Index( fuente, L'#' );
		FT_Load_Glyph( fuente, glyph_index, FT_LOAD_DEFAULT );
		FT_Get_Glyph( fuente->glyph, &gly );
		FT_Glyph_Get_CBox( gly, FT_GLYPH_BBOX_PIXELS, &bbox );
		FT_Done_Glyph(gly); 

		temby=(float)(fuente->glyph->metrics.horiBearingY/64);
		if(temby>temmax) temmax=temby; 
		 
		if(maxx<bbox.xMax-bbox.xMin) { maxx=bbox.xMax-bbox.xMin;  }
		if(maxy<bbox.yMax-bbox.yMin) { maxy=bbox.yMax-bbox.yMin;  }  
		charcode++;
	}
	ff=(float)tam2/(float)temmax;

	// decido tamaño de textura general en base al tamaño maximo de glifo
	if( maxx+(borde*2)<16 && maxy+(borde*2)<16 ) tambmp=16;
	else if( maxx+(borde*2)<32 && maxy+(borde*2)<32 ) tambmp=32;
	else if( maxx+(borde*2)<64 && maxy+(borde*2)<64 ) tambmp=64;
	else if( maxx+(borde*2)<128 && maxy+(borde*2)<128 ) tambmp=128;
	else { sprintf(Error,"Tamaño excesivo"); return 1; } 
	tamtex=tambmp*16;

	// Si la fuente trae kerning genero una tabla para usar al imprimir
	if(FT_HAS_KERNING( fuente )) 
	{	
		kerning=new int[255*255];
		for(i=0;i<255;i++)
		{	
			if(i==1) indice1=FT_Get_Char_Index( fuente, L'€' );
			else indice1=FT_Get_Char_Index( fuente, i );
			if(indice1==0) indice1=FT_Get_Char_Index( fuente, L'#' );
			for(j=0;j<255;j++)
			{	
				if(j==1) indice2=FT_Get_Char_Index( fuente, L'€' );
				else indice2=FT_Get_Char_Index( fuente, j );
				if(indice2==0) indice2=FT_Get_Char_Index( fuente, L'#' );
				FT_Get_Kerning( fuente, indice1, indice2, 0, &delta );	
				kerning[(i*255)+j] = ((float)(delta.x/64))*ff;
			}
		}
	}
	
	// bites por pixel con borde(4) y sin borde(1)
	if(borde>0) bitpp=4;
	else bitpp=1;
		
	// Creo bufferes temporales
	buf=new unsigned char[(bitpp*tambmp*tambmp)];
	bit=new unsigned char[(bitpp*tambmp*tambmp)]; 

	// Creo textura general
	glGenTextures(1, &textur); glBindTexture(GL_TEXTURE_2D, textur);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	//glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);
	if(borde>0) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tamtex, tamtex, 
			0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	else glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, tamtex, tamtex, 
			0, GL_ALPHA, GL_UNSIGNED_BYTE, 0);

	if(som>0) 
	{
		glGenTextures(1, &texsom); glBindTexture(GL_TEXTURE_2D, texsom);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, tamtex, tamtex, 
			0, GL_ALPHA, GL_UNSIGNED_BYTE, 0);
	}
	
	// Genero caracteres ASCII del 32 al 255 y los cargo en la textura general       
	charcode=32;   // 32                                       
	for(tl=2;tl<16;tl++)   //2
	{   
		for(tc=0;tc<16;tc++)
		{
		if(1==generachar(charcode)) return 1;
		glBindTexture(GL_TEXTURE_2D, textur);
		if(borde>0 ) 
			glTexSubImage2D(GL_TEXTURE_2D, 0, 
			tc*tambmp, tl*tambmp, tambmp,tambmp,
			GL_RGBA, GL_UNSIGNED_BYTE, buf); 
		else glTexSubImage2D(GL_TEXTURE_2D, 0, 
			tc*tambmp, tl*tambmp, tambmp,tambmp,
			GL_ALPHA, GL_UNSIGNED_BYTE, buf); 
		
		if(som>0)
		{
		if(1==generachar(charcode, true)) return 1;
		glBindTexture(GL_TEXTURE_2D, texsom);
		//if(borde>0 && som==0 ) 
		glTexSubImage2D(GL_TEXTURE_2D, 0, 
			tc*tambmp, tl*tambmp, tambmp,tambmp,
			GL_ALPHA, GL_UNSIGNED_BYTE, buf); 
		}
		medir[charcode].posx=tc*tambmp;
		medir[charcode].posy=tl*tambmp;
		medir[charcode].tamx=(float)bitx*ff; 
		medir[charcode].tamy=(float)bity*ff;  
		medir[charcode].ttamx=bitx; 
		medir[charcode].ttamy=bity;  
		medir[charcode].beary=((float)(fuente->glyph->metrics.horiBearingY/64)+((float)borde*2))*ff;
		medir[charcode].bearx=((float)(fuente->glyph->metrics.horiBearingX/64)-((float)borde))*ff;
		medir[charcode].avance=((float)(fuente->glyph->metrics.horiAdvance/64)*ff);
		
		if(maxbeary<medir[charcode].beary) maxbeary=medir[charcode].beary;
		charcode++;
		}  
	}
	// TAB
	medir[9].avance=medir[32].avance*4;

	// Genero caracteres mas alla de 255 (unicode)
	generachar(L'€');
	glBindTexture(GL_TEXTURE_2D, textur);
	if(borde>0) glTexSubImage2D(GL_TEXTURE_2D, 0, 
			1*tambmp, 1*tambmp, tambmp,tambmp,
			GL_RGBA, GL_UNSIGNED_BYTE, buf); 
	else glTexSubImage2D(GL_TEXTURE_2D, 0, 
			1*tambmp, 1*tambmp, tambmp,tambmp,
			GL_ALPHA, GL_UNSIGNED_BYTE, buf); 
	if(som>0)
		{
		generachar(L'€', true);
		glBindTexture(GL_TEXTURE_2D, texsom);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 
			1*tambmp, 1*tambmp, tambmp,tambmp,
			GL_ALPHA, GL_UNSIGNED_BYTE, buf); 
		}
	medir[1].posx=1*tambmp;
	medir[1].posy=1*tambmp;
	medir[1].tamx=(float)bitx*ff; 
	medir[1].tamy=(float)bity*ff; 
	medir[1].ttamx=bitx; 
	medir[1].ttamy=bity;  
	medir[1].beary=((float)(fuente->glyph->metrics.horiBearingY/64)+((float)borde*2))*ff;
	medir[1].bearx=((float)(fuente->glyph->metrics.horiBearingX/64)-((float)borde))*ff;
	medir[1].avance=((float)(fuente->glyph->metrics.horiAdvance/64))*ff;

	// borro bufferes temporales
	if(buf!=NULL) {delete []buf; buf=NULL; }
	if(bit!=NULL) {delete []bit; bit=NULL; }
	// borro objeto fuente y objeto freetype
	FT_Done_Face(fuente);
	FT_Done_Library(libFT);

	// Busco el maximo alto del bounding box
	int cont=0;
	int temp;
	for(i=0;i<256;i++)
	{
		temp=maxbeary-medir[i].beary;
		temp+=medir[i].tamy;
		if(maxbby<temp) maxbby=temp;
	}
	borde=(float)borde*ff;
	
	// genero VBOs
	GLfloat *Cpuntos;
	GLfloat *Ctextura;
	Cpuntos=new GLfloat[256*12];
	Ctextura=new GLfloat[256*8];

	for(i=0;i<255;i++)
		{
			Cpuntos[(i*12)+0]=0; Cpuntos[(i*12)+1]=0; Cpuntos[(i*12)+2]=0;
			Cpuntos[(i*12)+3]=0; Cpuntos[(i*12)+4]=medir[i].tamy; Cpuntos[(i*12)+5]=0;
			Cpuntos[(i*12)+6]=medir[i].tamx; Cpuntos[(i*12)+7]=medir[i].tamy; Cpuntos[(i*12)+8]=0;
			Cpuntos[(i*12)+9]=medir[i].tamx; Cpuntos[(i*12)+10]=0; Cpuntos[(i*12)+11]=0;

			Ctextura[(i*8)+0]=medir[i].posx; Ctextura[(i*8)+1]=medir[i].posy;
			Ctextura[(i*8)+2]=medir[i].posx; Ctextura[(i*8)+3]=medir[i].posy+medir[i].ttamy/*ff*/;
			Ctextura[(i*8)+4]=medir[i].posx+medir[i].ttamx/*ff*/; Ctextura[(i*8)+5]=medir[i].posy+medir[i].ttamy/*ff*/;
			Ctextura[(i*8)+6]=medir[i].posx+medir[i].ttamx/*ff*/; Ctextura[(i*8)+7]=medir[i].posy;
			
		}
		
	glGenBuffersARB(1, &vboVertices);
	glGenBuffersARB(1, &vboTextura);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboVertices);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(GLfloat)*256*12, Cpuntos, GL_STATIC_DRAW_ARB);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboTextura);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(GLfloat)*256*8, Ctextura, GL_STATIC_DRAW_ARB);
	if(Cpuntos!=NULL) {delete []Cpuntos; Cpuntos=NULL; }
	if(Ctextura!=NULL) {delete []Ctextura; Ctextura=NULL; }
	return 0;
};
