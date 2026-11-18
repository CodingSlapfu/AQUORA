/* SPDX-License-Identifier: MIT; original KIRA-9 cel portrait and phonetic rig.
 * Row-addressed immutable ROM art; no heap, decompression buffer or DSP changes. */
#include "kira_face/face.h"
#include "kira_face/character023.h"
static void layer(uint16_t *p,unsigned y0,unsigned height,unsigned region,int dx,int dy,int tilt){
 const uint16_t *r=k9_art_regions[region];
 for(unsigned row=0;row<r[3];row++){
  int y=(int)r[1]+(int)row+dy;if(y<(int)y0||y>=(int)(y0+height)||y<0||y>=240)continue;
  unsigned pos=k9_art_rows[r[4]+row],used=0;
  while(used<r[2]){
   unsigned n=k9_art_data[pos++];uint16_t color=k9_art_data[pos++];color|=(uint16_t)k9_art_data[pos++]<<8;
   for(unsigned j=0;j<n;j++){int x=(int)r[0]+(int)used+(int)j+dx+((int)r[1]+(int)row-120)*tilt/128;if(x>=0&&x<240)p[(y-y0)*240+x]=color;}used+=n;
  }
 }
}
/* Extract a moving servo plate from the same registered artwork. Only the
 * tiny metal region moves; lips, skin planes and the head remain stable. */
static void plate(uint16_t *p,unsigned y0,unsigned height,int left,int top,int width,int rows,int dx,int dy){
 for(int row=top;row<top+rows;row++){
  int y=row+dy;if(y<(int)y0||y>=(int)(y0+height)||y<0||y>=240)continue;
  unsigned pos=k9_art_rows[row],used=0;
  while(used<240){unsigned n=k9_art_data[pos++];uint16_t color=k9_art_data[pos++];color|=(uint16_t)k9_art_data[pos++]<<8;
   for(unsigned j=0;j<n;j++){int x=(int)(used+j);if(x>=left&&x<left+width&&x+dx>=0&&x+dx<240)p[(y-y0)*240+x+dx]=color;}used+=n;}
 }
}
void k9_face_render(uint16_t *pixels,unsigned y0,unsigned height,const k9_face *f){
 if(y0>=240||!height)return;if(height>240-y0)height=240-y0;
 unsigned background=k9_art_rows[0];
 uint16_t color=k9_art_data[background+1u]|((uint16_t)k9_art_data[background+2u]<<8u);
 for(unsigned i=0;i<height*240u;i++)pixels[i]=color;
 int dx=f->head_x,dy=f->head_y;
 layer(pixels,y0,height,0,dx,dy,f->tilt);
 /* Art-derived lids preserve eyelashes/anatomy. Half-lid is used on both
  * sides of the blink. Emotional eye state follows the engine's real pose. */
 if(f->blink>190)layer(pixels,y0,height,2,dx,dy,f->tilt);
 else if(f->blink>65||f->lid>=3)layer(pixels,y0,height,1,dx,dy,f->tilt);
 else if(f->brow<0||f->cheek==3)layer(pixels,y0,height,3,dx,dy,f->tilt);
 /* Small eye tracking moves the registered eye patch by at most one pixel.
  * Head, skin silhouette and cybernetic neck never wobble with the audio. */
 else if(f->gaze)layer(pixels,y0,height,3,dx+(f->gaze>0?1:-1),dy,f->tilt);
 unsigned mouth=4;
 if(f->mouth!=4&&f->open>2){
  mouth=f->mouth==3?7:f->mouth==2||f->mouth==6?6:f->mouth==5?8:5;
 }else if(f->cheek==1)mouth=9;
 layer(pixels,y0,height,mouth,dx,dy,f->tilt);
 if(f->jaw>2)plate(pixels,y0,height,52,96,8,28,dx,dy+1);
 if(f->neck)plate(pixels,y0,height,109,195,24,29,dx+(f->neck>0?1:-1),dy);
 /* LED clusters sit on the cheek plates and neck hardware. The palette
  * alternates gold and aqua across the modules. */
 if(f->singing_light){
   static const uint8_t modules[][3]={
     {54,102,3},{56,113,2},{184,99,3},{182,109,2},{113,209,2},{121,212,4},{129,209,2}
   };
   unsigned strength=f->singing_light;
   for(unsigned i=0;i<sizeof modules/sizeof modules[0];i++){
     int x=(int)modules[i][0]+dx,y=(int)modules[i][1]+dy;
     unsigned bright=((f->module+i)%4u)!=0u;
     int radius=modules[i][2]-(bright?0:1);
     unsigned level=bright?strength:strength/3u;
     uint16_t halo=i<4?(uint16_t)(((7u+level/24u)<<11u)|((13u+level/9u)<<5u)|3u):
                        (uint16_t)(((14u+level/7u)<<5u)|(8u+level/13u));
     uint16_t core=i<4?0xff4du:0x7fffu;
     for(int iy=-radius-1;iy<=radius+1;iy++){
       int row=y+iy;
       if(row<(int)y0||row>=(int)(y0+height))continue;
       uint16_t *line=pixels+(row-y0)*240u;
       for(int ix=-radius-1;ix<=radius+1;ix++){
         int d=ix*ix+iy*iy,column=x+ix;
         if(d>(radius+1)*(radius+1)||column<0||column>=240)continue;
         line[column]=(d<=radius&&bright)?core:halo;
       }
     }
   }
 }
}

void k9_face_render_canvas(uint16_t *pixels,unsigned y0,unsigned height,const k9_face *pose)
{
 if(y0>=240||!height)return;
 if(height>240-y0)height=240-y0;
 k9_face_render(pixels,y0,height,pose);
 for(unsigned i=0;i<240u*height;i++){
   uint16_t color=pixels[i];
   pixels[i]=(uint16_t)((color>>8u)|(color<<8u));
 }
}
